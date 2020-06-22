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
#include <fstream>
#include <sstream>
#include <iomanip>
#include "csvfile.h"

namespace csv 
{

    //-------csv装载------    
    Parser::Parser(const std::string& path, char sep,bool parseheader,bool index)
        : _sep(sep),_parseheader(parseheader), _index(index)
    {
       
        if (!load_file(path))
        {
            throw Error("We can't load file or file is empty");
        }
    }
    Parser::Parser(const std::string& path, std::vector<std::string> headers, char sep, bool parseheader, bool index)
        : _sep(sep), _parseheader(parseheader), _index(index)
    {
      
        if (!load_file(path))
        {
            create_file(path, headers);
            sync();//写入文件
        }
    }

    Parser::~Parser(void)
    {
        for (auto it = _content.begin(); it != _content.end(); it++)
            delete* it;
    }
    //创建一个新的文件  注意这个操作只是在内存中保存，需要调用sync()来真正保存到硬盘上
    void Parser::create_file(const std::string& path,std::vector<std::string>& headers)
    {
        _file = path;
        _header.assign(headers.begin(), headers.end());
        for (unsigned long int i = 0; i < _header.size(); i++)
        {
            _header_index.insert({ _header[i],i });
        }
    }
    //按照路径加载并分析文件
    bool Parser::load_file(const std::string& path)
    {
        std::string line;
        _file = path;
        std::ifstream ifile(_file.c_str());
        if (ifile.is_open())    //打开文件
        {
            while (ifile.good())    //还未读取完
            {
                getline(ifile, line);   //获取一行
                if (line != "")
                    _originalFile.push_back(line);
            }
            ifile.close();

            if (_originalFile.size() == 0)
                return false;
            if (_parseheader)
            {
                parseHeader();
            }
            parseContent();
            return true;
        }
        else
            return false;
    }
    //获取表头信息
    void Parser::parseHeader(void)
    {   
        std::stringstream ss(_originalFile[0]); //第一行是表头分析
        std::string item;
        unsigned long int i = 0;
        while (std::getline(ss, item, _sep))
        {
            _header.emplace_back(item);
            //构建hash表，加快字符串查找速度
            _header_index.insert({ item,i });
            i++;
        }
        
    }
    //提取内容
    void Parser::parseContent(void)//
    {
        std::vector<std::string>::iterator it;

        it = _originalFile.begin();
        if(_parseheader)
            it++; // 跳过 header

        for (; it != _originalFile.end(); it++)
        {
            bool quoted = false;    //引号内逗号保留
            int tokenStart = 0;     //一个但云哥的开始位置
            unsigned int i = 0;
            Row* row;               //保存行指针
            if (_header.size()>0)
                row = new Row(&_header,&_header_index); //加入column信息
            else 
                row = new Row();
            for (; i != it->length(); i++)
            {
                if (it->at(i) == '"')
                    quoted = ((quoted) ? (false) : (true)); //第一次找到引号为true 第二次为false
                else if (it->at(i) == _sep && !quoted)       //找到分隔符，并且不在引号里面
                {
                    row->push(it->substr(tokenStart, i - tokenStart));
                    tokenStart = i + 1;
                }
            }
            //end把最后一个放进去
            row->push(it->substr(tokenStart, it->length() - tokenStart));

            // if value(s) missing
            if(_header.size() > 0)
                if (row->size() != _header.size())
                    throw Error("corrupted data !");
            _content.push_back(row);    //添加进去
            if (_index)
            {
                _row_index.insert({ row->get_value<std::string>(0), (_content.size() - 1) });
            }
        }
    }
    //获取一行数据
    Row& Parser::get_row(unsigned long long int rowPosition)
    {
        if (rowPosition < _content.size())
            return *(_content[rowPosition]);
        throw Error("can't return this row (doesn't exist)");
    }

    //获取一行数据
    Row& Parser::get_row(std::string name)
    {
        return get_row(get_row_idx(name));
    }
    //获取总行数
    unsigned long long int Parser::rowcount(void) const
    {
        return _content.size();
    }
    //获取总列数
    unsigned long int Parser::columncount(void) const
    {
        return _header.size();
    }
    //获取表头
    std::vector<std::string> Parser::get_header(void) const
    {
        return _header;
    }
    //根据表头名获取表头序号
    unsigned long int Parser::get_header_idx(std::string name)
    {
        if (_index)
        {
            //利用hash表快速查找
            std::unordered_map<std::string, unsigned long int>::const_iterator got = _header_index.find(name);
            if (got == _header_index.end())
            {
                //没有找到
                throw Error("can't find the column");
            }

            return got->second;
        }
        else
        {
            //没有找到
            throw Error("No row index");
            return 0;//错误
        }
    }
    //获取表头元素
    const std::string Parser::get_header_element(unsigned long int pos) const
    {
        if (pos >= _header.size())
            throw Error("can't return this header (doesn't exist)");
        return _header[pos];
    }
    //更新表头元素
    void Parser::set_header_element(unsigned long int pos, std::string name)
    {
        _header[pos] = name;
    }
    //根据表头名获取表头序号
    unsigned long long int Parser::get_row_idx(std::string name)
    {
        //利用hash表快速查找
        auto got = _row_index.find(name);
        if (got == _row_index.end())
        {
            //没有找到
            throw Error("can't find the row");
        }
        return got->second;
    }
    //删除行
    bool Parser::delete_row(unsigned long long int pos)
    {
        if (pos < _content.size())
        {
           
            if (_index)
            {
                _row_index.erase(_content[pos]->get_value<std::string>(0));
            }
            delete* (_content.begin() + pos);       //删除指针指向的内容
            _content.erase(_content.begin() + pos); //删除指针
            return true;
        }
        return false;
    }
    void Parser::push_row(const std::vector<std::string>& r)
    {
        add_row(rowcount(), r);
    }
    //添加行
    bool Parser::add_row(unsigned long long int pos, const std::vector<std::string>& r)
    {
        Row* row;
        if (columncount() > 0)
        {
            row = new Row(&_header, &_header_index);
        }
        else row = new Row();   //首先实例化
        for (auto it = r.begin(); it != r.end(); it++)
            row->push(*it); //添加数据内容

        if (pos <= _content.size()) //位置合法
        {
            _content.insert(_content.begin() + pos, row);
            if (_index)
            {
                _row_index.insert({ row->get_value<std::string>(0), pos });
            }
            return true;
        }
        return false;
    }
    //添加列
    void Parser::add_column(std::string name)
    {
        _header.emplace_back(name);
        _header_index.insert({ name,_header.size() - 1 });
    }
    //添加列并添加数据
    void Parser::add_column(std::string name, std::vector<std::string> data)
    {
        add_column(name);
        unsigned long long int row_c = data.size();
        for (unsigned long long int i=0;i < row_c;i++)
        {
            _content[i]->push(data[i]);
        }
    }
    //根据当前表保存数据
    void Parser::sync(void) const
    {
        std::ofstream f;
        f.open(_file, std::ios::out | std::ios::trunc);

        // 写入表头
        if (columncount() > 0)
        {
            unsigned int i = 0;
            for (auto it = _header.begin(); it != _header.end(); it++)
            {
                f << *it;
                if (i < _header.size() - 1)
                    f << _sep;
                else
                    f << std::endl;
                i++;
            }
        }
        // 写入每一行内容
        for (auto it = _content.begin(); it != _content.end(); it++)
        {
            (*it)->_sep = _sep;
            f << **it << std::endl;
        }
        f.close();
    }
    //获取文件名
    const std::string& Parser::get_file_name(void) const
    {
        return _file;
    }

    
    //---------ROW---------
    
    const unsigned long int Row::get_column_idx(const std::string& valueName)
    {
        if (_column_index==NULL)
        {
            //表头为空
            throw Error("Column is empty");
        }
        //利用hash表快速查找
        std::unordered_map<std::string, unsigned long int>::const_iterator got = _column_index->find(valueName);
        if (got == _column_index->end())
        {
            //没有找到
            throw Error("can't find the column");
        }
        return got->second;
    }

    //删除一个列
    void Row::delete_value(const unsigned long int idx)
    {
        if(idx >= _values.size()) //没有找到
            throw Error("out of the column range");
        _values.erase(_values.begin() + idx);
    }

    //屏幕输出重载
    std::ostream& operator<<(std::ostream& os, const Row& row)
    {
        for (unsigned int i = 0; i != row._values.size(); i++)
            os << row._values[i] << " | ";

        return os;
    }

    //文件输出重载，用以保存文件
    std::ofstream& operator<<(std::ofstream& os, const Row& row)
    {
        os.imbue(std::locale(""));
        for (unsigned int i = 0; i != row._values.size(); i++)
        {
            os << row._values[i];
            if (i < row._values.size() - 1)
                os << row._sep;
        }
        return os;
    }
}
