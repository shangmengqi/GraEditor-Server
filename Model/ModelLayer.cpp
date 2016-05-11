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

ModelLayer::ModelLayer()
{
    INT32 rc = SDB_OK;

    // 尝试连接
    rc = connection.connect(s_hostName.c_str(), s_port.c_str(), s_usr.c_str(), s_passwd.c_str());
    if (rc != SDB_OK) { // 连接失败
        cout << "Fail to connet to database, rc = " << rc << endl;
    }
    else { // 连接成功
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

        // 插入
        BSONObj in;
        //fromjson("test", in);
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

/**
 * 向指定空间的指定集合中插入一个json文档
 * @param jsonStr 要插入的json字符串
 * @param setSpace 指定集合空间名称
 * @param set 指定集合名称
 * @return 
 */
int ModelLayer::insert(const std::string jsonStr,
                       std::string setSpace,
                       std::string set)
{
    BSONObj in;
    fromjson(jsonStr.c_str(), in);  // 这里直接使用std::_cxx11::string不行，可能是不兼容c++11
    // 获取集合空间
    sdbCollectionSpace cs;
    connection.getCollectionSpace(setSpace.c_str(), cs);
    // 获取集合
    sdbCollection cl;
    cs.getCollection(set.c_str(), cl);
    // 插入
    cl.insert(in);
}

