/*
 * 该类简述：
 * 主要功能：
 */

/* 
 * 文件:   VersionModule.h
 * 创建者: Chen Yufei
 *
 * 创建于 2016年4月27日, 上午10:20
 * 
 * 修改者1：[请填写]
 * 
 * 最后修改于 [请填写]
 */

#ifndef VERSIONMODULE_H
#define VERSIONMODULE_H

#include "Module.h"

class VersionModule : public Module {
public:
    VersionModule();
    VersionModule(const VersionModule& orig);
    virtual ~VersionModule();

    virtual int init() override;

    /**
     * 处理HTTPMessage消息，并给出处理结果
     * @param message 需要处理的消息
     * @param filenames 处理结果涉及的多个文件路径名称（如果有文件）
     * @return 处理结果
     */
    virtual std::string handleMessage(
            HTTPMessage message,
            std::vector<std::string>& filenames
            ) override;


private:

};

#endif /* VERSIONMODULE_H */

