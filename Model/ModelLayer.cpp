/*
 * 该类简述：
 * 主要功能：
 */

/* 
 * 文件:   ModelLayer.cpp
 * 创建者: Chen Yufei
 * 
 * 创建于 2016年4月27日, 上午10:18
 * 
 * 修改者1：[请填写]
 * 
 * 最后修改于 [请填写]
 */

#include "ModelLayer.h"
#include <iostream>;
#include "client.hpp"

using namespace std;
using namespace sdbclient;
using namespace bson;

ModelLayer::ModelLayer()
{
    // 链接数据库的参数
    CHAR *pHostName = "localhost";
    CHAR *pPort = "11810";
    CHAR *pUsr = "sdbadmin";
    CHAR *pPasswd = "sdbadmin";


    // 声明数据库连接
    sdb connection;
    INT32 rc = SDB_OK;
    
    // 尝试连接
    rc = connection.connect(pHostName, pPort, pUsr, pPasswd);
    if (rc != SDB_OK) {  // 连接失败
        cout << "Fail to connet to database, rc = " << rc << endl;
    }
    else {  // 连接成功
        cout << "Connect success!" << endl;
        // 获取集合空间
        sdbCollectionSpace cs;
        connection.getCollectionSpace("GraEditor", cs);
        // 获取集合
        sdbCollection cl;
        cs.getCollection("test", cl);
        // 查询
        sdbCursor cursor;
        cl.query(cursor);
        // 从查询结果中获取值
        BSONObj obj;
        cursor.current(obj);
        cout << obj.toString() << endl;
        BSONElement e = obj["name"];
        cout << e.str() << endl;
    }

    // disconnect from database
    connection.disconnect();

}

ModelLayer::ModelLayer(const ModelLayer& orig)
{
}

ModelLayer::~ModelLayer()
{
}

