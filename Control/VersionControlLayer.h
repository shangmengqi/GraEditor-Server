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


class VersionControlLayer : public ControlLayer{
public:
    VersionControlLayer();
    VersionControlLayer(const VersionControlLayer& orig);
    virtual ~VersionControlLayer();
private:

};

#endif /* VERSIONCONTROLLAYER_H */

