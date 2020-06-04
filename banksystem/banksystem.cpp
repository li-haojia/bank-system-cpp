// banksystem.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>
#include "csvfile.h"
#include <direct.h>
using namespace std;
using namespace csv;
int main(int argc, char** argv)
{
    string cwdpath(argv[0]);//参数0表示程序路径，通过\找到工作目录
    cwdpath = cwdpath.substr(0, cwdpath.rfind('\\')+1) + "new.csv";

    
    
}


