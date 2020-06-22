/**
 * @brief:
 *          读取csv文件
 *          写入csv文件
 *          利用模板进行类型转换
 *          利用hash进行查找表头
 * @Useage: Parser d(filepath);
 *          cout<<d[1]["column1"]<<d[1].get_value<double>("column1");
 *          cout<<d[1][0]<<d[1].get_value<double>(0);
 *          d[1].set_value<>(1)
 * @author:   李昊佳
 * @date:     2020.6.1
**/
#pragma once
#ifndef CSV_FILE_H
#define CSV_FILE_H


#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>

namespace csv
{
    ////------错误处理-------
    class Error : public std::runtime_error
    {
    public:
        Error(const std::string& msg) :
            std::runtime_error(std::string("CSVfile : ").append(msg)) {}
    };

    //------row 数据--------
   // 原因：1.可以重载运算符 2.便于查找 3.可以当成一个数据成为新的vector，同时实现动态长度 
   //
    class Row
    {
    public:
        char _sep = ',';
        //初始化 
        Row(const std::vector<std::string>& values) : _values(values) { _column_index = NULL; _column = NULL; };  //只初始化数据
        Row(const std::vector<std::string>* col, const std::unordered_map<std::string, unsigned long int>* col_idx) //初始化表头
            :_column(col), _column_index(col_idx) {};
        Row(const std::vector<std::string>* col, const std::unordered_map<std::string, unsigned long int>* col_idx, const std::vector<std::string>& values)
            :_column(col), _column_index(col_idx), _values(values) {}; //初始化表头和数据
        Row() { _column_index = NULL; _column = NULL; };    //使用系统默认
        ~Row(void) = default;
        //类内定义 默认内联函数，提升效率

       //返回含有的变量个数
        unsigned int size(void)
        {
            return _values.size();
        }
        //根据列名，返回列的序号
        const unsigned long int get_column_idx(const std::string& valueName);
        //添加一列
        void push(const std::string& value)
        {
            _values.emplace_back(value);
        }
        //删除一个数据
        //调用这个方法需要保证其表头和_values严格对应
        void delete_value(const std::string& valueName)
        {
            delete_value(get_column_idx(valueName));
        }
        void delete_value(const unsigned long int idx);


        //均不需要考虑类型转换，函数内部自动进行  需要类型支持<< >>运算符
        //使用模板，体现多态性,C++不支持模板分离编译，会链接不到“error LNK2019：无法解析的外部符号”

        //根据序号访问并进行类型转换
        template<typename T>
        T get_value(const unsigned long int valuePosition)
        {
            if (valuePosition < _values.size())
            {
                T res;
                std::stringstream ss;
                ss.imbue(std::locale(std::locale::classic(), "", std::locale::ctype));
                ss << _values[valuePosition];
                ss >> res;
                return res;
            }
            throw Error("can't return this value (doesn't exist)");
        }
        //根据列名访问并进行类型转换
        template<typename T>
        T get_value(const std::string& valueName)
        {
            return get_value<T>(get_column_idx(valueName));
        }

        //根据序号设定数据 需要指定传入数据类型
        template<typename T>
        bool set(unsigned long int idx, const T& value)
        {
            //_column 不一定存在
            /*if (idx > _column.size())
            {
                //超出column范围
                throw Error("Set values out of column");
                return false;
            }*/
            //进行类型转化
            std::ostringstream  ss;
            ss.imbue(std::locale(std::locale::classic(), "", std::locale::ctype));
            ss << value;
            //if (idx < _column.size())
                _values[idx] = ss.str();
           // else
              //  _values.emplace_back(ss.str());
            return true;
        }

        //根据列名设定数据 需要指定传入数据类型
        template<typename T>
        bool set(const std::string& valueName, const T& value)
        {
            return set(get_column_idx(valueName), value);
        }

    public: //运算符重载

        //根据序号访问(string 可以直接修改值)
        std::string& operator[](unsigned long int valuePosition)
        {
            if (valuePosition < _values.size())
                return _values[valuePosition];
            throw Error("can't return this value (doesn't exist)");
        }
        //根据列名访问(string 可以直接修改值)
        std::string& operator[](const std::string& valueName)
        {
            return _values[get_column_idx(valueName)];
        }
        //根据序号访问并进行类型转换
        /*template<typename T>
        const T operator[](unsigned long int valuePosition) const
        {
            return get_value<T>(valuePosition);
        }
        //根据列名访问
        template<typename T>
        const T operator[](const std::string& valueName) const
        {
            return get_value<T>(valueName);
        }*/
        //屏幕输出重载
        friend std::ostream& operator<<(std::ostream& os, const Row& row);
        //文件输出重载，用以保存文件
        friend std::ofstream& operator<<(std::ofstream& os, const Row& row);

    private:
        std::vector<std::string> _values;       //数据
        const std::vector<std::string>* _column;      //表头字符串信息  用指针方式实现公共表头
        const std::unordered_map<std::string, unsigned long int>* _column_index;  //构建hash表加快表头查找速度  用指针方式实现公共表头
    };


    ////------csv 获取转化-------- 
    class Parser
    {

    public:
        //打开分析一个csv文件，path代表路径 sep代表分隔符  parseheader表示是否分析表头
        Parser(char sep = ',', bool parseheader = true, bool index=true) :_sep(sep), _parseheader(parseheader),_index(index) 
        {
        };
        Parser(const std::string& path, char sep = ',',bool parseheader=true, bool index = true);
        //如果带表头的话，不存在文件情况下直接自动创建文件，存在的话忽略传入的表头，以文件为准
        Parser(const std::string& path, std::vector<std::string> headers, char sep=',', bool parseheader=true, bool index = true);
        ~Parser(void);

    public:
        //创建一个新的文件  注意这个操作只是在内存中保存，需要调用sync()来真正保存到硬盘上
        void create_file(const std::string& path,std::vector<std::string>& headers);
        //按照路径加载并分析文件
        bool load_file(const std::string& path);
        //获取一行数据
        Row& get_row(unsigned long long int row);
        //获取一行数据
        Row& get_row(std::string name);
        //获取总行数
        unsigned long long int rowcount(void) const;
        //获取总列数
        unsigned long int columncount(void) const;
        //获取表头
        std::vector<std::string> get_header(void) const;
        //根据表头名获取表头序号
        unsigned long int get_header_idx(std::string name);
        //获取表头元素
        const std::string get_header_element(unsigned long int pos) const;
        //更新表头元素
        void set_header_element(unsigned long int pos, std::string name);
        //添加一整列并添加数据
        void add_column(std::string name, std::vector<std::string> data);
        //添加一列仅添加表头
        void add_column(std::string name);
        //保存路径
        const std::string& get_file_name(void) const;
        unsigned long long int get_row_idx(std::string name);
        //删除行
        bool delete_row(unsigned long long int pos);
        //删除行
        bool delete_row(std::string name)
        {
            return delete_row(get_row_idx(name));
        }
        //末尾添加新行
        void push_row(const std::vector<std::string>& r);
        template <typename... U>
        void push_row(U... V)
        {
            add_row(rowcount(), V...);
        }
        //添加行,这种方法更高效，减少递归
        bool add_row(unsigned long long int pos, const std::vector<std::string>& r);
        //添加行，使用<< 作为类型转化  利用扩展参数包，实现变长参数
        template <typename... U>
        bool add_row(unsigned long long int pos, U... V)
        {
            return add_row(pos, gen_string_vec(V...));
        }
        //写入文件
        void sync(void) const;
        // 利用递归对参数模板展开
        // 将一堆变量转化为一个string的vector
        template <typename... U>
        static std::vector<std::string> gen_string_vec(U... V)
        {
            std::vector<std::string> Vec;
            gen_string_vec(Vec, V...);
            return Vec;
        }
        template <typename T,typename... U>
        static std::vector<std::string>& gen_string_vec(std::vector<std::string>& vec,T v1, U... V)
        {
            std::ostringstream  ss;
            ss.imbue(std::locale(std::locale::classic(), "", std::locale::ctype)); //解决出现千分符逗号问题
            ss <<v1;
            vec.push_back(ss.str());
            return gen_string_vec(vec, V...);
        }
        static std::vector<std::string>& gen_string_vec(std::vector<std::string>& vec)
        {
            return vec;
        }
    protected:
        //获取表头信息
        void parseHeader(void);
        //提取内容
        void parseContent(void);

    private:
        std::string _file;
        const char _sep;
        std::vector<std::string> _originalFile; //保存原始csv文件
        std::vector<std::string> _header;       //保存header 作为row的column的实例
        std::unordered_map<std::string, unsigned long int> _header_index;
        std::unordered_map<std::string, unsigned long long int> _row_index;

        std::vector<Row*> _content;             //指向每一行
        bool _parseheader;
        bool _index;

    public:
       Row& operator[](const unsigned long long int row)
        {
            return get_row(row);
        }
        Row& operator[](const std::string row)
        {
            return get_row(row);
        }
    };

}

#endif // !CSV_FILE_H
