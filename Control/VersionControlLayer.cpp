/*
 * 该类简述：
 * 主要功能：
 */

/*
 * 文件:   VersionControlLayer.cpp
 * 创建者: cyf
 *
 * 创建于 2016年5月11日, 下午6:46
 *
 * 修改者1：[请填写]
 *
 * 最后修改于 [请填写]
 */

#include "VersionControlLayer.h"
#include "../Global.hpp"

#include <queue>
//#include <string>

using namespace std;
using namespace rapidjson;  // rapidjson



VersionControlLayer::VersionControlLayer() : ControlLayer()
{
}

VersionControlLayer::VersionControlLayer(const VersionControlLayer& orig)
{
}

VersionControlLayer::~VersionControlLayer()
{
}

/**
* 处理HTTPMessage消息，并给出处理结果
* @param message 需要处理的消息
* @param filenames 处理结果涉及的多个文件路径名称（如果有文件）
* @return 处理结果
*/
std::string VersionControlLayer::handleMessage(HTTPMessage message,
                                               vector<std::string>& filenames)
{
    if(message.command == "push")
    {
        if(message.step == "start")
        {

        }
        else if(message.step == "result")
        {

        }
        else  // step == [1-n]
        {

        }
    }
    else if(message.command == "compare")
    {

    }
    else if(message.command == "pull")
    {

    }

    // 返回结果
    filenames.push_back("/home/cyf/testfiles/test.txt");
    filenames.push_back("/home/cyf/testfiles/ttt");
    filenames.push_back("/home/cyf/testfiles/test.txt");
    return "OK";
}

int VersionControlLayer::compareFile(std::string file1, std::string file2,
                                     std::string filename)
{
    return 0;
}

int VersionControlLayer::mergeFile(std::string file0,
                                   std::string file1,
                                   std::string file2,
                                   std::string filename)
{
    // parse into document
    Document doc0, doc1, doc2;
    doc0.Parse(file0.c_str());
    doc1.Parse(file1.c_str());
    doc2.Parse(file2.c_str());

    queue<Value*> nodeQueue0;
    queue<Value*> nodeQueue1;
    queue<Value*> nodeQueue2;

    // 获取nodelist
    Value& n0 = doc0["description"]["diagram"]["node"];
    Value& n1 = doc1["description"]["diagram"]["node"];
    Value& n2 = doc2["description"]["diagram"]["node"];

    // 加入队列
    nodeQueue0.push(&n0);
    nodeQueue1.push(&n1);
    nodeQueue2.push(&n2);

    while(!nodeQueue0.empty())
    {
        // 从队列获取nodelist
        Value* nodelist0 = nodeQueue0.front();
        Value* nodelist1 = nodeQueue1.front();
        Value* nodelist2 = nodeQueue2.front();


        // ------------ nodelist1----------->>>
        assert((*nodelist1).IsArray());
        for(SizeType i = 0;i < (*nodelist1).Size(); i++)
        {
            // 获取 node[i]的id
            Value& id1_value = (*nodelist1)[i]["@shape_id"];
            string id1_string = id1_value.GetString();

            // 在nodelist2中找到相同id的节点
            int j = findNodeByID((*nodelist2), id1_string);  // 返回找到的节点下标

            // 如果找到了相同节点，比较是否所有属性相同
            if(j >= 0)
            {
                // --------将子节点数组加入队列---->>>
                // 从nodelist0中取出该节点（1、2有共同节点，一定是从0继承的，0一定存在该点）
                int k = findNodeByID((*nodelist0), id1_string);
                
                Value& subn0 = (*nodelist1)[k]["node"];
                Value& subn1 = (*nodelist1)[i]["node"];
                Value& subn2 = (*nodelist2)[j]["node"];

                // 加入队列
                nodeQueue0.push(&subn0);
                nodeQueue1.push(&subn1);
                nodeQueue2.push(&subn2);
                // <<<-----将子节点数组加入队列-------

                // ------对比text------>>>
                bool propertySame12 = compareProperty((*nodelist1)[i], (*nodelist2)[j], "text");
                // 如果不相同则需要各自检查相对于基础版本的变动情况
                if(propertySame12 == false)
                {
                    bool propertySame01 = false, propertySame02 = false;

                    // 比较0和1的text
                    propertySame01 = compareProperty((*nodelist0)[i], (*nodelist1)[j], "text");

                    // 比较0和2的text
                    propertySame02 = compareProperty((*nodelist0)[i], (*nodelist2)[j], "text");

                    // 如果两者都发生变更，则冲突
                    if(propertySame01 == false && propertySame02 == false)
                    {
                        // TODO: 记录冲突
                    }
                    // 如果1变了2没变，则用1覆盖2
                    else if(propertySame01 == false && propertySame02 == true)
                    {
                        // TODO: 记录合并

                        // TODO: 合并
                    }
                    // 如果2变了1没变，则保留2
                    else if(propertySame01 == true && propertySame02 == false)
                    {
                        // TODO: 记录合并

                        // TODO: 合并
                    }
                    else // 两者都没变更的情况，不可能发生
                    {}
                }  // if
                // <<<------对比text------

                // ------对比跳转链接------>>>
                propertySame12 = compareProperty((*nodelist1)[i], (*nodelist2)[j], "navi");
                // 如果不相同则需要各自检查相对于基础版本的变动情况
                if(propertySame12 == false)
                {
                    bool propertySame01 = false, propertySame02 = false;

                    // 比较0和1的text
                    propertySame01 = compareProperty((*nodelist0)[i], (*nodelist1)[j], "navi");

                    // 比较0和2的text
                    propertySame02 = compareProperty((*nodelist0)[i], (*nodelist2)[j], "navi");

                    // 如果两者都发生变更，则冲突
                    if(propertySame01 == false && propertySame02 == false)
                    {
                        // TODO: 记录冲突
                    }
                    // 如果1变了2没变，则用1覆盖2
                    else if(propertySame01 == false && propertySame02 == true)
                    {
                        // TODO: 记录合并

                        // TODO: 合并
                    }
                    // 如果2变了1没变，则保留2
                    else if(propertySame01 == true && propertySame02 == false)
                    {
                        // TODO: 记录合并

                        // TODO: 合并
                    }
                    else // 两者都没变更的情况，不可能发生
                    {}
                }  // if
                // <<<------对比跳转链接------

            }
            else  // 如果nodelist2中没有找到相同节点，则从nodelist0中找，以确定节点的增删情况
            {
                // 在nodelist0中找到相同id的节点
                int k = findNodeByID(*nodelist0, id1_string);  // 返回找到的节点下标

                // 如果版本0存在该节点，则这是版本2删除的节点
                if(k>=0)
                {
                    // 比较是否版本1相对版本0存在不同
                    if(diffNodeTree((*nodelist1)[i], (*nodelist0)[k]))  // 存在删除冲突
                    {
                        // 该节点原样存入doc2,并在冲突文件中标注为删除冲突
                        // 该节点加入到nodelist2
                        Value cp;
                        cp.CopyFrom((*nodelist1)[i], doc1.GetAllocator());
                        nodelist2->PushBack(cp, doc2.GetAllocator());

                        // 相关连线加入doc["connection"]
                        cpyConnections(doc1, doc2, (*nodelist1)[i]["anchors"]);

                        // TODO: record conflict
                        
                    }
                    else  // 不存在删除冲突
                    {
                        // 合并:不需要处理，维持版本2对该节点的删除状态

                        // TODO: record merge
                    }
                }
                else // 版本0没有这个节点，则这是1添加的节点
                {
                    // TODO: 合并:将该节点增加至diagram2,并将相关连线加入diagram2的connections数组

                    // TODO: record merge
                }
            }
        }  // for
        // <<<--------- nodelist1--------------

        // ------------ nodelist2----------->>>
        for(SizeType j = 0;j < (*nodelist2).Size(); j++)
        {
            // TODO: check list2
        }  // for

        // 将刚处理完的nodelist弹出队列
        nodeQueue0.pop();
        nodeQueue1.pop();
        nodeQueue2.pop();
    }


    // 将doc2和merge结果存下来
//    StringBuffer buffer;
//    Writer<StringBuffer> writer(buffer);
//    doc1.Accept(writer);
//    // Output
//    std::cout << buffer.GetString() << std::endl;

    return 0;
}

// <<----------- tools --------------->>
/**
 * 寻找指定id的节点
 * @param nodes
 * @param id
 * @return
 */
int VersionControlLayer::findNodeByID(Value& nodes,string id)
{
    bool found = false;
        SizeType j;
        for(j = 0;j < nodes.Size(); j++)
        {
            // 获取 node[j] 的id
            Value& id2_value = nodes[j]["@shape_id"];
            string id2_string = id2_value.GetString();

            // 如果id相同，即找到相同节点
            if(id == id2_string)
            {
                found = true;
                break;
            }
        }
    return found == true?j:-1;
}

/**
 * 比较两个节点当中的某个属性是否相同
 * @param node1
 * @param node2
 * @param name
 * @return
 */
bool VersionControlLayer::compareProperty(Value& node1, Value& node2, string name)
{
    // 在两个节点中分别找出指定属性值

    // compare
    return true;
}

/**
 * 比较两个节点以及其所有子节点是否不相同
 * @param node1
 * @param node2
 * @return 不相同则返回true，否则false
 */
bool VersionControlLayer::diffNodeTree(Value& node1, Value& node2)
{
    // 如果该层节点的属性不一致，则返回true
    if(compareProperty(node1, node2, "text") ||
       compareProperty(node1, node2, "linkto") ||
       compareProperty(node1, node2, "anchors") ||
       compareProperty(node1, node2, "nodelist")
                       )
    {
        return true;
    }

    Value& subnodes1 = node1["node"];
    Value& subnodes2 = node2["node"];

    // 对于每个nodes1中的节点，在nodes2中找到对应id的节点，然后递归比较，一旦发现不一致则返回true
    for(int i=0;i<subnodes1.Size();i++)
    {
        // 获取 node[i]的id
        Value& id1_value = subnodes1[i]["@shape_id"];
        string id1_string = id1_value.GetString();

        // 在nodes2中找对应id的节点(一定存在，否则循环之前就已经返回true)
        int j = findNodeByID(subnodes2, id1_string);

        // 递归比较每个子节点
        if(diffNodeTree(subnodes1[i], subnodes2[j])) return true;
    }

    return false;
}

bool VersionControlLayer::cpyConnections(Document& src_doc, Document& dst_doc, Value& anchors)
{
    Value& src_conn = src_doc["description"]["connections"];
    Value& dst_conn = dst_doc["description"]["connections"];

    Value& v_incomming = anchors["@incomingConnections"];
    Value& v_outgoing = anchors["@outgoingConnections"];
    
    string incomming(v_incomming.GetString());
    string outgoing(v_outgoing.GetString());

    // split ids from incomming

    // for:find conn that has given id from doc1; if doc2 doesnt have this id, cpy conn to doc2

    // split ids from outgoing

    // for:find conn that has given id from doc1; if doc2 doesnt have this id, cpy conn to doc2

    return true;
}
// <<----------- tools --------------->>