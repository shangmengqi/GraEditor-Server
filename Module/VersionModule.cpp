/*
 * 该类简述：
 * 主要功能：
 */

/* 
 * 文件:   VersionModule.cpp
 * 创建者: Chen Yufei
 * 
 * 创建于 2016年4月27日, 上午10:20
 * 
 * 修改者1：[请填写]
 * 
 * 最后修改于 [请填写]
 */

#include "VersionModule.h"

#include "../Net/NetLayer.h"
#include "../Control/ControlLayer.h"
#include "../Control/VersionControlLayer.h"
#include "../Model/ModelLayer.h"
#include "../Global.hpp"

using namespace std;

VersionModule::VersionModule() : Module()
{
}

VersionModule::VersionModule(const VersionModule& orig)
{
}

VersionModule::~VersionModule()
{
}

int VersionModule::init()
{
    // 将this指针注入当前模块包含的网络层和控制层
    if(netLayer)
        netLayer->module = this;
    if(controlLayer)
        controlLayer->module = this;

    return RES_OK;
}

/**
* 处理HTTPMessage消息，并给出处理结果
* @param message 需要处理的消息
* @param filenames 处理结果涉及的多个文件路径名称（如果有文件）
* @return 处理结果
*/
std::string VersionModule::handleMessage(HTTPMessage message, vector<std::string>& filenames)
{
    cout<<"handle message"<<endl;
    // 交给控制层函数
    controlLayer->handleMessage(message, filenames);

//    // test返回结果
//    filenames.push_back("/home/cyf/testfiles/test.txt");
//    filenames.push_back("/home/cyf/testfiles/ttt");
//    filenames.push_back("/home/cyf/testfiles/test.txt");
    return "OK";
}

