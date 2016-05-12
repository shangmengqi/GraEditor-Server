/*
 * 该类简述：实时协作模块
 * 主要功能：用于实现多个客户端上的多用户实时协作
 */

/* 
 * 文件:   CooperationModule.h
 * 创建者: Chen Yufei
 *
 * 创建于 2016年4月27日, 上午10:20
 * 
 * 修改者1：[请填写]
 * 
 * 最后修改于 [请填写]
 */

#ifndef COOPERATIONMODULE_H
#define COOPERATIONMODULE_H

#include "Module.h"

class CooperationModule : public Module {
public:
    CooperationModule();
    CooperationModule(const CooperationModule& orig);
    virtual ~CooperationModule();

    virtual int init() override;

    /**
     * 处理HTTPMessage消息，并给出处理结果
     * @param message 需要处理的消息
     * @param filenames 处理结果涉及的多个文件路径名称（如果有文件）
     * @return 处理结果
     */
    virtual std::string handleMessage(
            HTTPMessage message,
            std::vector<std::string>& filenames) override;
private:

};

#endif /* COOPERATIONMODULE_H */

