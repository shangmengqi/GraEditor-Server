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
    netLayer->module = this;
    /*
     * TODO:
     * 将this注入控制层
     */
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
    return "OK";
}

