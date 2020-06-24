/**
 * @brief:	客户账户类
 *          
 * @Useage: 
 * @author:   李昊佳
 * @date:     2020.6.4
**/

#pragma once
#ifndef _ACCOUNT_H_
#define _ACCOUNT_H_

#include <string>
#include <iostream>
#include "csvfile.h"
#include <vector>
#include <unordered_map>
#include <ctime>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace csv;
typedef struct account
{
	account(unsigned long long int id, string n, string pass, string adr, long double mon)
		:ID(id), name(n), passwd(pass), address(adr), money(mon)
	{};
	account() {};
	unsigned long long int ID;
	std::string name;
	std::string passwd;//采用sha256存储
	std::string address;
	long double money;

}account;

enum class log_type
{
	Deposit,//存款
	Withdraw //取款
};
//主要是用来类型转化
std::istream& operator>>(std::istream& is, log_type& dat)
{
	/*string n;
	is >> n;
	//用int保存会更好一点，这里是为了直观
	if (n == "Withdraw")
		dat = log_type::Withdraw;
	else dat = log_type::Deposit;
	return is;*/
	int n;
	is >> n;
	dat = log_type(n);
	return is;
}

std::ostream& operator<<(std::ostream& os, log_type& dat)
{
	int n;
	n = (int)dat;
	os << n;
	return os;
}
typedef struct money_log
{
	money_log(unsigned long long int id, string dat, log_type typ, long double mon, string trans)
		:ID(id), date(dat), type(typ),money(mon),transactor(trans)
	{};
	unsigned long long int ID;
	std::string date;
	log_type type;
	long double money;
	std::string transactor;//经办人

}money_log;


class datamartix
{
public:
	datamartix(string clientpath,string logpath);
	~datamartix();
	auto  get_account_data(unsigned long long int n)
	{
		return  ID_hash.equal_range(n);
	}

	auto get_account_data(string n)
	{ 
		return  name_hash.equal_range(n);
	}

	auto get_log_data(string n)
	{
		return log_name_hash.equal_range(n);
	}
	auto get_log_data(unsigned long long int n)
	{
		return log_ID_hash.equal_range(n);
	}
	//存钱
	void deposit_money(unsigned long long int ID, long double  money, string transactor, bool syn = true)
	{
		string dt = getCurrentSystemTime();
		money_log* one_money_log = new money_log(ID,
			string(dt), log_type::Deposit, money, transactor);
		//插入hash表 快速查找
		log_ID_hash.insert({ one_money_log->ID,one_money_log });
		//首先先根据ID_hash表格查找account信息，然后获取其中的name，添加进来
		log_name_hash.insert({ (ID_hash.find(one_money_log->ID))->second->name,one_money_log });
		logpaser.push_row(ID, string(dt), log_type::Deposit, money, transactor);
		(ID_hash.find(one_money_log->ID))->second->money += money;
		long double m = (ID_hash.find(one_money_log->ID))->second->money;
		clientpaser[to_string(ID)].set<long double>("money", m);
		if (syn)
		{
			clientpaser.sync();
			logpaser.sync();
		}
	}
	//取钱
	bool withdraw_money(unsigned long long int ID, long double  money,string transactor, bool syn = true)
	{
		if ((ID_hash.find(ID))->second->money < money)
			return false;
		string dt = getCurrentSystemTime();
		money_log* one_money_log = new money_log(ID,
			string(dt),log_type::Withdraw,money, transactor);
		//插入hash表 快速查找
		log_ID_hash.insert({ one_money_log->ID,one_money_log });
		//首先先根据ID_hash表格查找account信息，然后获取其中的name，添加进来
		log_name_hash.insert({ (ID_hash.find(one_money_log->ID))->second->name,one_money_log });
		logpaser.push_row(ID, string(dt), log_type::Withdraw, money, transactor);
		(ID_hash.find(one_money_log->ID))->second->money -= money;
		long double m = (ID_hash.find(one_money_log->ID))->second->money;
		clientpaser[to_string(ID)].set<long double>("money", m);
		if (syn)
		{
			clientpaser.sync();
			logpaser.sync();
		}
		return true;
	}
	//添加一个新的账户，并保存
	void add_account(account& acc, bool syn = true)
	{
		account* account_data = new account(acc);
		long double mm = account_data->money;
		account_data->money = 0;
		//插入hash表 快速查找
		ID_hash.insert({ account_data->ID,account_data });
		name_hash.insert({ account_data->name,account_data });
		clientpaser.push_row(acc.ID, acc.name, acc.passwd, acc.address, acc.money);
		if(syn)
			clientpaser.sync();
		deposit_money(acc.ID, mm, "admin", syn);
	}
	// 获取系统当前时间
	// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
	const std::string getCurrentSystemTime(){
		time_t     now = time(0);
		struct tm  tstruct;
		char       buf[80];
		localtime_s((tm* const)&tstruct,&now);
		// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
		strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

		return buf;
	}
	//保存
	void sync()
	{
		clientpaser.sync();
		logpaser.sync();
	}
public:
	//用指针避免重复存储
	unordered_multimap<string, account*>name_hash;	//名字可能重复
	unordered_map<unsigned long long int, account*>ID_hash;
	unordered_multimap<string, money_log*> log_name_hash;
	unordered_multimap<unsigned long long int, money_log*> log_ID_hash;
	string clientpath, logpath;
	Parser clientpaser, logpaser;
	
};

datamartix::datamartix(string clientpath, string logpath)
	:clientpath(clientpath), logpath(logpath),
	clientpaser(clientpath, { "ID","name","passwd","adress","money" }),
	logpaser(logpath, { "ID","date","type","money","transactor" })  //使用构造函数 只能在这里初始化
{
	//vector<string> client_cols({ "ID","name","passwd","adress","money" });
	//vector<string> log_cols({ "ID","date","type","money","transactor" });
	//自动装载文件或者创建文件
	//Parser  clientpaser(clientpath, client_cols),logpaser(logpath, log_cols);
	//获取行数 加入datamartix
	unsigned long long int ar=clientpaser.rowcount();
	for (unsigned long long int i = 0; i < ar; i++)
	{
		//这里测试我写的各种用法
		account* account_data = new account(clientpaser[i].get_value<unsigned long long int>("ID"),
			clientpaser[i][1],
			clientpaser[i]["passwd"],
			clientpaser[i]["adress"],
			clientpaser[i].get_value<long double>("money")
		);
		//插入hash表 快速查找
		ID_hash.insert({ account_data->ID,account_data });
		name_hash.insert({ account_data->name,account_data});
	}

	unsigned long long int lr = logpaser.rowcount();
	for (unsigned long long int i = 0; i < lr; i++)
	{
		csv::Row one_log = logpaser[i];
		//Row one_log = logpaser.get_row(i);
		//这里测试我写的各种用法
		money_log* one_money_log = new money_log(one_log.get_value<unsigned long long int>(0),
			one_log["date"],
			one_log.get_value<log_type>("type"),
			one_log.get_value<long double>("money"),
			one_log.get_value<string>("transactor")
		);
		//插入hash表 快速查找
		log_ID_hash.insert({ one_money_log->ID,one_money_log });
		//首先先根据ID_hash表格查找account信息，然后获取其中的name，添加进来
		log_name_hash.insert({ (ID_hash.find(one_money_log->ID))->second->name,one_money_log });
	}

}

datamartix::~datamartix()
{
	//释放内存
	for (auto i = ID_hash.begin(); i != ID_hash.end(); i++)
	{
		delete i->second;
	}
	for (auto i = log_ID_hash.begin(); i != log_ID_hash.end(); i++)
	{
		delete i->second;
	}
}

#endif // !_ACCOUNT_H_
