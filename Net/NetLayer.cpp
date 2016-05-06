/* 
 * 文件:   NetLayer.cpp
 * 创建者: Chen Yufei
 * 
 * 创建于 2016年4月27日, 上午10:12
 * 
 * 修改者1：[请填写]
 * 
 * 最后修改于 [请填写]
 */

#include "NetLayer.h"
#include "../Global.hpp"
#include <iostream>
#include <string.h>

using namespace std;

NetLayer::NetLayer()
{
}

NetLayer::NetLayer(const NetLayer& orig)
{
}

NetLayer::~NetLayer()
{
    //event_free(base);
}

// 初始化网络参数
int NetLayer::init()
{
    // 初始化http
    this->base = event_base_new();
    this->httpServer = evhttp_new(base);
    if (httpServer == NULL) {
        fprintf(stderr, "Error: Unable to listen on %s:%d\n", _ip.c_str(), _port);
        return RES_ERROR;
    }
    int ret = evhttp_bind_socket(httpServer, _ip.c_str(), _port);
    if (ret != 0) {

    }

    // 设置超时
    evhttp_set_timeout(httpServer, _timeout);
    // 设置请求处理回调
    evhttp_set_cb(httpServer, "/", rootRequestHandler, this);
    // 设置请求处理回调
    evhttp_set_cb(httpServer, "/pull", pullRequestHandler, this);

    return RES_OK;
}

void NetLayer::startLoop()
{
    // 设置循环处理
    event_base_dispatch(base);
}

void NetLayer::rootRequestHandler(evhttp_request* req, void* arg)
{
    auto layer = (NetLayer*)arg;
    std::string out;
    char _buf[layer->MAXBUF];

    size_t post_size = EVBUFFER_LENGTH(req->input_buffer);

    printf("len = %zd\n", post_size);
    if (post_size > 0 )
    {
        size_t copy_len = post_size > layer->MAXBUF ? layer->MAXBUF : post_size;
        memcpy (_buf, EVBUFFER_DATA(req->input_buffer), copy_len);
        out.assign(_buf, copy_len);
    }

    // process posted data
    
    // 创建回复
    struct evbuffer *buf;

    buf = evbuffer_new();
    if (buf == NULL)
        err(1, "failed to create response buffer");
    evbuffer_add_printf(buf, "root!\n");
    evhttp_send_reply(req, HTTP_OK, "OK", buf);
}

void NetLayer::pullRequestHandler(evhttp_request* req, void* arg)
{
    auto layer = (NetLayer*)arg;
    std::string out;
    char _buf[layer->MAXBUF];

    size_t post_size = EVBUFFER_LENGTH(req->input_buffer);

    printf("len = %zd\n", post_size);
    if (post_size > 0 )
    {
        size_t copy_len = post_size > layer->MAXBUF ? layer->MAXBUF : post_size;
        memcpy (_buf, EVBUFFER_DATA(req->input_buffer), copy_len);
        out.assign(_buf, copy_len);
    }

    // process posted data
    
    // 创建回复
    struct evbuffer *buf;

    buf = evbuffer_new();
    if (buf == NULL)
        err(1, "failed to create response buffer");
    evbuffer_add_printf(buf, "pull!\n");
    evhttp_send_reply(req, HTTP_OK, "OK", buf);
}