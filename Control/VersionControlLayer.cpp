/*
 * 该类简述：
 * 主要功能：
 */

/* 
 * 文件:   VersionControlLayer.cpp
 * 创建者: cyf
 * 
 * 创建于 2016年5月11日, 下午6:46
 * 
 * 修改者1：[请填写]
 * 
 * 最后修改于 [请填写]
 */

#include "VersionControlLayer.h"
#include "../Global.hpp"
#include "../Thirdpart/rapidjson/document.h"  // rapidjson
#include "../Thirdpart/rapidjson/stringbuffer.h"  // rapidjson
#include "../Thirdpart/rapidjson/writer.h"  // rapidjson
//#include <string>

using namespace std;
using namespace rapidjson;  // rapidjson

VersionControlLayer::VersionControlLayer() : ControlLayer()
{
}

VersionControlLayer::VersionControlLayer(const VersionControlLayer& orig)
{
}

VersionControlLayer::~VersionControlLayer()
{
}

/**
* 处理HTTPMessage消息，并给出处理结果
* @param message 需要处理的消息
* @param filenames 处理结果涉及的多个文件路径名称（如果有文件）
* @return 处理结果
*/
std::string VersionControlLayer::handleMessage(HTTPMessage message, vector<std::string>& filenames)
{
    if(message.command == "push")
    {
        if(message.step == "start")
        {

        }
        else if(message.step == "result")
        {

        }
        else  // step == [1-n]
        {

        }
    }
    else if(message.command == "compare")
    {

    }
    else if(message.command == "pull")
    {

    }

    // 返回结果
    filenames.push_back("/home/cyf/testfiles/test.txt");
    filenames.push_back("/home/cyf/testfiles/ttt");
    filenames.push_back("/home/cyf/testfiles/test.txt");
    return "OK";
}

int VersionControlLayer::compareFile(std::string file1, std::string file2,
                                     std::string filename)
{
    return 0;
}