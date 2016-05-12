/*
 * 该类简述：
 * 主要功能：
 */

/*
 * 文件:   Module.cpp
 * 创建者: cyf
 *
 * 创建于 2016年5月11日, 上午10:59
 *
 * 修改者1：[请填写]
 *
 * 最后修改于 [请填写]
 */

#ifndef MODULE_H
#define MODULE_H

#include <string>
#include <vector>

class NetLayer;
class ControlLayer;
class ModelLayer;
class HTTPMessage;

class Module {
public:
    Module();
    Module(const Module& orig);
    virtual ~Module();

    NetLayer* netLayer;
    ControlLayer* controlLayer;
    ModelLayer* modelLayer;

    virtual int init() = 0;

    /**
     * 处理HTTPMessage消息，并给出处理结果
     * @param message 需要处理的消息
     * @param filenames 处理结果涉及的多个文件路径名称（如果有文件）
     * @return 处理结果
     */
    virtual std::string handleMessage(
            HTTPMessage message,
            std::vector<std::string>& filenames
            ) = 0;

private:

};

#endif /* MODULE_H */

