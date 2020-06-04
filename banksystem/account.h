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
#include 
class account
{
public:
	account();
	~account();

private:
	unsigned long long int ID;
	std::string name;
	std::string passwd;
	std::string adress;
	long double money;

};



#endif // !_ACCOUNT_H_
