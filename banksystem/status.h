/**
 * @brief:	״̬��
 *          ����״̬��Ϣ�Լ����ܴ�������������
 * @Useage:
 * @author:   ��껼�
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

//״̬�����࣬����������Ҫ��sub_process���б�д
//������Ҫ����һ��״̬���� ����Ϊ��һ��״̬����
//��ôд����ҪĿ��ѧϰһ�¼̳У������麯��
class status
{
public:

    static void process(string start_process);

public:
    //������Ҫ���ȵ������������hash����ע��
    void set_new_status(string name, status* sub_process);
    // �����麯����������������Ĵ���
    virtual string sub_process(string last_status)
    {
        return last_status;
    }

public:
    //����һ�����ƺ���ָ���hash�����е��๲��һ��
    static unordered_map<string, status*> status_map;

    string sub_process_name;

};
//���ھ�̬����Ҫ�������ʼ��
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

//��½����
class welcome : public status
{
public:
    welcome()
    {   //������Ҫ���ȵ������������hash����ע��
        set_new_status("welcome", this);
    }

public:
    //һ��״̬����Ĵ���
    string sub_process(string last_status)
    {
        //�������
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
        //��ȡѡ��״̬
        char key;
        cin >> key;
        //״̬��ת��
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

//����һ��ȫ�ֱ��� �����ڼ���״̬֮�䶯̬������Ϣ
account* login_account;

class atm_login : public status
{
public:
    atm_login(datamartix& dat)
        :dat(&dat)
    {   //������Ҫ���ȵ������������hash����ע��
        set_new_status("atm_login", this);
    }

public:
    //һ��״̬����Ĵ���
    string sub_process(string last_status)
    {
        showwelcome();
        
        if (login())
        {
            //��½�ɹ�
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
            //����1.5s��ʾ��Ϣ
            Sleep(1500);
            return "atm_login";
        }

    }
    void showwelcome()
    {
        //�������
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
        cout << "�������˺�(ID)" << endl;
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// �Թ�����
        unsigned long long int ID;
        cin >> ID;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// �Թ�����
        }
        auto it=dat->get_account_data(ID);
        if (it.first == it.second)
        {
            cout << "�˺Ų�����" << endl;
            return false;
        }
        else
        {
            cout << "����������" << endl;
            string passw;
            cin >> passw;
            //��passw����sha256���ܺ�Ƚ�
            if (picosha2::hash256_hex_string(passw) == it.first->second->passwd)
            {
                login_account = it.first->second;//�����¼��Ϣ
                return true;
            }
            else
            {
                cout << "�������" << endl;
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
    {   //������Ҫ���ȵ������������hash����ע��
        set_new_status("welcome_atm", this);
    }

public:
    //һ��״̬����Ĵ���
    string sub_process(string last_status)
    {
        show_welcome();
        show_acc();
        //����ѡ��

        
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
        //�������
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
        table.add_row({ "a) ��Ǯ" });
        table.add_row({ "b) ȡǮ" });
        table.add_row({ "c) ��ѯ��¼" });
        table.add_row({ "d) �˳���¼" });
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

        table_res.add_row({ "ID","����","��ַ","�ܽ��" });
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
    {   //������Ҫ���ȵ������������hash����ע��
        set_new_status("atm_showlog", this);
    }

public:
    //һ��״̬����Ĵ���
    string sub_process(string last_status)
    {

        showwelcome();
        show_acc();
        //��ѯ���˺�
        showlog();
        cout << "����������ĸ�˳�, �س�ȷ��" << endl;
        char r;
        cin >> r;
        return "welcome_atm";
    }
    void showwelcome()
    {
        //�������
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

        table_res.add_row({ "ID", "����", "����", "���", "������" });
        table_res[0].format().font_color(Color::yellow);
        auto it = dat->get_log_data(ID);
        if (it.first == it.second)
        {
            cout << endl << "�޼�¼" << endl;
        }
        for (auto i = it.first; i != it.second; i++)
        {
            money_log* ml = i->second;
            string d = (ml->type == log_type::Deposit) ? "��Ǯ" : "ȡǮ";
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

        table_res.add_row({ "ID","����","��ַ","�ܽ��" });
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
    {   //������Ҫ���ȵ������������hash����ע��
        set_new_status("atm_withdraw", this);
    }

public:
    //һ��״̬����Ĵ���
    string sub_process(string last_status)
    {
        showwelcome();
        show_acc();
        double dmoney;
        string who;
        do
        {
            cout << "ȡ����" << endl;
            cin >> dmoney;
            if (cin.fail())
            {
                cout << "ȡ����Ϊ������" << endl;
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// �Թ�����
            }
            else
                break;
        } while (true);  //������
        cout << "������" << endl;
        cin >> who;
        if (dat->withdraw_money(login_account->ID, dmoney, who))
        {
            cout << "ȡ��ɹ�" << endl;
        }
        else 
            cout << "����" << endl;
        show_acc();
        cout << "����e�˳�,����������ĸ������ѯ, �س�ȷ��" << endl;
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
        //�������
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

        table_res.add_row({ "ID","����","��ַ","�ܽ��" });
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
    {   //������Ҫ���ȵ������������hash����ע��
        set_new_status("atm_deposit", this);
    }

public:
    //һ��״̬����Ĵ���
    string sub_process(string last_status)
    {
        showwelcome();
        show_acc();
        double dmoney;
        string who;
        do
        {
            cout << "�����" << endl;
            cin >> dmoney;
            if (cin.fail())
            {
                cout << "�����Ϊ������" << endl;
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// �Թ�����
            }
            else
                break;
        } while (true);  //������
        cout << "������" << endl;
        cin >> who;
        dat->deposit_money(login_account->ID, dmoney, who);
        show_acc();
        cout << "����e�˳�,����������ĸ������ѯ, �س�ȷ��" << endl;
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
        //�������
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

        table_res.add_row({ "ID","����","��ַ","�ܽ��" });
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
    {   //������Ҫ���ȵ������������hash����ע��
        set_new_status("super_login", this);
        _ID = "admin";
        string passw = "admin";   //��������
        //����ͨ��sha256 ���ܴ洢
        _PASSWD = picosha2::hash256_hex_string(passw);
    }

public:
    //һ��״̬����Ĵ���
    string sub_process(string last_status)
    {
        showwelcome();
        cout << "�������˺�(ID)" << endl;
        //��½�˻�����֤
        string ID, passwd;
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// �Թ�����
        
        cin >> ID;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// �Թ�����
        }
        if (ID == "admin")
        {
            cout << "����������(PASSWD)" << endl;
            cin >> passwd;
            //����ܺ��������бȽϣ��������Ĵ洢
            string shapasswd = picosha2::hash256_hex_string(passwd);
            if (shapasswd == _PASSWD)
            {
                //��½�ɹ�
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
                cout << "�������" << endl;
            }
        }
        else
        {
            cout << "�˺Ŵ���" << endl;
        }
        //����1.5s��ʾ��Ϣ
        Sleep(1500);
        return "super_login";

    }
    void showwelcome()
    {
        //�������
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
    string _PASSWD;//����sha256���ܴ洢
};

class welcome_super : public status
{
public:
    welcome_super()
    {   //������Ҫ���ȵ������������hash����ע��
        set_new_status("welcome_super", this);
    }

public:
    //һ��״̬����Ĵ���
    string sub_process(string last_status)
    {
        //�������
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
        table.add_row({ "a) ͨ��ID��ѯ��¼" });
        table.add_row({ "b) ͨ������(name)��ѯ��¼" });
        table.add_row({ "c) ͨ��ID��ѯ�˻���Ϣ" });
        table.add_row({ "d) ͨ������(name)��ѯ�˻���Ϣ" });
        table.add_row({ "e) ����һ���˻�" });
        table.add_row({ "f) �˳���¼" });
        table[1][0].format().font_background_color(Color::blue);
        table[2][0].format().font_background_color(Color::blue);
        table[3][0].format().font_background_color(Color::blue);
        table[4][0].format().font_background_color(Color::blue);
        table[5][0].format().font_background_color(Color::green);
        table[6][0].format().font_background_color(Color::red);
        cout << table << endl;
        //����ѡ��
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
    {   //������Ҫ���ȵ������������hash����ע��
        set_new_status("query_id", this);
    }

public:
    //һ��״̬����Ĵ���
    string sub_process(string last_status)
    {
        showwelcome();
        
        //��ѯ���˺�
        query();
        cout << "����e�˳�,����������ĸ������ѯ, �س�ȷ��"<<endl;
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
        //�������
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
        cout << "��ѯ��¼��Ϣ" << endl;
        cout << "��������Ҫ��ѯ���˺�(ID)" << endl;
        unsigned long long int ID;
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// �Թ�����
        
        cin >> ID;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// �Թ�����
        }
        Table table_res;
        table_res.format()
            .font_align(FontAlign::center)
            .corner("*")
            .width(20);

        table_res.add_row({ "ID", "����", "����", "���", "������" });
        table_res[0].format().font_color(Color::yellow);
        auto it = dat->get_log_data(ID);
        if (it.first == it.second)
        {
            cout << endl << "��ѯʧ��" << endl;
        }
        for (auto i = it.first; i != it.second; i++)
        {
            money_log* ml = i->second;
            string d = (ml->type == log_type::Deposit) ? "��Ǯ" : "ȡǮ";
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
    {   //������Ҫ���ȵ������������hash����ע��
        set_new_status("query_name", this);
    }

public:
    //һ��״̬����Ĵ���
    string sub_process(string last_status)
    {
        showwelcome();
        
        //��ѯ
        query();
        cout << "����e�˳�,����������ĸ������ѯ, �س�ȷ��" << endl;
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
        //�������
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
    //��ѯ����
    void query()
    {
        cout << "��ѯ��¼��Ϣ" << endl;
        cout << "��������Ҫ��ѯ����(name)" << endl;
        string name;
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// �Թ�����
        
        cin >> name;
        Table table_res;
        table_res.format()
            .font_align(FontAlign::center)
            .corner("*")
            .width(20);

        table_res.add_row({ "ID", "����", "����", "���", "������" });
        table_res[0].format().font_color(Color::yellow);
        //�����Ĳ�ѯ����һ��
        auto it = dat->get_log_data(name);
        if (it.first == it.second)
        {
            cout << endl << "��ѯʧ��" << endl;
        }
        for (auto i = it.first; i != it.second; i++)
        {
            money_log* ml = i->second;
            string d = (ml->type == log_type::Deposit) ? "��Ǯ" : "ȡǮ";
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
    {   //������Ҫ���ȵ������������hash����ע��
        set_new_status("query_acc_id", this);
    }

public:
    //һ��״̬����Ĵ���
    string sub_process(string last_status)
    {
        showwelcome();

        //��ѯ���˺�
        query();
        cout << "����e�˳�,����������ĸ������ѯ, �س�ȷ��" << endl;
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
        //�������
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
        cout << "��ѯ�˻���Ϣ" << endl;
        cout << "��������Ҫ��ѯ���˺�(ID)" << endl;
        unsigned long long int ID;
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// �Թ�����
        
        cin >> ID;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// �Թ�����
        }
        Table table_res;
        table_res.format()
            .font_align(FontAlign::center)
            .corner("*")
            .width(25);

        table_res.add_row({ "ID", "����", "��ַ", "�ܽ��" });
        table_res[0].format().font_color(Color::yellow);
        auto it = dat->get_account_data(ID);
        if (it.first == it.second)
        {
            cout << endl << "��ѯʧ��" << endl;
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
    {   //������Ҫ���ȵ������������hash����ע��
        set_new_status("query_acc_name", this);
    }

public:
    //һ��״̬����Ĵ���
    string sub_process(string last_status)
    {
        showwelcome();
        //��ѯ
        query();
        cout << "����e�˳�,����������ĸ������ѯ, �س�ȷ��" << endl;
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
        //�������
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
    //��ѯ����
    void query()
    {
        cout << "��ѯ�˻���Ϣ" << endl;
        cout << "��������Ҫ��ѯ����(name)" << endl;
        string name;
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// �Թ�����
        
        cin >> name;
        Table table_res;
        table_res.format()
            .font_align(FontAlign::center)
            .corner("*")
            .width(25);

        table_res.add_row({ "ID", "����", "��ַ", "�ܽ��" });
        table_res[0].format().font_color(Color::yellow);
        //�����Ĳ�ѯ����һ��
        auto it = dat->get_account_data(name);
        if (it.first == it.second)
        {
            cout << endl << "��ѯʧ��" << endl;
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
    {   //������Ҫ���ȵ������������hash����ע��
        set_new_status("create_account", this);
    }

public:
    //һ��״̬����Ĵ���
    string sub_process(string last_status)
    {
        showwelcome();
        account a;
        do 
        {
            cout << "�������˺�(ID) ������" << endl;
            cin >> a.ID; 
            if (cin.fail())
            {
                cout << "�˺�Ϊ������" << endl;
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// �Թ�����
            }
            else
            {
                auto it = dat->get_account_data(a.ID);
                if (it.first == it.second)
                {
                    break;
                }
                else
                    cout << "�˺��Ѵ���" << endl;
            }
        } while (true);  //������
        cout << "����������" << endl;
        cin >> a.passwd;
        //��passw����sha256���ܴ���
        a.passwd = picosha2::hash256_hex_string(a.passwd);
        
        cout << "����������" << endl;
        cin >> a.name;
        cout << "�������ַ" << endl;
        cin >> a.address;
        do
        {
            cout << "������Ԥ���" << endl;
            cin >> a.money;
            if (cin.fail())
            {
                cout << "Ԥ���Ϊ������" << endl;
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');// �Թ�����
            }
            else
                break;
        } while (true);  //������

        dat->add_account(a);
        cout << "�����ɹ�!" << endl;
        cout << "����e�˳�,����������ĸ��������, �س�ȷ��" << endl;
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
        //�������
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
