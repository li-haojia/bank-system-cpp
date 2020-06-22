// banksystem.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>
#include "csvfile.h"
#include <direct.h>
#include <unordered_map>
#include "status.h"
#include "account.h"

int main(int argc, char** argv)
{
    string cwdpath(argv[0]);//参数0表示程序路径，通过\找到工作目录
    cwdpath = cwdpath.substr(0, cwdpath.rfind('\\')+1);
    string clientpath(cwdpath + "client.csv");
    string logpath(cwdpath + "log.csv");

    datamartix dat(clientpath, logpath);
    //状态子类要在这里实例化
    welcome w;
    super_login s1;
    welcome_super s2;
    query_id s3(dat);
    query_name s4(dat);
    query_acc_id s5(dat);
    query_acc_name s6(dat);
    atm_login c1(dat);
    welcome_atm c2;
    atm_showlog c3(dat);
    atm_withdraw c4(dat);
    atm_deposit c5(dat);
    create_account s7(dat);

    //指定启动节点
    status::process("welcome");
    return 0;
    
}

