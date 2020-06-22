/**
 * @brief:	状态类
 *          所有状态信息以及功能处理均在这个里面
 * @Useage:
 * @author:   李昊佳
 * @date:     2020.6.4
**/

#pragma once
#ifndef _STATUS_H_
#define _STATUS_H_
#include "lib/tabulate.hpp"
#include "lib/picosha2.h"
#include "account.h"
using namespace std;
using namespace csv;
using namespace tabulate;

//状态机基类，所有子类主要对sub_process进行编写
//传入主要是上一个状态名称 返回为下一个状态名称
//这么写的主要目的学习一下继承，还有虚函数
class status
{
public:

    static void process(string start_process);

public:
    //子类需要首先调用这个，来在hash表中注册
    void set_new_status(string name, status* sub_process);
    // 利用虚函数，用来调用子类的处理
    virtual string sub_process(string last_status)
    {
        return last_status;
    }

public:
    //构建一个名称和类指针的hash表，所有的类共用一个
    static unordered_map<string, status*> status_map;

    string sub_process_name;

};
//类内静态变量要在类外初始化
unordered_map<string, status*> status::status_map;

void status::set_new_status(string name, status* sub_process)
{
    status_map.insert({ name,sub_process });
    sub_process_name = name;
}
void status::process(string start_process)
{

    std::string next_status(start_process);
    while (!next_status.empty())
    {
        next_status = status_map.at(next_status)->sub_process(next_status);
    }
}

//登陆界面
class welcome : public status
{
public:
    welcome()
    {   //子类需要首先调用这个，来在hash表中注册
        set_new_status("welcome", this);
    }

public:
    //一个状态子类的处理
    string sub_process(string last_status)
    {
        //界面绘制
        system("cls");
        Table table;
        Table table_control;
        table.format();
        table.add_row({ "\n"
                    "Welcome to NEU BANK!\n"
                    "\n"
                    "Have a nice day! ^_^\n"
                    "Program by Li Haojia\n"
                    "\n"
                    "Deposit or withdraw money by using ATM\n"
                    "Query or create ID by using superuser\n"
                    "\n"
            });
        table.format()
            .multi_byte_characters(true)
            .font_align(FontAlign::center)
            .corner("*")
            .width(50);

        table_control.format().width(20);
        table_control.add_row({ "a) ATM","b) Superuser"
            });
        table_control.format().hide_border();
        table.add_row({ table_control });
        table[1][0].format().font_background_color(Color::green);
        cout << table << endl;
        //获取选择状态
        char key;
        cin >> key;
        //状态机转跳
        switch (key)
        {
        case 'a':
        case 'A':
            return "atm_login";
            break;
        case 'b':
        case 'B':
            return "super_login";
            break;
        default:
            return "welcome";
            break;
        }
        return "welcome";
    }
};

//这是一个全局变量 用来在几个状态之间动态共享信息
account* login_account;

class atm_login : public status
{
public:
    atm_login(datamartix& dat)
        :dat(&dat)
    {   //子类需要首先调用这个，来在hash表中注册
        set_new_status("atm_login", this);
    }

public:
    //一个状态子类的处理
    string sub_process(string last_status)
    {
        showwelcome();
        
        if (login())
        {
            //登陆成功
            system("cls");
            Table table;
            table.format()
                .multi_byte_characters(true)
                .font_align(FontAlign::center)
                .corner("*")
                .font_background_color(Color::green)
                .width(50);
            table.add_row({ "\n"
                        "Welcome to NEU BANK!\n"
                        "\n"
                        "\n"
                        "Login Successfully!\n"
                        "\n"
                        "WELCOME!\n"
                        "\n"
                        "\n"
                });
            cout << table << endl;
            Sleep(1000);
            return "welcome_atm";
        }
        else
        {
            //保留1.5s提示信息
            Sleep(1500);
            return "atm_login";
        }

    }
    void showwelcome()
    {
        //界面绘制
        system("cls");
        Table table;
        table.format()
            .corner("*")
            .width(50);
        table.add_row({ "\n"
            "Welcome to NEU BANK!\n"
            "\n"
            "\n"
            "Login in the ATM\n"
            "\n"
            "Test case ID:1000, PASSWD:123456 \n"
            "All Default PASSWD:123456 \n"
            "\n"
            });
        table[0].format().multi_byte_characters(true)
            .font_align(FontAlign::center);

        cout << table << endl;
    }
    bool login()
    {
        cout << "请输入账号(ID)" << endl;
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// 略过缓存
        unsigned long long int ID;
        cin >> ID;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// 略过缓存
        }
        auto it=dat->get_account_data(ID);
        if (it.first == it.second)
        {
            cout << "账号不存在" << endl;
            return false;
        }
        else
        {
            cout << "请输入密码" << endl;
            string passw;
            cin >> passw;
            //对passw进行sha256加密后比较
            if (picosha2::hash256_hex_string(passw) == it.first->second->passwd)
            {
                login_account = it.first->second;//保存登录信息
                return true;
            }
            else
            {
                cout << "密码错误" << endl;
                return false;
            }
        }
    }
private:
    datamartix* dat;
};

class welcome_atm : public status
{
public:
    welcome_atm()
    {   //子类需要首先调用这个，来在hash表中注册
        set_new_status("welcome_atm", this);
    }

public:
    //一个状态子类的处理
    string sub_process(string last_status)
    {
        show_welcome();
        show_acc();
        //功能选择

        
        char key;
        cin >> key;
        switch (key)
        {
        case 'a':
        case 'A':
            return "atm_deposit";
            break;
        case 'b':
        case 'B':
            return "atm_withdraw";
            break;
        case 'c':
        case 'C':
            return "atm_showlog";
            break;
        case 'D':
        case 'd':
            login_account = NULL;
            return "welcome";
            break;
        default:
            return "welcome_atm";
            break;
        }
        return "welcome_atm";
    }
    void show_welcome()
    {
        //界面绘制
        system("cls");
        Table table;

        table.add_row({ "\n"
                    "Welcome to NEU BANK!\n"
                    "\n"
                    "Have a nice day! ^_^\n"
                    "\n"
                    "\n"
            });
        table[0].format().multi_byte_characters(true).font_align(FontAlign::center);
        table.format()
            .font_align(FontAlign::left)
            .corner("*")
            .width(100);
        table.add_row({ "a) 存钱" });
        table.add_row({ "b) 取钱" });
        table.add_row({ "c) 查询记录" });
        table.add_row({ "d) 退出登录" });
        table[1][0].format().font_background_color(Color::blue);
        table[2][0].format().font_background_color(Color::blue);
        table[3][0].format().font_background_color(Color::green);
        table[4][0].format().font_background_color(Color::red);
        cout << table << endl;
    }
    void show_acc()
    {
        Table table_res;
        table_res.format()
            .font_align(FontAlign::center)
            .corner("*")
            .width(25);

        table_res.add_row({ "ID","姓名","地址","总金额" });
        table_res[0].format().font_color(Color::yellow);
        const account* ml = login_account;
        table_res.add_row({ to_string(ml->ID).c_str(), ml->name.c_str(),
                ml->address.c_str(), to_string(ml->money).c_str() });
        cout << endl << table_res << endl;
    }
};

class atm_showlog : public status
{
public:
    atm_showlog(datamartix& dat)
        :dat(&dat)
    {   //子类需要首先调用这个，来在hash表中注册
        set_new_status("atm_showlog", this);
    }

public:
    //一个状态子类的处理
    string sub_process(string last_status)
    {

        showwelcome();
        show_acc();
        //查询得账号
        showlog();
        cout << "输入任意字母退出, 回车确认" << endl;
        char r;
        cin >> r;
        return "welcome_atm";
    }
    void showwelcome()
    {
        //界面绘制
        system("cls");
        Table table;
        table.format()
            .multi_byte_characters(true)
            .font_align(FontAlign::center)
            .corner("*")
            .width(100)
            .font_color(Color::yellow);
        table.add_row({ "\n"
                    "Welcome to NEU BANK!\n"
                    "\n"
                    "Query logs by ID\n"
                    "\n"
                    "\n"
                    "I use hash table to accelerate\n"
                    "\n"
                    "\n"
            });

        cout << table << endl;
    }
    void showlog()
    {
        if (login_account == NULL)return;
        unsigned long long int ID=login_account->ID;
        Table table_res;
        table_res.format()
            .font_align(FontAlign::center)
            .corner("*")
            .width(20);

        table_res.add_row({ "ID", "日期", "类型", "金额", "经办人" });
        table_res[0].format().font_color(Color::yellow);
        auto it = dat->get_log_data(ID);
        if (it.first == it.second)
        {
            cout << endl << "无记录" << endl;
        }
        for (auto i = it.first; i != it.second; i++)
        {
            money_log* ml = i->second;
            string d = (ml->type == log_type::Deposit) ? "存钱" : "取钱";
            table_res.add_row({ to_string(ml->ID).c_str(), ml->date.c_str(),
                d.c_str(), to_string(ml->money).c_str(), ml->transactor.c_str() });

        }
        cout << endl << table_res << endl;
    }
    void show_acc()
    {
        Table table_res;
        table_res.format()
            .font_align(FontAlign::center)
            .corner("*")
            .width(25);

        table_res.add_row({ "ID","姓名","地址","总金额" });
        table_res[0].format().font_color(Color::yellow);
        const account* ml = login_account;
        table_res.add_row({ to_string(ml->ID).c_str(), ml->name.c_str(),
                ml->address.c_str(), to_string(ml->money).c_str() });
        cout << endl << table_res << endl;
    }
private:
    datamartix* dat;
};

class atm_withdraw : public status
{
public:
    atm_withdraw(datamartix& dat)
        :dat(&dat)
    {   //子类需要首先调用这个，来在hash表中注册
        set_new_status("atm_withdraw", this);
    }

public:
    //一个状态子类的处理
    string sub_process(string last_status)
    {
        showwelcome();
        show_acc();
        double dmoney;
        string who;
        do
        {
            cout << "取款金额" << endl;
            cin >> dmoney;
            if (cin.fail())
            {
                cout << "取款金额为纯数字" << endl;
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// 略过缓存
            }
            else
                break;
        } while (true);  //输入检查
        cout << "经办人" << endl;
        cin >> who;
        if (dat->withdraw_money(login_account->ID, dmoney, who))
        {
            cout << "取款成功" << endl;
        }
        else 
            cout << "余额不足" << endl;
        show_acc();
        cout << "输入e退出,输入任意字母继续查询, 回车确认" << endl;
        char r;
        cin >> r;
        if (r == 'e' || r == 'E')
        {
            return "welcome_atm";
        }
        else
            return "atm_withdraw";

    }
    void showwelcome()
    {
        //界面绘制
        system("cls");
        Table table;
        table.format()
            .corner("*")
            .width(100);
        table.add_row({ "\n"
            "Welcome to NEU BANK!\n"
            "\n"
            "\n"
            "Withdraw!\n"
            "\n"
            "\n"
            "\n"
            "\n"
            });
        table[0].format().multi_byte_characters(true)
            .font_align(FontAlign::center);

        cout << table << endl;
    }
    void show_acc()
    {
        Table table_res;
        table_res.format()
            .font_align(FontAlign::center)
            .corner("*")
            .width(25);

        table_res.add_row({ "ID","姓名","地址","总金额" });
        table_res[0].format().font_color(Color::yellow);
        const account* ml = login_account;
        table_res.add_row({ to_string(ml->ID).c_str(), ml->name.c_str(),
                ml->address.c_str(), to_string(ml->money).c_str() });
        cout << endl << table_res << endl;
    }
private:
    datamartix* dat;
};

class atm_deposit : public status
{
public:
    atm_deposit(datamartix& dat)
        :dat(&dat)
    {   //子类需要首先调用这个，来在hash表中注册
        set_new_status("atm_deposit", this);
    }

public:
    //一个状态子类的处理
    string sub_process(string last_status)
    {
        showwelcome();
        show_acc();
        double dmoney;
        string who;
        do
        {
            cout << "存款金额" << endl;
            cin >> dmoney;
            if (cin.fail())
            {
                cout << "存款金额为纯数字" << endl;
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// 略过缓存
            }
            else
                break;
        } while (true);  //输入检查
        cout << "经办人" << endl;
        cin >> who;
        dat->deposit_money(login_account->ID, dmoney, who);
        show_acc();
        cout << "输入e退出,输入任意字母继续查询, 回车确认" << endl;
        char r;
        cin >> r;
        if (r == 'e' || r == 'E')
        {
            return "welcome_atm";
        }
        else
            return "atm_deposit";

    }
    void showwelcome()
    {
        //界面绘制
        system("cls");
        Table table;
        table.format()
            .corner("*")
            .width(100);
        table.add_row({ "\n"
            "Welcome to NEU BANK!\n"
            "\n"
            "\n"
            "Deposit!\n"
            "\n"
            "\n"
            "\n"
            "\n"
            });
        table[0].format().multi_byte_characters(true)
            .font_align(FontAlign::center);

        cout << table << endl;
    }
    void show_acc()
    {
        Table table_res;
        table_res.format()
            .font_align(FontAlign::center)
            .corner("*")
            .width(25);

        table_res.add_row({ "ID","姓名","地址","总金额" });
        table_res[0].format().font_color(Color::yellow);
        const account* ml = login_account;
        table_res.add_row({ to_string(ml->ID).c_str(), ml->name.c_str(),
                ml->address.c_str(), to_string(ml->money).c_str() });
        cout << endl << table_res << endl;
    }
private:
    datamartix* dat;
};

class super_login : public status
{
public:
    super_login()
    {   //子类需要首先调用这个，来在hash表中注册
        set_new_status("super_login", this);
        _ID = "admin";
        string passw = "admin";   //明文密码
        //密码通过sha256 加密存储
        _PASSWD = picosha2::hash256_hex_string(passw);
    }

public:
    //一个状态子类的处理
    string sub_process(string last_status)
    {
        showwelcome();
        cout << "请输入账号(ID)" << endl;
        //登陆账户并验证
        string ID, passwd;
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// 略过缓存
        
        cin >> ID;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// 略过缓存
        }
        if (ID == "admin")
        {
            cout << "请输入密码(PASSWD)" << endl;
            cin >> passwd;
            //与加密后的密码进行比较，避免明文存储
            string shapasswd = picosha2::hash256_hex_string(passwd);
            if (shapasswd == _PASSWD)
            {
                //登陆成功
                system("cls");
                Table table;
                table.format()
                    .multi_byte_characters(true)
                    .font_align(FontAlign::center)
                    .corner("*")
                    .font_background_color(Color::green)
                    .width(50);
                table.add_row({ "\n"
                            "Welcome to NEU BANK!\n"
                            "\n"
                            "\n"
                            "Login Successfully!\n"
                            "\n"
                            "WELCOME!\n"
                            "\n"
                            "\n"
                    });
                cout << table << endl;
                Sleep(1000);
                return "welcome_super";
            }
            else
            {
                cout << "密码错误！" << endl;
            }
        }
        else
        {
            cout << "账号错误！" << endl;
        }
        //保留1.5s提示信息
        Sleep(1500);
        return "super_login";

    }
    void showwelcome()
    {
        //界面绘制
        system("cls");
        Table table;
        table.format()
            .multi_byte_characters(true)
            .font_align(FontAlign::center)
            .corner("*")
            .width(50)
            .font_color(Color::yellow);
        table.add_row({ "\n"
            "Welcome to NEU BANK!\n"
            "\n"
            "\n"
            "Login in the Superuser\n"
            "\n"
            "Test case ID:admin, PASSWD:admin \n"
            "\n"
            "\n"
            });

        cout << table << endl;
    }
private:
    string _ID;
    string _PASSWD;//利用sha256加密存储
};

class welcome_super : public status
{
public:
    welcome_super()
    {   //子类需要首先调用这个，来在hash表中注册
        set_new_status("welcome_super", this);
    }

public:
    //一个状态子类的处理
    string sub_process(string last_status)
    {
        //界面绘制
        system("cls");
        Table table;
        
        table.add_row({ "\n"
                    "Welcome to NEU BANK!\n"
                    "\n"
                    "Have a nice day! ^_^\n"
                    "\n"
                    "You are superuser\n"
                    "\n"
                    "Query or create ID by using superuser\n"
                    "\n"
            });
        table[0].format().multi_byte_characters(true).font_align(FontAlign::center);
        table.format()
            .font_align(FontAlign::left)
            .corner("*")
            .width(50);
        table.add_row({ "a) 通过ID查询记录" });
        table.add_row({ "b) 通过姓名(name)查询记录" });
        table.add_row({ "c) 通过ID查询账户信息" });
        table.add_row({ "d) 通过姓名(name)查询账户信息" });
        table.add_row({ "e) 创建一个账户" });
        table.add_row({ "f) 退出登录" });
        table[1][0].format().font_background_color(Color::blue);
        table[2][0].format().font_background_color(Color::blue);
        table[3][0].format().font_background_color(Color::blue);
        table[4][0].format().font_background_color(Color::blue);
        table[5][0].format().font_background_color(Color::green);
        table[6][0].format().font_background_color(Color::red);
        cout << table << endl;
        //功能选择
        char key;
        cin >> key;
        switch (key)
        {
        case 'a':
        case 'A':
            return "query_id";
            break;
        case 'b':
        case 'B':
            return "query_name";
            break;
        case 'c':
        case 'C':
            return "query_acc_id";
            break;
        case 'D':
        case 'd':
            return "query_acc_name";
            break;
        case 'e':
        case 'E':
            return "create_account";
            break;
        case 'f':
        case 'F':
            return "welcome";
            break;
        default:
            return "welcome_super";
            break;
        }
        return "welcome_super";
    }
};

class query_id : public status
{
public:
    query_id(datamartix& dat)
        :dat(&dat)
    {   //子类需要首先调用这个，来在hash表中注册
        set_new_status("query_id", this);
    }

public:
    //一个状态子类的处理
    string sub_process(string last_status)
    {
        showwelcome();
        
        //查询得账号
        query();
        cout << "输入e退出,输入任意字母继续查询, 回车确认"<<endl;
        char r;
        cin >> r;
        if (r == 'e' || r == 'E')
        {
            return "welcome_super";
        }
        else
            return "query_id";

    }
    void showwelcome()
    {
        //界面绘制
        system("cls");
        Table table;
        table.format()
            .multi_byte_characters(true)
            .font_align(FontAlign::center)
            .corner("*")
            .width(100)
            .font_color(Color::yellow);
        table.add_row({ "\n"
            "Welcome to NEU BANK!\n"
            "\n"
            "Query logs by ID\n"
            "\n"
            "\n"
            "I use hash table to accelerate\n"
            "\n"
            "\n"
            });

        cout << table << endl;
    }
    void query()
    {
        cout << "查询记录信息" << endl;
        cout << "请输入需要查询的账号(ID)" << endl;
        unsigned long long int ID;
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// 略过缓存
        
        cin >> ID;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// 略过缓存
        }
        Table table_res;
        table_res.format()
            .font_align(FontAlign::center)
            .corner("*")
            .width(20);

        table_res.add_row({ "ID", "日期", "类型", "金额", "经办人" });
        table_res[0].format().font_color(Color::yellow);
        auto it = dat->get_log_data(ID);
        if (it.first == it.second)
        {
            cout << endl << "查询失败" << endl;
        }
        for (auto i = it.first; i != it.second; i++)
        {
            money_log* ml = i->second;
            string d = (ml->type == log_type::Deposit) ? "存钱" : "取钱";
            table_res.add_row({ to_string(ml->ID).c_str(), ml->date.c_str(),
                d.c_str(), to_string(ml->money).c_str(), ml->transactor.c_str() });

        }
        cout << endl << table_res << endl;
    }
private:
    datamartix* dat;
};

class query_name : public status
{
public:
    query_name(datamartix& dat)
        :dat(&dat)
    {   //子类需要首先调用这个，来在hash表中注册
        set_new_status("query_name", this);
    }

public:
    //一个状态子类的处理
    string sub_process(string last_status)
    {
        showwelcome();
        
        //查询
        query();
        cout << "输入e退出,输入任意字母继续查询, 回车确认" << endl;
        char r;

        
        cin >> r;
        if (r == 'e' || r == 'E')
        {
            return "welcome_super";
        }
        else
            return "query_name";

    }
    void showwelcome()
    {
        //界面绘制
        system("cls");
        Table table;
        table.format()
            .multi_byte_characters(true)
            .font_align(FontAlign::center)
            .corner("*")
            .width(100)
            .font_color(Color::yellow);
        table.add_row({ "\n"
            "Welcome to NEU BANK!\n"
            "\n"
            "Query logs by name\n"
            "\n"
            "\n"
            "I use hash table to accelerate\n"
            "\n"
            "\n"
            });

        cout << table << endl;
    }
    //查询方法
    void query()
    {
        cout << "查询记录信息" << endl;
        cout << "请输入需要查询姓名(name)" << endl;
        string name;
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// 略过缓存
        
        cin >> name;
        Table table_res;
        table_res.format()
            .font_align(FontAlign::center)
            .corner("*")
            .width(20);

        table_res.add_row({ "ID", "日期", "类型", "金额", "经办人" });
        table_res[0].format().font_color(Color::yellow);
        //真正的查询就这一句
        auto it = dat->get_log_data(name);
        if (it.first == it.second)
        {
            cout << endl << "查询失败" << endl;
        }
        for (auto i = it.first; i != it.second; i++)
        {
            money_log* ml = i->second;
            string d = (ml->type == log_type::Deposit) ? "存钱" : "取钱";
            table_res.add_row({ to_string(ml->ID).c_str(), ml->date.c_str(),
                d.c_str(), to_string(ml->money).c_str(), ml->transactor.c_str() });

        }
        cout << endl << table_res << endl;
    }
private:
    datamartix* dat;
};

class query_acc_id : public status
{
public:
    query_acc_id(datamartix& dat)
        :dat(&dat)
    {   //子类需要首先调用这个，来在hash表中注册
        set_new_status("query_acc_id", this);
    }

public:
    //一个状态子类的处理
    string sub_process(string last_status)
    {
        showwelcome();

        //查询得账号
        query();
        cout << "输入e退出,输入任意字母继续查询, 回车确认" << endl;
        char r;
        
        cin >> r;
        if (r == 'e' || r == 'E')
        {
            return "welcome_super";
        }
        else
            return "query_acc_id";

    }
    void showwelcome()
    {
        //界面绘制
        system("cls");
        Table table;
        table.format()
            .multi_byte_characters(true)
            .font_align(FontAlign::center)
            .corner("*")
            .width(100)
            .font_color(Color::yellow);
        table.add_row({ "\n"
            "Welcome to NEU BANK!\n"
            "\n"
            "Query account by ID\n"
            "\n"
            "\n"
            "I use hash table to accelerate\n"
            "\n"
            "\n"
            });

        cout << table << endl;
    }
    void query()
    {
        cout << "查询账户信息" << endl;
        cout << "请输入需要查询的账号(ID)" << endl;
        unsigned long long int ID;
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// 略过缓存
        
        cin >> ID;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// 略过缓存
        }
        Table table_res;
        table_res.format()
            .font_align(FontAlign::center)
            .corner("*")
            .width(25);

        table_res.add_row({ "ID", "姓名", "地址", "总金额" });
        table_res[0].format().font_color(Color::yellow);
        auto it = dat->get_account_data(ID);
        if (it.first == it.second)
        {
            cout << endl << "查询失败" << endl;
        }
        for (auto i = it.first; i != it.second; i++)
        {
            account* ml = i->second;
            table_res.add_row({ to_string(ml->ID).c_str(), ml->name.c_str(),
                ml->address.c_str(), to_string(ml->money).c_str() });

        }
        cout << endl << table_res << endl;
    }
private:
    datamartix* dat;
};

class query_acc_name : public status
{
public:
    query_acc_name(datamartix& dat)
        :dat(&dat)
    {   //子类需要首先调用这个，来在hash表中注册
        set_new_status("query_acc_name", this);
    }

public:
    //一个状态子类的处理
    string sub_process(string last_status)
    {
        showwelcome();
        //查询
        query();
        cout << "输入e退出,输入任意字母继续查询, 回车确认" << endl;
        char r;
        cin >> r;
        if (r == 'e' || r == 'E')
        {
            return "welcome_super";
        }
        else
            return "query_acc_name";

    }
    void showwelcome()
    {
        //界面绘制
        system("cls");
        Table table;
        table.format()
            .multi_byte_characters(true)
            .font_align(FontAlign::center)
            .corner("*")
            .width(100)
            .font_color(Color::yellow);
        table.add_row({ "\n"
            "Welcome to NEU BANK!\n"
            "\n"
            "Query accounts by name\n"
            "\n"
            "\n"
            "I use hash table to accelerate\n"
            "\n"
            "\n"
            });

        cout << table << endl;
    }
    //查询方法
    void query()
    {
        cout << "查询账户信息" << endl;
        cout << "请输入需要查询姓名(name)" << endl;
        string name;
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// 略过缓存
        
        cin >> name;
        Table table_res;
        table_res.format()
            .font_align(FontAlign::center)
            .corner("*")
            .width(25);

        table_res.add_row({ "ID", "姓名", "地址", "总金额" });
        table_res[0].format().font_color(Color::yellow);
        //真正的查询就这一句
        auto it = dat->get_account_data(name);
        if (it.first == it.second)
        {
            cout << endl << "查询失败" << endl;
        }
        for (auto i = it.first; i != it.second; i++)
        {
            account* ml = i->second;
            table_res.add_row({ to_string(ml->ID).c_str(), ml->name.c_str(),
                ml->address.c_str(), to_string(ml->money).c_str() });

        }
        cout << endl << table_res << endl;
    }
private:
    datamartix* dat;
};

class create_account : public status
{
public:
    create_account(datamartix& dat)
        :dat(&dat)
    {   //子类需要首先调用这个，来在hash表中注册
        set_new_status("create_account", this);
    }

public:
    //一个状态子类的处理
    string sub_process(string last_status)
    {
        showwelcome();
        account a;
        do 
        {
            cout << "请输入账号(ID) 纯数字" << endl;
            cin >> a.ID; 
            if (cin.fail())
            {
                cout << "账号为纯数字" << endl;
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// 略过缓存
            }
            else
            {
                auto it = dat->get_account_data(a.ID);
                if (it.first == it.second)
                {
                    break;
                }
                else
                    cout << "账号已存在" << endl;
            }
        } while (true);  //输入检查
        cout << "请输入密码" << endl;
        cin >> a.passwd;
        //对passw进行sha256加密储存
        a.passwd = picosha2::hash256_hex_string(a.passwd);
        
        cout << "请输入姓名" << endl;
        cin >> a.name;
        cout << "请输入地址" << endl;
        cin >> a.address;
        do
        {
            cout << "请输入预存款" << endl;
            cin >> a.money;
            if (cin.fail())
            {
                cout << "预存款为纯数字" << endl;
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// 略过缓存
            }
            else
                break;
        } while (true);  //输入检查

        dat->add_account(a);
        cout << "创建成功!" << endl;
        cout << "输入e退出,输入任意字母继续创建, 回车确认" << endl;
        char r;
        cin >> r;
        if (r == 'e' || r == 'E')
        {
            return "welcome_super";
        }
        else
            return "create_account";
    }
    void showwelcome()
    {
        //界面绘制
        system("cls");
        Table table;
        table.format()
            .corner("*")
            .width(50);
        table.add_row({ "\n"
            "Welcome to NEU BANK!\n"
            "\n"
            "\n"
            "create new ATM account\n"
            "\n"
            "\n"
            "\n"
            "\n"
            });
        table[0].format().multi_byte_characters(true)
            .font_align(FontAlign::center);

        cout << table << endl;
    }
private:
    datamartix* dat;
};
#endif // !_STATUS_H_
