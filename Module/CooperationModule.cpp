/*
 * 该类简述：实时协作模块
 * 主要功能：用于实现多个客户端上的多用户实时协作
 */

/* 
 * 文件:   CooperationModule.cpp
 * 创建者: Chen Yufei
 * 
 * 创建于 2016年4月27日, 上午10:20
 * 
 * 修改者1：[请填写]
 * 
 * 最后修改于 [请填写]
 */

#include "CooperationModule.h"

#include "../Net/NetLayer.h"
#include "../Control/ControlLayer.h"
#include "../Control/CooperControlLayer.h"
#include "../Model/ModelLayer.h"
#include "../Global.hpp"

CooperationModule::CooperationModule() : Module()
{
}

CooperationModule::CooperationModule(const CooperationModule& orig)
{
}

CooperationModule::~CooperationModule()
{
}

int CooperationModule::init()
{
    return RES_OK;
}

/**
* 处理HTTPMessage消息，并给出处理结果
* @param message 需要处理的消息
* @param filenames 处理结果涉及的多个文件路径名称（如果有文件）
* @return 处理结果
*/
std::string CooperationModule::handleMessage(HTTPMessage message, vector<std::string>& filenames)
{
    return "";
}
