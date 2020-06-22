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
#include <fstream>
#include <sstream>
#include <iomanip>
#include "csvfile.h"

namespace csv 
{

    //-------csvװ��------    
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
            sync();//д���ļ�
        }
    }

    Parser::~Parser(void)
    {
        for (auto it = _content.begin(); it != _content.end(); it++)
            delete* it;
    }
    //����һ���µ��ļ�  ע���������ֻ�����ڴ��б��棬��Ҫ����sync()���������浽Ӳ����
    void Parser::create_file(const std::string& path,std::vector<std::string>& headers)
    {
        _file = path;
        _header.assign(headers.begin(), headers.end());
        for (unsigned long int i = 0; i < _header.size(); i++)
        {
            _header_index.insert({ _header[i],i });
        }
    }
    //����·�����ز������ļ�
    bool Parser::load_file(const std::string& path)
    {
        std::string line;
        _file = path;
        std::ifstream ifile(_file.c_str());
        if (ifile.is_open())    //���ļ�
        {
            while (ifile.good())    //��δ��ȡ��
            {
                getline(ifile, line);   //��ȡһ��
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
    //��ȡ��ͷ��Ϣ
    void Parser::parseHeader(void)
    {   
        std::stringstream ss(_originalFile[0]); //��һ���Ǳ�ͷ����
        std::string item;
        unsigned long int i = 0;
        while (std::getline(ss, item, _sep))
        {
            _header.emplace_back(item);
            //����hash���ӿ��ַ��������ٶ�
            _header_index.insert({ item,i });
            i++;
        }
        
    }
    //��ȡ����
    void Parser::parseContent(void)//
    {
        std::vector<std::string>::iterator it;

        it = _originalFile.begin();
        if(_parseheader)
            it++; // ���� header

        for (; it != _originalFile.end(); it++)
        {
            bool quoted = false;    //�����ڶ��ű���
            int tokenStart = 0;     //һ�����Ƹ�Ŀ�ʼλ��
            unsigned int i = 0;
            Row* row;               //������ָ��
            if (_header.size()>0)
                row = new Row(&_header,&_header_index); //����column��Ϣ
            else 
                row = new Row();
            for (; i != it->length(); i++)
            {
                if (it->at(i) == '"')
                    quoted = ((quoted) ? (false) : (true)); //��һ���ҵ�����Ϊtrue �ڶ���Ϊfalse
                else if (it->at(i) == _sep && !quoted)       //�ҵ��ָ��������Ҳ�����������
                {
                    row->push(it->substr(tokenStart, i - tokenStart));
                    tokenStart = i + 1;
                }
            }
            //end�����һ���Ž�ȥ
            row->push(it->substr(tokenStart, it->length() - tokenStart));

            // if value(s) missing
            if(_header.size() > 0)
                if (row->size() != _header.size())
                    throw Error("corrupted data !");
            _content.push_back(row);    //��ӽ�ȥ
            if (_index)
            {
                _row_index.insert({ row->get_value<std::string>(0), (_content.size() - 1) });
            }
        }
    }
    //��ȡһ������
    Row& Parser::get_row(unsigned long long int rowPosition)
    {
        if (rowPosition < _content.size())
            return *(_content[rowPosition]);
        throw Error("can't return this row (doesn't exist)");
    }

    //��ȡһ������
    Row& Parser::get_row(std::string name)
    {
        return get_row(get_row_idx(name));
    }
    //��ȡ������
    unsigned long long int Parser::rowcount(void) const
    {
        return _content.size();
    }
    //��ȡ������
    unsigned long int Parser::columncount(void) const
    {
        return _header.size();
    }
    //��ȡ��ͷ
    std::vector<std::string> Parser::get_header(void) const
    {
        return _header;
    }
    //���ݱ�ͷ����ȡ��ͷ���
    unsigned long int Parser::get_header_idx(std::string name)
    {
        if (_index)
        {
            //����hash����ٲ���
            std::unordered_map<std::string, unsigned long int>::const_iterator got = _header_index.find(name);
            if (got == _header_index.end())
            {
                //û���ҵ�
                throw Error("can't find the column");
            }

            return got->second;
        }
        else
        {
            //û���ҵ�
            throw Error("No row index");
            return 0;//����
        }
    }
    //��ȡ��ͷԪ��
    const std::string Parser::get_header_element(unsigned long int pos) const
    {
        if (pos >= _header.size())
            throw Error("can't return this header (doesn't exist)");
        return _header[pos];
    }
    //���±�ͷԪ��
    void Parser::set_header_element(unsigned long int pos, std::string name)
    {
        _header[pos] = name;
    }
    //���ݱ�ͷ����ȡ��ͷ���
    unsigned long long int Parser::get_row_idx(std::string name)
    {
        //����hash����ٲ���
        auto got = _row_index.find(name);
        if (got == _row_index.end())
        {
            //û���ҵ�
            throw Error("can't find the row");
        }
        return got->second;
    }
    //ɾ����
    bool Parser::delete_row(unsigned long long int pos)
    {
        if (pos < _content.size())
        {
           
            if (_index)
            {
                _row_index.erase(_content[pos]->get_value<std::string>(0));
            }
            delete* (_content.begin() + pos);       //ɾ��ָ��ָ�������
            _content.erase(_content.begin() + pos); //ɾ��ָ��
            return true;
        }
        return false;
    }
    void Parser::push_row(const std::vector<std::string>& r)
    {
        add_row(rowcount(), r);
    }
    //�����
    bool Parser::add_row(unsigned long long int pos, const std::vector<std::string>& r)
    {
        Row* row;
        if (columncount() > 0)
        {
            row = new Row(&_header, &_header_index);
        }
        else row = new Row();   //����ʵ����
        for (auto it = r.begin(); it != r.end(); it++)
            row->push(*it); //�����������

        if (pos <= _content.size()) //λ�úϷ�
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
    //�����
    void Parser::add_column(std::string name)
    {
        _header.emplace_back(name);
        _header_index.insert({ name,_header.size() - 1 });
    }
    //����в��������
    void Parser::add_column(std::string name, std::vector<std::string> data)
    {
        add_column(name);
        unsigned long long int row_c = data.size();
        for (unsigned long long int i=0;i < row_c;i++)
        {
            _content[i]->push(data[i]);
        }
    }
    //���ݵ�ǰ��������
    void Parser::sync(void) const
    {
        std::ofstream f;
        f.open(_file, std::ios::out | std::ios::trunc);

        // д���ͷ
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
        // д��ÿһ������
        for (auto it = _content.begin(); it != _content.end(); it++)
        {
            (*it)->_sep = _sep;
            f << **it << std::endl;
        }
        f.close();
    }
    //��ȡ�ļ���
    const std::string& Parser::get_file_name(void) const
    {
        return _file;
    }

    
    //---------ROW---------
    
    const unsigned long int Row::get_column_idx(const std::string& valueName)
    {
        if (_column_index==NULL)
        {
            //��ͷΪ��
            throw Error("Column is empty");
        }
        //����hash����ٲ���
        std::unordered_map<std::string, unsigned long int>::const_iterator got = _column_index->find(valueName);
        if (got == _column_index->end())
        {
            //û���ҵ�
            throw Error("can't find the column");
        }
        return got->second;
    }

    //ɾ��һ����
    void Row::delete_value(const unsigned long int idx)
    {
        if(idx >= _values.size()) //û���ҵ�
            throw Error("out of the column range");
        _values.erase(_values.begin() + idx);
    }

    //��Ļ�������
    std::ostream& operator<<(std::ostream& os, const Row& row)
    {
        for (unsigned int i = 0; i != row._values.size(); i++)
            os << row._values[i] << " | ";

        return os;
    }

    //�ļ�������أ����Ա����ļ�
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
