/**
 * @brief:
 *          ��ȡcsv�ļ�
 *          д��csv�ļ�
 *          ����ģ���������ת��
 *          ����hash���в��ұ�ͷ
 * @Useage: Parser d(filepath);
 *          cout<<d[1]["column1"]<<d[1].get_value<double>("column1");
 *          cout<<d[1][0]<<d[1].get_value<double>(0);
 *          d[1].set_value<>(1)
 * @author:   ��껼�
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
    ////------������-------
    class Error : public std::runtime_error
    {
    public:
        Error(const std::string& msg) :
            std::runtime_error(std::string("CSVfile : ").append(msg)) {}
    };

    //------row ����--------
   // ԭ��1.������������� 2.���ڲ��� 3.���Ե���һ�����ݳ�Ϊ�µ�vector��ͬʱʵ�ֶ�̬���� 
   //
    class Row
    {
    public:
        char _sep = ',';
        //��ʼ�� 
        Row(const std::vector<std::string>& values) : _values(values) { _column_index = NULL; _column = NULL; };  //ֻ��ʼ������
        Row(const std::vector<std::string>* col, const std::unordered_map<std::string, unsigned long int>* col_idx) //��ʼ����ͷ
            :_column(col), _column_index(col_idx) {};
        Row(const std::vector<std::string>* col, const std::unordered_map<std::string, unsigned long int>* col_idx, const std::vector<std::string>& values)
            :_column(col), _column_index(col_idx), _values(values) {}; //��ʼ����ͷ������
        Row() { _column_index = NULL; _column = NULL; };    //ʹ��ϵͳĬ��
        ~Row(void) = default;
        //���ڶ��� Ĭ����������������Ч��

       //���غ��еı�������
        unsigned int size(void)
        {
            return _values.size();
        }
        //���������������е����
        const unsigned long int get_column_idx(const std::string& valueName);
        //���һ��
        void push(const std::string& value)
        {
            _values.emplace_back(value);
        }
        //ɾ��һ������
        //�������������Ҫ��֤���ͷ��_values�ϸ��Ӧ
        void delete_value(const std::string& valueName)
        {
            delete_value(get_column_idx(valueName));
        }
        void delete_value(const unsigned long int idx);


        //������Ҫ��������ת���������ڲ��Զ�����  ��Ҫ����֧��<< >>�����
        //ʹ��ģ�壬���ֶ�̬��,C++��֧��ģ�������룬�����Ӳ�����error LNK2019���޷��������ⲿ���š�

        //������ŷ��ʲ���������ת��
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
        //�����������ʲ���������ת��
        template<typename T>
        T get_value(const std::string& valueName)
        {
            return get_value<T>(get_column_idx(valueName));
        }

        //��������趨���� ��Ҫָ��������������
        template<typename T>
        bool set(unsigned long int idx, const T& value)
        {
            //_column ��һ������
            /*if (idx > _column.size())
            {
                //����column��Χ
                throw Error("Set values out of column");
                return false;
            }*/
            //��������ת��
            std::ostringstream  ss;
            ss.imbue(std::locale(std::locale::classic(), "", std::locale::ctype));
            ss << value;
            //if (idx < _column.size())
                _values[idx] = ss.str();
           // else
              //  _values.emplace_back(ss.str());
            return true;
        }

        //���������趨���� ��Ҫָ��������������
        template<typename T>
        bool set(const std::string& valueName, const T& value)
        {
            return set(get_column_idx(valueName), value);
        }

    public: //���������

        //������ŷ���(string ����ֱ���޸�ֵ)
        std::string& operator[](unsigned long int valuePosition)
        {
            if (valuePosition < _values.size())
                return _values[valuePosition];
            throw Error("can't return this value (doesn't exist)");
        }
        //������������(string ����ֱ���޸�ֵ)
        std::string& operator[](const std::string& valueName)
        {
            return _values[get_column_idx(valueName)];
        }
        //������ŷ��ʲ���������ת��
        /*template<typename T>
        const T operator[](unsigned long int valuePosition) const
        {
            return get_value<T>(valuePosition);
        }
        //������������
        template<typename T>
        const T operator[](const std::string& valueName) const
        {
            return get_value<T>(valueName);
        }*/
        //��Ļ�������
        friend std::ostream& operator<<(std::ostream& os, const Row& row);
        //�ļ�������أ����Ա����ļ�
        friend std::ofstream& operator<<(std::ofstream& os, const Row& row);

    private:
        std::vector<std::string> _values;       //����
        const std::vector<std::string>* _column;      //��ͷ�ַ�����Ϣ  ��ָ�뷽ʽʵ�ֹ�����ͷ
        const std::unordered_map<std::string, unsigned long int>* _column_index;  //����hash��ӿ��ͷ�����ٶ�  ��ָ�뷽ʽʵ�ֹ�����ͷ
    };


    ////------csv ��ȡת��-------- 
    class Parser
    {

    public:
        //�򿪷���һ��csv�ļ���path����·�� sep����ָ���  parseheader��ʾ�Ƿ������ͷ
        Parser(char sep = ',', bool parseheader = true, bool index=true) :_sep(sep), _parseheader(parseheader),_index(index) 
        {
        };
        Parser(const std::string& path, char sep = ',',bool parseheader=true, bool index = true);
        //�������ͷ�Ļ����������ļ������ֱ���Զ������ļ������ڵĻ����Դ���ı�ͷ�����ļ�Ϊ׼
        Parser(const std::string& path, std::vector<std::string> headers, char sep=',', bool parseheader=true, bool index = true);
        ~Parser(void);

    public:
        //����һ���µ��ļ�  ע���������ֻ�����ڴ��б��棬��Ҫ����sync()���������浽Ӳ����
        void create_file(const std::string& path,std::vector<std::string>& headers);
        //����·�����ز������ļ�
        bool load_file(const std::string& path);
        //��ȡһ������
        Row& get_row(unsigned long long int row);
        //��ȡһ������
        Row& get_row(std::string name);
        //��ȡ������
        unsigned long long int rowcount(void) const;
        //��ȡ������
        unsigned long int columncount(void) const;
        //��ȡ��ͷ
        std::vector<std::string> get_header(void) const;
        //���ݱ�ͷ����ȡ��ͷ���
        unsigned long int get_header_idx(std::string name);
        //��ȡ��ͷԪ��
        const std::string get_header_element(unsigned long int pos) const;
        //���±�ͷԪ��
        void set_header_element(unsigned long int pos, std::string name);
        //���һ���в��������
        void add_column(std::string name, std::vector<std::string> data);
        //���һ�н���ӱ�ͷ
        void add_column(std::string name);
        //����·��
        const std::string& get_file_name(void) const;
        unsigned long long int get_row_idx(std::string name);
        //ɾ����
        bool delete_row(unsigned long long int pos);
        //ɾ����
        bool delete_row(std::string name)
        {
            return delete_row(get_row_idx(name));
        }
        //ĩβ�������
        void push_row(const std::vector<std::string>& r);
        template <typename... U>
        void push_row(U... V)
        {
            add_row(rowcount(), V...);
        }
        //�����,���ַ�������Ч�����ٵݹ�
        bool add_row(unsigned long long int pos, const std::vector<std::string>& r);
        //����У�ʹ��<< ��Ϊ����ת��  ������չ��������ʵ�ֱ䳤����
        template <typename... U>
        bool add_row(unsigned long long int pos, U... V)
        {
            return add_row(pos, gen_string_vec(V...));
        }
        //д���ļ�
        void sync(void) const;
        // ���õݹ�Բ���ģ��չ��
        // ��һ�ѱ���ת��Ϊһ��string��vector
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
            ss.imbue(std::locale(std::locale::classic(), "", std::locale::ctype)); //�������ǧ�ַ���������
            ss <<v1;
            vec.push_back(ss.str());
            return gen_string_vec(vec, V...);
        }
        static std::vector<std::string>& gen_string_vec(std::vector<std::string>& vec)
        {
            return vec;
        }
    protected:
        //��ȡ��ͷ��Ϣ
        void parseHeader(void);
        //��ȡ����
        void parseContent(void);

    private:
        std::string _file;
        const char _sep;
        std::vector<std::string> _originalFile; //����ԭʼcsv�ļ�
        std::vector<std::string> _header;       //����header ��Ϊrow��column��ʵ��
        std::unordered_map<std::string, unsigned long int> _header_index;
        std::unordered_map<std::string, unsigned long long int> _row_index;

        std::vector<Row*> _content;             //ָ��ÿһ��
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
