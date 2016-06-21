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
#include <set>
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
                                   std::string hash1,
                                   std::string file2,
                                   std::string hash2,
                                   std::string filename)
{
    // 
    Document conflictDoc, mergeDoc;
    conflictDoc.SetObject();
    mergeDoc.SetObject();

    Value conflict_node, merged_node, merged_navi;
    conflict_node.SetArray();
    merged_node.SetArray();
    merged_navi.SetArray();
    assert(merged_node.IsArray());

    // 将json字符串转成文档
    Document doc0, doc1, doc2;
    doc0.Parse(file0.c_str());
    doc1.Parse(file1.c_str());
    doc2.Parse(file2.c_str());

    // ------------------navi--------------->>>
    Value& navi0 = doc0["description"]["-diagram"]["navi"];
    Value& navi1 = doc1["description"]["-diagram"]["navi"];
    Value& navi2 = doc2["description"]["-diagram"]["navi"];

    for(int n=0;n<navi1.Size();n++)
    {
        Value mergeNaviObj(kObjectType);
        
        Value& href = navi1[n]["businessObjects"]["@href"];
        // 在navi2中查找同样的跳转
        if(findNaviByHref(navi2, href))  // 如果存在
        {
            // nothing to do
        }
        else  // 如果navi2中不存在同样的跳转
        {
            // 查看navi0中是否存在该跳转
            if(findNaviByHref(navi0, href))  // navi0存在该跳转，则这是被版本2删除的跳转
            {
                // 合并：从版本1中删除该跳转，并存入结果
                // 实际实现：保持navi2对该跳转的删除状态

                // 记录合并：删除
                mergeNaviObj.AddMember("version",
                                       Value(kStringType).SetString(hash2.c_str(),hash2.length()),
                                       mergeDoc.GetAllocator());
                mergeNaviObj.AddMember("action", Value("delete"), mergeDoc.GetAllocator());
                mergeNaviObj.AddMember("href",
                                       Value(kStringType).CopyFrom(href, mergeDoc.GetAllocator()),
                                       mergeDoc.GetAllocator());
            }
            else  // navi0不存在该跳转，这是被版本1添加的跳转
            {
                // 添加到 navi2
                Value temp;
                temp.CopyFrom(navi1[n], doc1.GetAllocator());
                navi2.PushBack(temp,doc2.GetAllocator());

                // 记录合并
                mergeNaviObj.AddMember("version",
                                       Value(kStringType).SetString(hash1.c_str(),hash1.length()),
                                       mergeDoc.GetAllocator());
                mergeNaviObj.AddMember("action", Value("add"), mergeDoc.GetAllocator());
                mergeNaviObj.AddMember("href",
                                       Value(kStringType).CopyFrom(href, mergeDoc.GetAllocator()),
                                       mergeDoc.GetAllocator());
            }
        }
        merged_navi.PushBack(mergeNaviObj, mergeDoc.GetAllocator());
    }  // for

    for(auto m=navi2.Begin();m!=navi2.End();m++)
    {
        Value mergeNaviObj(kObjectType);
        
        Value& href = (*m)["businessObjects"]["@href"];
        // 在navi1中查找同样的跳转
        if(findNaviByHref(navi1, href))  //
        {
            // nothing to do
        }
        else  // if navi2 doesn't have this obj
        {
            // check the obj in navi0
            if(findNaviByHref(navi0, href))  // navi0存在该跳转，则这是被版本1删除的跳转
            {
                // 合并：从版本2中删除该跳转，并存入结果
                m = navi2.Erase(m);

                // 记录合并：删除
                mergeNaviObj.AddMember("version",
                                       Value(kStringType).SetString(hash1.c_str(),hash1.length()),
                                       mergeDoc.GetAllocator());
                mergeNaviObj.AddMember("action", Value("delete"), mergeDoc.GetAllocator());
                mergeNaviObj.AddMember("href",
                                       Value(kStringType).CopyFrom(href, mergeDoc.GetAllocator()),
                                       mergeDoc.GetAllocator());

                if(m == navi2.End()) break;
            }
            else  // navi0不存在该跳转，这是被版本2添加的跳转
            {
                // 将该跳转添加到最终结果
                // 实现：保持navi2的状态

                // 记录合并
                mergeNaviObj.AddMember("version",
                                       Value(kStringType).SetString(hash2.c_str(), hash2.length()),
                                       mergeDoc.GetAllocator());
                mergeNaviObj.AddMember("action", Value("add"), mergeDoc.GetAllocator());
                mergeNaviObj.AddMember("href",
                                       Value(kStringType).CopyFrom(href, mergeDoc.GetAllocator()),
                                       mergeDoc.GetAllocator());
            }
        }
        merged_navi.PushBack(mergeNaviObj, mergeDoc.GetAllocator());
    }  // for
    // <<<---------------navi------------------

    // -----------------connections--------->>>
    mergeConnections(doc1, doc2);
    // <<<--------------connections------------

    queue<Value*> nodeQueue0;
    queue<Value*> nodeQueue1;
    queue<Value*> nodeQueue2;

    // 获取nodelist
    Value& n0 = doc0["description"]["-diagram"]["node"];
    Value& n1 = doc1["description"]["-diagram"]["node"];
    Value& n2 = doc2["description"]["-diagram"]["node"];

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
            // 声明用于保存冲突信息的对象
            Value conflictNode_i(kObjectType);
            conflictNode_i.AddMember("isDeleted", Value("false"), conflictDoc.GetAllocator());
            Value conflict_key;
            conflict_key.SetArray();

            bool conflictHappened = false;

            // 获取 node[i]的id
            Value& id1_value = (*nodelist1)[i]["@shape_id"];
            string id1_string = id1_value.GetString();

            // 将id存入 conflictNode_i
            Value temp;
            temp.CopyFrom(id1_value, doc1.GetAllocator());
            conflictNode_i.AddMember("shape_id", temp, conflictDoc.GetAllocator());

            // 在nodelist2中找到相同id的节点
            int j = findNodeByID((*nodelist2), id1_string);  // 返回找到的节点下标

            // 如果找到了相同节点，比较是否所有属性相同
            if(j >= 0)
            {
                // --------将子节点数组加入队列---->>>
                // 从nodelist0中取出该节点（1、2有共同节点，一定是从0继承的，0一定存在该点）
                int k = findNodeByID((*nodelist0), id1_string);
                
                Value& subn0 = (*nodelist0)[k]["node"];
                Value& subn1 = (*nodelist1)[i]["node"];
                Value& subn2 = (*nodelist2)[j]["node"];

                // 加入队列
                nodeQueue0.push(&subn0);
                nodeQueue1.push(&subn1);
                nodeQueue2.push(&subn2);
                // <<<-----将子节点数组加入队列-------

                // ------对比text------>>>
                Value* v_text2;
                bool propertySame12 = compareProperty((*nodelist1)[i], (*nodelist2)[j], "text", &v_text2);
                // 如果不相同则需要各自检查相对于基础版本的变动情况
                if(propertySame12 == false)
                {
                    bool propertySame01 = false, propertySame02 = false;

                    // 比较0和1的text
                    Value* v_text1;
                    propertySame01 = compareProperty((*nodelist0)[i], (*nodelist1)[j], "text", &v_text1);

                    // 比较0和2的text
                    propertySame02 = compareProperty((*nodelist0)[i], (*nodelist2)[j], "text", &v_text2);

                    // 如果两者都发生变更，则冲突
                    if(propertySame01 == false && propertySame02 == false)
                    {
                        // 记录冲突
                        conflict_key.PushBack(Value("text"), conflictDoc.GetAllocator());
                        temp.CopyFrom(*v_text1, mergeDoc.GetAllocator());
                        conflictNode_i.AddMember("text", temp, conflictDoc.GetAllocator());
                        conflictHappened = true;
                    }
                    // 如果1变了2没变，则用1覆盖2
                    else if(propertySame01 == false && propertySame02 == true)
                    {
                        // 记录合并，保留版本1的值
                        Value mergeObj(kObjectType);
                        temp.Clear();
                        temp.SetString(hash1.c_str(), hash1.length());  // 获取版本1的hash号
                        mergeObj.AddMember("version", temp, mergeDoc.GetAllocator());  // 记录所保留的是哪个版本的值
                        mergeObj.AddMember("action", Value("modify"), mergeDoc.GetAllocator());
                        mergeObj.AddMember("object", Value("text"), mergeDoc.GetAllocator());
                        temp.CopyFrom(id1_value, mergeDoc.GetAllocator());
                        mergeObj.AddMember("shape_id", temp, mergeDoc.GetAllocator());
                        temp.CopyFrom(*v_text2, mergeDoc.GetAllocator());
                        mergeObj.AddMember("attr_name", temp, mergeDoc.GetAllocator());
                        
                        merged_node.PushBack(mergeObj, mergeDoc.GetAllocator());  // 添加到 merge array

                        // 合并
                        v_text2->CopyFrom(*v_text1, mergeDoc.GetAllocator());
                    }
                    // 如果2变了1没变，则保留2
                    else if(propertySame01 == true && propertySame02 == false)
                    {
                        // 记录合并，保留版本2的值
                        Value mergeObj(kObjectType);
                        temp.Clear();
                        temp.SetString(hash2.c_str(), hash2.length());  // 获取版本2的hash号
                        mergeObj.AddMember("version", temp, mergeDoc.GetAllocator());  // 记录所保留的是哪个版本的值
                        mergeObj.AddMember("action", Value("modify"), mergeDoc.GetAllocator());
                        mergeObj.AddMember("object", Value("text"), mergeDoc.GetAllocator());
                        temp.CopyFrom(id1_value, mergeDoc.GetAllocator());
                        mergeObj.AddMember("shape_id", temp, mergeDoc.GetAllocator());
                        temp.CopyFrom(*v_text2, mergeDoc.GetAllocator());
                        mergeObj.AddMember("attr_name", temp, mergeDoc.GetAllocator());

                        merged_node.PushBack(mergeObj, mergeDoc.GetAllocator());  // 添加到 merge array

                        // 合并: nothing to do
                    }
                    else // 两者都没变更的情况，不可能发生
                    {}
                }  // if

                // 合并两个版本中，该节点的连线列表，将相关连线加入2中
                
                // <<<------对比text------
            }  // if(j >= 0)
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
                        // 该节点原样存入doc2, 在冲突文件中标注为删除冲突
                        // 该节点加入到nodelist2
                        Value cp;
                        cp.CopyFrom((*nodelist1)[i], doc1.GetAllocator());
                        nodelist2->PushBack(cp, doc2.GetAllocator());
                        // 在冲突文件中标注为删除冲突
                        Value& isDeleted = conflictNode_i["isDeleted"];
                        isDeleted.SetString(hash2.c_str(), hash2.length());
                        conflictHappened = true;

//                        // 相关连线加入doc["connection"]
//                        cpyConnections(doc1, doc2, (*nodelist1)[i]["anchors"]);
                    }
                    else  // 不存在删除冲突
                    {
                        // 合并: 维持版本2对该节点的删除状态，并删除doc2中受影响的连线
                        removeConnections(doc2, (*nodelist1)[i]["anchors"]);

                        // 记录合并
                        Value mergeObj(kObjectType);
                        temp.Clear();
                        temp.SetString(hash2.c_str(), hash2.length());  // 获取版本2的hash号
                        mergeObj.AddMember("version", temp, mergeDoc.GetAllocator());  // 记录所保留的是哪个版本的值
                        mergeObj.AddMember("action", Value("delete"), mergeDoc.GetAllocator());
                        mergeObj.AddMember("object", Value("node"), mergeDoc.GetAllocator());
                        temp.CopyFrom(id1_value, mergeDoc.GetAllocator());
                        mergeObj.AddMember("shape_id", temp, mergeDoc.GetAllocator());

                        merged_node.PushBack(mergeObj, mergeDoc.GetAllocator());  // 添加到 merge array
                    }
                }
                else // 版本0没有这个节点，则这是1添加的节点
                {
                    // 合并:将该节点增加至diagram2当前正在处理的node数组中,并将相关连线加入diagram2的connections数组
                    // 该节点加入到nodelist2
                    Value cp;
                    cp.CopyFrom((*nodelist1)[i], doc1.GetAllocator());
                    nodelist2->PushBack(cp, doc2.GetAllocator());

//                    // 相关连线加入doc["connection"]
//                    cpyConnections(doc1, doc2, (*nodelist1)[i]["anchors"]);

                    // 记录合并
                    Value mergeObj(kObjectType);
                    temp.Clear();
                    temp.SetString(hash1.c_str(), hash2.length());  // 获取版本2的hash号
                    mergeObj.AddMember("version", temp, mergeDoc.GetAllocator());  // 记录所保留的是哪个版本的值
                    mergeObj.AddMember("action", Value("add"), mergeDoc.GetAllocator());
                    mergeObj.AddMember("object", Value("node"), mergeDoc.GetAllocator());
                    temp.CopyFrom(id1_value, mergeDoc.GetAllocator());
                    mergeObj.AddMember("shape_id", temp, mergeDoc.GetAllocator());

                    merged_node.PushBack(mergeObj, mergeDoc.GetAllocator());  // 添加到 merge array
                }
            }  // else

            // 将 conflictNode_i 存入 conflict_node 数组
            if(conflictHappened == true)
                conflict_node.PushBack(conflictNode_i, conflictDoc.GetAllocator());

        }  // for
        // <<<--------- nodelist1--------------

        // ------------ nodelist2----------->>>
        auto it = nodelist2->Begin();
        for(SizeType j = 0;j < (*nodelist2).Size(); j++, it++)
        {
			// 声明用于保存冲突信息的对象
            Value conflictNode_j(kObjectType);
            conflictNode_j.AddMember("isDeleted", Value("false"), conflictDoc.GetAllocator());
            Value conflict_key;
            conflict_key.SetArray();

            bool conflictHappened = false;

            // 获取 node[j]的id
            Value& id2_value = (*nodelist2)[j]["@shape_id"];
            string id2_string = id2_value.GetString();

            // 将id存入 conflictNode_j
            Value temp;
            temp.CopyFrom(id2_value, doc2.GetAllocator());
            conflictNode_j.AddMember("shape_id", temp, conflictDoc.GetAllocator());

            // 在nodelist1中找到相同id的节点
            int i = findNodeByID((*nodelist1), id2_string);  // 返回找到的节点下标

            if(i >= 0)  // 如果找到了相同节点
            {
                // nothing to do
            }
            else  // 如果nodelist1中没有找到相同节点，则从nodelist0中找，以确定节点的增删情况
            {
                // 在nodelist0中找到相同id的节点
                int k = findNodeByID(*nodelist0, id2_string);  // 返回找到的节点下标

                // 如果版本0存在该节点，则这是版本1删除的节点
                if(k>=0)
                {
                    // 比较是否版本2相对版本0存在不同
                    if(diffNodeTree((*nodelist2)[j], (*nodelist0)[k]))  // 存在删除冲突
                    {
                        // 保留节点信息在版本2中
                        // 在冲突文件中标注为删除冲突
                        Value& isDeleted = conflictNode_j["isDeleted"];
                        isDeleted.SetString(hash1.c_str(), hash1.length());
                        conflictHappened = true;
                    }
                    else  // 不存在删除冲突
                    {
                        // 合并:删除版本2中该节点 及 相关连线
                        removeConnections(doc2, (*nodelist2)[j]["anchors"]);
                        it = nodelist2->Erase(it);

                        // 记录合并
                        Value mergeObj(kObjectType);
                        temp.Clear();
                        temp.SetString(hash1.c_str(), hash1.length());  // 获取版本1的hash号
                        mergeObj.AddMember("version", temp, mergeDoc.GetAllocator());  // 记录所保留的是哪个版本的值
                        mergeObj.AddMember("action", Value("delete"), mergeDoc.GetAllocator());
                        mergeObj.AddMember("object", Value("node"), mergeDoc.GetAllocator());
                        temp.CopyFrom(id2_value, mergeDoc.GetAllocator());
                        mergeObj.AddMember("shape_id", temp, mergeDoc.GetAllocator());

                        merged_node.PushBack(mergeObj, mergeDoc.GetAllocator());  // 添加到 merge array

                        if(it == nodelist2->End()) break;  // 如果此时刚刚删除的节点是最后一个节点，则退出循环
                    }
                }
                else  // added by 2
                {
                    // nothing to do
                }
            }  // else

            // 将 conflictNode_j 存入 conflict_node 数组
            if(conflictHappened == true)
                conflict_node.PushBack(conflictNode_j, conflictDoc.GetAllocator());
        }  // for

        // 将刚处理完的nodelist弹出队列
        nodeQueue0.pop();
        nodeQueue1.pop();
        nodeQueue2.pop();
    }

    conflictDoc.AddMember("conflict_node", conflict_node, conflictDoc.GetAllocator());
    mergeDoc.AddMember("merged_navi", merged_navi, mergeDoc.GetAllocator());
    mergeDoc.AddMember("merged_node", merged_node, mergeDoc.GetAllocator());
    
    StringBuffer buffer1;
    Writer<StringBuffer> writer1(buffer1);
    conflictDoc.Accept(writer1);
    // Output
    std::cout << buffer1.GetString() << std::endl;

    StringBuffer buffer2;
    Writer<StringBuffer> writer2(buffer2);
    mergeDoc.Accept(writer2);
    // Output
    std::cout << buffer2.GetString() << std::endl;

    StringBuffer buffer3;
    Writer<StringBuffer> writer3(buffer3);
    doc2.Accept(writer3);
    // Output
    //std::cout << buffer3.GetString() << std::endl;

    cout<<"return "<<endl;
    return 0;
}

// <<----------- tools --------------->>
/**
 * 寻找指定id的节点, TODO: 改为从一个Map中查找，与生成map的函数配合使用
 * @param nodes
 * @param id
 * @return
 */
int VersionControlLayer::findNodeByID(Value& nodes, string id)
{
    bool found = false;
        SizeType j;
        for(j = 0;j < nodes.Size(); j++)
        {
            // 获取 node[j] 的id
            Value& id2_value = nodes[j]["@shape_id"];
            string id2_string(id2_value.GetString());

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
 * @param node1 节点1
 * @param node2 节点2
 * @param name 属性名
 * @param val 如果传入非Null值，则可以获取到node2中指定属性对象
 * @return 相同则返回true，否则false
 */
bool VersionControlLayer::compareProperty(Value& node1, Value& node2, string name, Value** val)
{
    Document tempDoc;
    tempDoc.SetObject();
    // 在两个节点中分别找出指定属性值
    if(name == "text")
    {
        Value& textMember1 = node1["ext"]["text"][0];
        Value& text1 = textMember1["style"]["@value"];

        Value& textMember2 = node2["ext"]["text"][0];
        Value& text2 = textMember2["style"]["@value"];

        if(val)
            *val = &textMember2["style"]["@value"];

        return text1 == text2;
    }
    else if(name == "anchors")
    {
        Value& anchors1 = node1["anchors"];
        Value& anchors2 = node2["anchors"];

        if(val)
            *val = &node2["anchors"];

        return anchors1 == anchors2;
    }
    else if(name == "nodelist")
    {
        Value& nodelist1 = node1["nodelist"];
        Value& nodelist2 = node2["nodelist"];

        if(val)
            *val = &node2["nodelist"];

        return nodelist1 == nodelist2;
    }

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
    Value* temp;
    if(compareProperty(node1, node2, "text", &temp) == false ||
       compareProperty(node1, node2, "anchors", &temp) == false ||
       compareProperty(node1, node2, "nodelist", &temp) == false
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

/**
 * 合并两个文档的connections部分，并将结果存入dst_doc
 * @param src_doc 文档1
 * @param dst_doc 文档2，同时也是保存结果的文档
 * @return
 */
bool VersionControlLayer::mergeConnections(Document& src_doc, Document& dst_doc)
{
    Value& src_conn = src_doc["description"]["-diagram"]["connections"];
    Value& dst_conn = dst_doc["description"]["-diagram"]["connections"];

    auto findDstByID = [&](Value& cid)->int
    {
        for(SizeType i=0;i<dst_conn.Size();i++)
        {
            Value& v_id2 = dst_conn[i]["@conn_id"];
            if(cid == v_id2)
                return i;
        }
        return -1;
    };

    for(int i=0;i<src_conn.Size();i++)
    {
        Value& id1 = src_conn[i]["@conn_id"];

        int j = findDstByID(id1);
        if(j < 0)
        {
            Value temp;
            temp.CopyFrom(src_conn[i], dst_doc.GetAllocator());
            dst_conn.PushBack(temp, dst_doc.GetAllocator());
        }
    }

    return true;
}

/**
 * 将两个anchor的incomming和outgoing部分分别合并，并将结果存入dst_anchor
 * @param src_anchor anchor1
 * @param dst_anchor anchor2，同时也是保存结果的anchor
 * @return
 */
bool VersionControlLayer::mergeAnchors(Value& src_anchor, Value& dst_anchor)
{
    Value& v_src_incomming = src_anchor["@incomingConnections"];
    Value& v_src_outgoing = src_anchor["@outgoingConnections"];
    Value& v_dst_incomming = dst_anchor["@incomingConnections"];
    Value& v_dst_outgoing = dst_anchor["@outgoingConnections"];

    string s_src_incomming(v_src_incomming.GetString());
    string s_src_outgoing(v_src_outgoing.GetString());
    string s_dst_incomming(v_dst_incomming.GetString());
    string s_dst_outgoing(v_dst_outgoing.GetString());

    vector<string> src_incomming, src_outgoing;
    vector<string> dst_incomming, dst_outgoing;

    split(s_src_incomming, " ", &src_incomming);
    split(s_src_outgoing, " ", &src_outgoing);
    split(s_dst_incomming, " ", &dst_incomming);
    split(s_dst_outgoing, " ", &dst_outgoing);

    set<string> mergeIdSet;

    // ---------incomminng---------->>>
    for(auto it=src_incomming.begin();it!=src_incomming.end();it++)
    {
        mergeIdSet.insert(*it);
    }

    for(auto it=dst_incomming.begin();it!=dst_incomming.end();it++)
    {
        mergeIdSet.insert(*it);
    }

    string s_mergeIncomming = "";
    for(auto it=mergeIdSet.begin();it!=mergeIdSet.end();it++)
    {
        s_mergeIncomming += *it;
    }
    v_dst_incomming.SetString(s_mergeIncomming.c_str(), s_mergeIncomming.length());
    // <<<-------incomming--------------

    mergeIdSet.clear();
    // ---------outgoing---------->>>
    for(auto it=src_outgoing.begin();it!=src_outgoing.end();it++)
    {
        mergeIdSet.insert(*it);
    }

    for(auto it=dst_outgoing.begin();it!=dst_outgoing.end();it++)
    {
        mergeIdSet.insert(*it);
    }

    string s_mergeOutgoing = "";
    for(auto it=mergeIdSet.begin();it!=mergeIdSet.end();it++)
    {
        s_mergeOutgoing += *it;
    }
    v_dst_outgoing.SetString(s_mergeOutgoing.c_str(), s_mergeOutgoing.length());
    // <<<-------outgoing--------------
    
    return true;
}

bool VersionControlLayer::removeConnections(Document& dst_doc, Value& anchors)
{
    Value& dst_conn = dst_doc["description"]["connections"];

    Value& v_incomming = anchors["@incomingConnections"];
    Value& v_outgoing = anchors["@outgoingConnections"];

    string incomming(v_incomming.GetString());
    string outgoing(v_outgoing.GetString());

    int start = 0, end = 0;
    auto splitID = [&](string& from)->string
    {
        end = from.find(' ', start);
        if(end != string::npos)
        {
            string result = from.substr(start, end-start);
            start = end+1;
            return result;
        }
        else
            return from.substr(start, from.length()-start);
    };

    auto removeFromDstByID = [&](string cid)->bool
    {
        for(auto it=dst_conn.Begin();it!=dst_conn.End();it++)
        {
            Value& v_id2 = (*it)["@conn_id"];
            if(cid == string(v_id2.GetString()))
            {
                dst_conn.Erase(it);
                return true;
            }
        }
        return true;
    };

    // handle incomming
    while(end != string::npos)
    {
        string id = splitID(incomming);

        removeFromDstByID(id);
    }

    start = 0;
    end = 0;
    // handle outgoing
    while(end != string::npos)
    {
        string id = splitID(outgoing);

        removeFromDstByID(id);
    }

    return true;
}

bool VersionControlLayer::findNaviByHref(Value& navi, Value& href)
{
    for(int i=0;i<navi.Size();i++)
    {
        Value& h = navi[i]["businessObjects"]["@href"];
        if(h == href) return true;
    }
    return false;
}

void VersionControlLayer::split(string& s, const string delim, vector<string>* ret)
{
    size_t last = 0;
    size_t index=s.find_first_of(delim,last);
    while (index!=string::npos)
    {
        ret->push_back(s.substr(last,index-last));
        last=index+1;
        index=s.find_first_of(delim,last);
    }
    if (index-last>0)
    {
        ret->push_back(s.substr(last,index-last));
    }
}
// <<----------- tools --------------->>
