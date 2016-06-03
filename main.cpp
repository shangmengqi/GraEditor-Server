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
#include <string>
#include <event.h>
//#include "client.hpp"

#include "Net/NetLayer.h"
#include "Module/VersionModule.h"
#include "Control/CooperControlLayer.h"
#include "Module/CooperationModule.h"
#include "Control/VersionControlLayer.h"
#include "Model/ModelLayer.h"

#include "Thirdpart/rapidjson/document.h"  // rapidjson
#include "Thirdpart/rapidjson/stringbuffer.h"  // rapidjson
#include "Thirdpart/rapidjson/writer.h"  // rapidjson

using namespace std ;
//using namespace sdbclient ;
//using namespace bson ;
using namespace rapidjson;

/*
 *
 */
int main(int argc, char** argv)
{
    // ---------------test-----------
    // parse into document
    Document doc1, doc2;
    doc1.SetObject();
    doc2.SetObject();
//    doc1.Parse(file1.c_str());
//    doc2.Parse(file2.c_str());

    Value a1(kObjectType);
    a1.AddMember("test1", Value(111), doc2.GetAllocator());
    Value a2(kObjectType);
    a2.AddMember("test2", Value(222), doc2.GetAllocator());
    
    Value array;
    array.SetArray();
    Value cp;
    cp.CopyFrom(a1, doc1.GetAllocator());
    array.PushBack(cp, doc1.GetAllocator());
    cp.CopyFrom(array[0], doc1.GetAllocator());
    array.PushBack(cp, doc1.GetAllocator());
    cp.CopyFrom(a2, doc1.GetAllocator());
    array.PushBack(cp, doc1.GetAllocator());

    doc1.AddMember("aaa", a1, doc1.GetAllocator());
    doc1.AddMember("array",array, doc1.GetAllocator());
    if(doc1.HasMember("bbb"))
        Value& nil = doc1["bbb"];

//    // delete a member from array
//    Value& test = doc1["array"];
//    auto it = test.Begin();
//    it++;
//    it++;
//    test.Erase(it);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc1.Accept(writer);
    // Output {"project":"rapidjson","stars":11}
    std::cout << buffer.GetString() << std::endl;


    // ------------声明--------------
    // 声明各个模块
    VersionModule versionModule;
    //CooperationModule CooperationModule;

    // 声明控制层
    //VersionControlLayer versionControlLayer;
    //CooperControlLayer cooperControlLayer;

    // 声明网络层，暂时考虑两个模块使用类似的网络层
    NetLayer versionNetLayer;
    //NetLayer cooperNetLayer;

    // 声明持久层
    //ModelLayer modelLayer;  // 持久层暂时考虑共用

    // ------------初始化-------------
    // 初始化控制层

    // 初始化网络层
    versionNetLayer.initHTTPServer("0.0.0.0", 8080);


    // 初始化持久层

    // 初始化各个模块
    versionModule.netLayer = &versionNetLayer;
    //versionModule.controlLayer = &versionControlLayer;
    //versionModule.modelLayer = &modelLayer;
    versionModule.init();

    versionNetLayer.startLoop();
    return 0;
}
