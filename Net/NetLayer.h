/*
 * 该类简述：
 * 主要功能：
 */

/* 
 * 文件:   NetLayer.h
 * 创建者: Chen Yufei
 *
 * 创建于 2016年4月27日, 上午10:12
 * 
 * 修改者1：[请填写]
 * 
 * 最后修改于 [请填写]
 */
#ifndef NETLAYER_H
#define NETLAYER_H

#include <event.h>  
#include <event2/http.h>
#include <event2/http_compat.h> 
#include <event2/http_struct.h>
#include <string>

class NetLayer {
public:
    NetLayer();
    NetLayer(const NetLayer& orig);
    virtual ~NetLayer();
    // 初始化网络参数
    int init();
    // 开始循环处理
    void startLoop();
    // root地址请求处理
    static void rootRequestHandler(struct evhttp_request *req, void *arg);
    // pull地址请求处理
    static void pullRequestHandler(struct evhttp_request *req, void *arg);
    
    // 最大buf
    int MAXBUF = 1024*16;
    // http结构
    struct event_base *base;
    struct evhttp *httpServer;
    // 地址
    std::string _ip = "0.0.0.0";
    // 端口
    int _port = 8080;
    // 超时长度
    int _timeout = 5;
    
    
private:

};

#endif /* NETLAYER_H */

