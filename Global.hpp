/*
 * 该类简述：
 * 主要功能：
 */

/* 
 * 文件:   Global.hpp
 * 创建者: cyf
 *
 * 创建于 2016年5月6日, 上午9:41
 * 
 * 修改者1:[请填写]
 * 
 * 最后修改于:[请填写]
 */

#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <err.h>
#include <iostream>
#include <sstream>

#define RES_OK      0x00;
#define RES_ERROR   0x01;

class HTTPMessage
{
public:
    std::string command = "";  // 指令，pull, push, compare
    std::string commit = "";  // 提交的版本hash值
    std::string base = "";  //
    std::string description = "";  //
    int filecount = 0;  // 记录指令将一共提交多少文件
    std::string fileName = "";  // 文件名，如果有文件
    std::string fileContent = "";  // 文件内容
    std::string step = "";  // push||compare:start, 1~n, result; pull:log, all;
    std::string state = "";  //

    HTTPMessage()
    {
        std::cout<<"http message"<<std::endl;
    }

    virtual ~HTTPMessage() {

    }


    /**
     * 根据不同的key，给该实例中的变量赋值
     * @param key 所要赋值的key
     * @param value 要赋予的值
     * @return 成功则返回RES_OK，未找到对应的key则返回RES_ERROR
     */
    int assign(std::string key, std::string value)
    {
        if(key == "command")
        {
            this->command = value;
        }
        else if(key == "commit")
        {
            this->commit = value;
        }
        else if(key == "base")
        {
            this->base = value;
        }
        else if(key == "description")
        {
            this->description = value;
        }
        else if(key == "filecount")
        {
            std::stringstream ss;
            ss<<value;
            ss>>this->filecount;
            ss.clear();
        }
        else if(key == "filename")
        {
            this->fileName = value;
        }
        else if(key == "fileContent")
        {
            this->fileContent = value;
        }
        else if(key == "step")
        {
            this->step = value;
        }
        else if(key == "state")
        {
            this->state = value;
        }
        else
        {
            return RES_ERROR;
        }
        return RES_OK;
    }
};

#endif /* GLOBAL_HPP */

