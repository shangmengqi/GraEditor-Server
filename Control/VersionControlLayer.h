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
#include <thread>
#include <mutex>
#include <atomic>

class Mission
{
public:
    //
    bool needCompare = false;
    //
    std::string base = "";
    //
    int stepWhole = 0;

    //
    void stepForward()
    {
        //_lock.lock();
        stepNow++;
        //_lock.unlock();
    }
    //
    bool isOver()
    {
        return stepNow == stepWhole;
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
            HTTPMessage message,
            std::vector<std::string>& filenames
            ) override;

    // 子线程执行，处理push-start命令，创建mission
    void handlePushStart(HTTPMessage message);
    // 子线程执行，处理push的文件
    // 如果mission提示是新版本，则直接存入数据库
    // 如果mission提示是可能冲突的版本，则进行比较
    void handlePushFile(HTTPMessage message);
    // 直接执行，处理push-result命令
    // 返回协议指定内容
    std::string handlePushResult(
                HTTPMessage message,
                std::vector<std::string>& filenames
                );

    // 直接执行，处理pull命令，先区分log和all
    void handlePull(HTTPMessage message, std::vector<std::string>& filenames);

    // 子线程执行，处理compare-start命令，创建mission
    // 如果是已提交的两个版本，则由该函数创建子线程，从数据库中获取两个版本进行文件比较
    void handleCompareStart(HTTPMessage message);
    // 子线程执行，从message获取文件内容，与数据库中另一版本进行文件比较
    void handleCompareFile(HTTPMessage message);
    // 直接执行，处理compare-result
    // 返回协议指定内容
    std::string handleCompareResult(
                HTTPMessage message,
                std::vector<std::string>& filenames
                );

    // save to filename.conflict    e.g. test.xml.conflict
    int compareFile(std::string file1, std::string file2, std::string filename);
    // save to filename.conflict and filename.merge; if no conflict, save as bson into DB
    int mergeFile(std::string file1, std::string file2, std::string filename);

private:
    std::map<std::string, Mission> missionMap;

};

#endif /* VERSIONCONTROLLAYER_H */

