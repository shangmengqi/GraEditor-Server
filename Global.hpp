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
#pragma once
#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <err.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

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
        else if(key == "fileName")
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

// DEBUG: 暂时使用的替代数据库，存储节点
class DataNode
{
public:
    std::string version;
    std::vector<std::string> parents;
    std::vector<std::string> childs;
    std::vector<std::string> filenames;
};

// DEBUG: 暂时使用的替代数据库的东西
class VersionData
{
private:
    VersionData(){};
    std::map<std::string, DataNode*> nodes;
    
public:
    static VersionData& Instance()
    {
        static VersionData data;
        return data;
    }
    
    std::string getChild(std::string baseVersion)
    {
        auto it = nodes.find(baseVersion);
        if(it == nodes.end())  // 如果指定的base版本不存在
        {
            return "nobase";
        }
        else
        {
            // 如果指定的base版本没有其他孩子节点
            if(it->second->childs.size() == 0)
            {
                return "nochild";
            }
            else
            {
                // 暂时先返回唯一一个子节点
                return it->second->childs[0];
            }
        }
    }
    
    
    int saveVersion(std::string thisVersion, std::string baseVersion)
    {
        auto baseIt = nodes.find(baseVersion);
        
        DataNode* node = new DataNode();
        
        node->version = thisVersion;
        if(baseIt != nodes.end())
        {
            baseIt->second->childs.push_back(thisVersion);
            node->parents.push_back(baseVersion);
        }
        
        nodes.insert(std::pair<std::string,DataNode*>(thisVersion,node));
        
        return 0;
    }
    
    // 暂时先给每个节点只保存一个文件
    int addFile(std::string version, std::string filename)
    {
        auto it = nodes.find(version);
        if(it != nodes.end())
        {
            it->second->filenames.push_back(filename);
        }
        
        return 0;
    }
    
};

#endif /* GLOBAL_HPP */

