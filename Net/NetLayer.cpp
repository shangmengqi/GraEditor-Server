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
#include <vector>
#include "../Module/Module.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

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

int NetLayer::initHTTPServer(string ip, int port)
{
    this->_ip = ip;
    this->_port = port;
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
    auto layer = (NetLayer*) arg;
    std::string out;
    out = "testtesttest";
    const char* t = out.c_str();
    char _buf[layer->MAXBUF];
    memset(_buf, 0, layer->MAXBUF);

    size_t post_size = EVBUFFER_LENGTH(req->input_buffer);

    printf("len = %zd\n\n\n", post_size);
    if (post_size > 0) {
        size_t copy_len = post_size > layer->MAXBUF ? layer->MAXBUF : post_size;
        memcpy(_buf, EVBUFFER_DATA(req->input_buffer), copy_len);

        //cout << _buf<<endl;

        out.assign(_buf, copy_len);

        cout << out << endl;
    }

    //将input buffer传入decodePost中解析，获取结果
    HTTPMessage message;
    layer->decodePost(out, &message);
    cout<<"decode over"<<endl;

    std::string returnPrint = "root!\n";  // 用于回复客户端的消息

    /*
     * TODO:
     * 区分message中的command类型，决定200 OK 附带的返回内容
     * 如果需要进行文件内容比对，则交给versionModule中的控制层相关函数
     */
    vector<string> filenames;
    cout<<"handle start"<<endl;
    returnPrint = layer->module->handleMessage(message, filenames);

    // 创建回复
    struct evbuffer *buf;
    buf = evbuffer_new();
    if (buf == NULL)
        err(1, "failed to create response buffer");

    // 添加回复语句
    evbuffer_add_printf(buf, returnPrint.data());

    // 如果filenames不为空，则打开filenames指定的所有文件并add到回复中
    int filecount = filenames.size();
    for(int i=0;i<filecount;i++)
    {
        cout<<"open "<<filenames[i]<<endl;
        // 打开文件
        int fd;
        fd = open(filenames[i].c_str(), O_RDONLY);

        // 如果打开成功，则添加到回复中
        if(fd >= 0)
        {
            cout<<"open succeed"<<endl;
            // 获取文件信息
            struct stat st;
            fstat(fd, &st);
            
            // 添加到回复
            evbuffer_add_printf(buf, "\n");
            evbuffer_add_file(buf,fd,0,st.st_size);
            close(fd);
        }
        else
        {
            cout<<"open error"<<endl;
        }
    }

    // 发送回复
    evhttp_send_reply(req, HTTP_OK, "OK", buf);
}

void NetLayer::pullRequestHandler(evhttp_request* req, void* arg)
{
    auto layer = (NetLayer*) arg;
    std::string out;
    char _buf[layer->MAXBUF];

    size_t post_size = EVBUFFER_LENGTH(req->input_buffer);

    printf("len = %zd\n", post_size);
    if (post_size > 0) {
        size_t copy_len = post_size > layer->MAXBUF ? layer->MAXBUF : post_size;
        memcpy(_buf, EVBUFFER_DATA(req->input_buffer), copy_len);
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

/**
 * 解析post请求中的各项键值，并存入一个HTTPMessage对象中
 * @param input_buffer post请求提交的内容
 * @param message 用于保存解析的结果
 * @return 如果成功解析则返回RES_OK，如果未成功解析任何key，则返回RES_ERROR
 */
int NetLayer::decodePost(std::string& input_buffer, HTTPMessage* message)
{
    int result = RES_ERROR;
    int index = 0; // 记录查找的起始位置，避免重复
    // 获取分割行hash
    int hashStart = input_buffer.find("--", 0);
    int hashEnd = input_buffer.find("Content", 0) - 2;  // 退回到\r\n的位置
    string hashLine = input_buffer.substr(hashStart, hashEnd - hashStart);

    // ----------循环解析所有的key-value并给message赋值------------
    while (input_buffer.size())
    {
        int start = 0, end = 0; // 所要查找的字段值的开始和结束
        // 找到字段的key
        start = input_buffer.find(
                                  "Content-Disposition: form-data; name=\"",
                                  index
                                  ); // 获取key的位置
        if (start >= 0) // 如果找到，计算出key所在的下标
        {
            result = RES_OK;

            start += string("Content-Disposition: form-data; name=\"").size();
            end = input_buffer.find("\"", start); // 找到key的结束引号

            string key = input_buffer.substr(start, end - start);
            index = end;

            // 找到该key对应的value
            if (key != "file") // key不为file时使用常规处理方式
            {
                start = input_buffer.find("Content-Transfer-Encoding: 8bit", index);
                start += string("Content-Transfer-Encoding: 8bit").size() + 4; // 跳过换行
                end = input_buffer.find(hashLine, start) - 2;  // 退回到\r\n的位置

                message->assign(key, input_buffer.substr(start, end - start));
                cout<<key<<":"<<input_buffer.substr(start, end - start)<<endl;
                index = end;
            }
            else // key为file时需要处理filename
            {
                // 获取filename的值
                start = input_buffer.find("filename=\"", index);
                start += string("filename=\"").size();
                end = input_buffer.find("\"", start);

                message->assign("filename", input_buffer.substr(start, end - start));
                cout<<"filename:"<<input_buffer.substr(start, end - start)<<endl;
                index = end;

                // 获取文件内容
                start = input_buffer.find("Content-Transfer-Encoding: binary", index);
                start += string("Content-Transfer-Encoding: binary").size() + 4; // 跳过换行
                end = input_buffer.find(hashLine, start) - 2;  // 退回到\r\n的位置

                message->assign("fileContent", input_buffer.substr(start, end - start));
                cout<<"fileContent"<<":"<<input_buffer.substr(start, end - start)<<endl;
                index = end;
            }
        }
        else {
            break;
        }
    }  // while

    return result;
}