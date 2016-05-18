/*
 * 该类简述：
 * 主要功能：
 */

/* 
 * 文件:   ModelLayer.h
 * 创建者: Chen Yufei
 *
 * 创建于 2016年4月27日, 上午10:18
 * 
 * 修改者1：[请填写]
 * 
 * 最后修改于 [请填写]
 */

#ifndef MODELLAYER_H
#define MODELLAYER_H

#include <string>
#include <iostream>
#include "client.hpp"  // SequoiaDB

using namespace std;
using namespace sdbclient;  // SequoiaDB
using namespace bson;  // SequoiaDB


class ModelLayer {
public:
    ModelLayer();
    ModelLayer(const ModelLayer& orig);
    virtual ~ModelLayer();
    
    // 查询
    
    
    // 插入
    int insert(const std::string jsonStr, std::string setSpace, std::string set);
    
    // 数据库连接
    sdb connection;
    
    // 数据库连接参数
    std::string s_hostName = "localhost";
    std::string s_port = "11810";
    std::string s_usr = "sdbadmin";
    std::string s_passwd = "sdbadmin";
    
private:

};

#endif /* MODELLAYER_H */

