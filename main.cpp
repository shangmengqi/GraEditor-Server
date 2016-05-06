/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * 文件:   main.cpp
 * 创建者: Chen Yufei
 *
 * 创建于 2016年4月26日, 下午2:13
 * 
 * 修改者1：[请填写]
 * 
 * 最后修改于 [请填写]
 */

#include <cstdlib>
#include <iostream>
#include <event.h>
#include "client.hpp"

#include "Net/NetLayer.h"

using namespace std ;
using namespace sdbclient ;
using namespace bson ;

/*
 * 
 */
int main(int argc, char** argv)
{
    NetLayer netLayer;
    netLayer.init();
    netLayer.startLoop();
    
    return 0;
}

