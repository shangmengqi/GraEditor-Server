/*
 * 该类简述：
 * 主要功能：
 */

/* 
 * 文件:   ControlLayer.h
 * 创建者: Chen Yufei
 *
 * 创建于 2016年4月27日, 上午10:13
 * 
 * 修改者1：[请填写]
 * 
 * 最后修改于 [请填写]
 */

#ifndef CONTROLLAYER_H
#define CONTROLLAYER_H

#include <string>
#include <vector>

class Module;
class HTTPMessage;

class ControlLayer {
public:
    ControlLayer();
    ControlLayer(const ControlLayer& orig);
    virtual ~ControlLayer();

    virtual std::string handleMessage(
            HTTPMessage message,
            std::vector<std::string>& filenames
            ) = 0;

    // 该控制层所关联的模块
    Module* module;
private:

};

#endif /* CONTROLLAYER_H */

