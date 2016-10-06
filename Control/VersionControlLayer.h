/*
 * 该类简述：
 * 主要功能：
 */

/*
 * 文件:   VersionControlLayer.h
 * 创建者: cyf
 *
 * 创建于 2016年5月11日, 下午6:46
 *
 * 修改者1：[请填写]
 *
 * 最后修改于 [请填写]
 */

#ifndef VERSIONCONTROLLAYER_H
#define VERSIONCONTROLLAYER_H

#include "ControlLayer.h"
#include <map>
#include <vector>
#include <set>
#include <thread>
#include <mutex>
#include <atomic>
#include <iostream>
#include "../Thirdpart/rapidjson/document.h"  // rapidjson
#include "../Thirdpart/rapidjson/stringbuffer.h"  // rapidjson
#include "../Thirdpart/rapidjson/writer.h"  // rapidjson

class Mission
{
public:
    // 表示该任务是否需要进行比较，push指令创建该任务时根据hash值决定，之后每个push处理线程都要检查该值
    bool needCompare = false;
    
    // 该任务所基于的基础版本hash
    std::string base = "";
    
    // 表示该任务最多有多少步骤，任务创建时赋值
    int stepWhole = 0;

    // 
    std::vector<std::string> filenames;

    // 每个线程执行完后执行一遍
    void stepForward()
    {
        //_lock.lock();
        stepNow++;
        //_lock.unlock();
    }
    
    // 判断该任务是否已经完成
    bool isOver()
    {
        std::cout<<"stepNow:"<<stepNow<<"/"<<stepWhole<<std::endl;
        return stepNow == stepWhole;
    }

    // 向任务中添加一个文件名
    bool addFile(std::string filename)
    {
        filenames.push_back(filename);
        return true;
    }


private:
    //
    std::atomic_int stepNow{0};
    //
    //std::mutex _lock;

    
};

class VersionControlLayer : public ControlLayer{
public:
    VersionControlLayer();
    VersionControlLayer(const VersionControlLayer& orig);
    virtual ~VersionControlLayer();

    virtual std::string handleMessage(
            HTTPMessage& message,
            std::vector<std::string>& filenames
            ) override;

    // 子线程执行，处理push-start命令，创建mission
    void handlePushStart(HTTPMessage& message);

    // 子线程执行，处理push的文件
    // 如果mission提示是新版本，则直接存入数据库
    // 如果mission提示是可能冲突的版本，则进行比较
    void handlePushFile(HTTPMessage& message);
    
    // 直接执行，处理push-result命令
    // 返回协议指定内容
    std::string handlePushResult(
                HTTPMessage& message,
                std::vector<std::string>& filenames
                );

    // 直接执行，处理pull命令，先区分log和all
    void handlePull(HTTPMessage& message, std::vector<std::string>& filenames);

    // 子线程执行，处理compare-start命令，创建mission
    // 如果是已提交的两个版本，则由该函数创建子线程，从数据库中获取两个版本进行文件比较
    void handleCompareStart(HTTPMessage& message);
    // 子线程执行，从message获取文件内容，与数据库中另一版本进行文件比较
    void handleCompareFile(HTTPMessage& message);
    // 直接执行，处理compare-result
    // 返回协议指定内容
    std::string handleCompareResult(
                HTTPMessage& message,
                std::vector<std::string>& filenames
                );

    // save to filename.conflict    e.g. test.xml.conflict
    int compareFile(std::string& file1, std::string& file2, std::string filename);
    // save to filename.merge; if no conflict, save as bson into DB || TODO: maybe use ref for file0,1,2
    int mergeFile(const std::string& file0,
                    const std::string& file1,
                    const std::string& hash1,
                    const std::string& file2,
                    const std::string& hash2,
                    const std::string& filename);

private:
    // 保存任务，key为任务对应的hash串
    std::map<std::string, Mission*> missionMap;

    // 寻找指定id的节点
    int findNodeByID(rapidjson::Value& nodes, std::string id);
    // 比较两个节点当中的某个属性是否相同
    bool compareProperty(rapidjson::Value& node1, rapidjson::Value& node2,
                         std::string name, rapidjson::Value** val);
    // 比较两个节点以及其所有字节点是否相同
    bool diffNodeTree(rapidjson::Value& node1, rapidjson::Value& node2);

//    // cpy connections from src_doc to dst_doc, connections are chosen by anchors["incomming"] and anchors["outgoing"]
//    bool cpyConnections(rapidjson::Document& src_doc, rapidjson::Document& dst_doc, rapidjson::Value& anchors);
    
    // merge connections of 2 doc
    bool mergeConnections(rapidjson::Value& src_conn,
                          rapidjson::Value& dst_conn,
                          rapidjson::Value& filt_conn,
                          rapidjson::Document& doc);
    
    // merge anchors of 2 nodes
    bool mergeAnchors(rapidjson::Value& src_anchor,
                      rapidjson::Value& dst_anchor,
                      rapidjson::Value& filt_anchor,
                      rapidjson::Document& doc);

    //
    bool removeConnections(rapidjson::Document& dst_doc, rapidjson::Value& anchors);

    //
    bool findNaviByHref(rapidjson::Value& navi, rapidjson::Value& href);

    //
    void splitStringToSet(std::string& s,
               const std::string delim,
               std::set< std::string >* ret,
               std::set< std::string >* filt);
    //
    void filtId(std::set< std::string >* ret,
                std::set< std::string >* src,
                std::set< std::string >* filt);
    
    // transfer node array into a map
    void createNodesMap(rapidjson::Value& nodelist, std::map<std::string, int> *nodeMap);

    int saveStringToFile(std::string content, std::string path);
    
    // 检查当前提交的版本是否有并列的版本，确定是否需要比较并合并
    bool thisVersionHasBrother(std::string thisVersion, std::string baseVersion);

};

#endif /* VERSIONCONTROLLAYER_H */
