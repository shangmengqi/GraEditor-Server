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

#define RES_OK      0x00;
#define RES_ERROR   0x01;

class HTTPMessage
{
public:
    std::string command = "";  // 指令，pull或push
    std::string commit = "";  // 提交的版本hash值
    //std::string content_disposition = "";  // Content-Disposition字段的name值
    std::string fileName = "";  // 文件名，如果有文件
    std::string fileContent = "";  // 文件内容

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
        else if(key == "filename")
        {
            this->fileName = value;
        }
        else if(key == "fileContent")
        {
            this->fileContent = value;
        }
        else
        {
            return RES_ERROR;
        }
        return RES_OK;
    }
};

#endif /* GLOBAL_HPP */

