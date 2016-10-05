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
#include <fstream>
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
std::string VersionControlLayer::handleMessage(HTTPMessage& message,
                                               vector<std::string>& filenames)
{
    string res = "OK";
    if(message.command == "push")
    {
        cout<<"handle push ";
        if(message.step == "start")
        {
            cout<<"start"<<endl;
            handlePushStart(message);
        }
        else if(message.step == "result")
        {
            cout<<"result"<<endl;
            res = handlePushResult(message, filenames);
        }
        else  // step == [1-n]
        {
            cout<<"step "<<message.step<<endl;
            handlePushFile(message);
        }
    }
    else if(message.command == "compare")
    {

    }
    else if(message.command == "pull")
    {

    }
    cout<<"---return:---"<<res<<endl;
    return res;
}

void VersionControlLayer::handlePushStart(HTTPMessage& message)
{
    // 创建任务
    Mission* p_mission = new Mission();
    p_mission->base = message.base;
    p_mission->stepWhole = message.filecount;

    // TODO: 根据版本关系判断是否需要差异比较
    p_mission->needCompare = true;

    // 添加任务
    missionMap.insert(pair<string, Mission*>(message.commit, p_mission));
}

void VersionControlLayer::handlePushFile(HTTPMessage& message)
{
    // 从map中找到相关的任务
    auto it = missionMap.find(message.commit);
    // 如果该任务存在
    if(it != missionMap.end())
    {
        auto p_mission = it->second;
        // 判断是否需要差异比较
        if(p_mission->needCompare)  // 如果需要差异比较
        {
            // TODO: 去数据库找对应版本的对应文件、版本编号
            string hash1 = "hash1";

            // ----------------- TEMP --------------------
            string filename = "/home/chenyufei/"+message.fileName+".1";
            cout<<"open file:"<<filename<<endl;
            ifstream ifs1(filename.c_str());  // 创建文件流
            std::string buf;
            std::string file1 = "";
            while(getline(ifs1, buf))  // 读文件
            {
                file1 += "\n";
                file1 += buf;
            }

            filename = "/home/chenyufei/"+message.fileName+".0";
            cout<<"open file:"<<filename<<endl;
            ifstream ifs0(filename.c_str());
            buf = "";
            std::string file0 = "";
            while(getline(ifs0, buf))
            {
                file0 += "\n";
                file0 += buf;
            }
            // ------------------ TEMP -------------------

/*            // test
            filename = "/home/cyf/midfile2.txt";
            ifstream ifs2(filename.c_str());
            buf = "";
            std::string file2 = "";
            while(getline(ifs2, buf))
            {
                file2 += "\n";
                file2 += buf;
            }
            //test
*/
            // 处理合并
            thread t([=](){  // ATTENTION: should not be [&], coz handlePushFile will free the vars
                cout<<"layer!"<<endl;
                mergeFile(file0, file1, hash1, message.fileContent, message.commit, message.fileName);
//                mergeFile(file0, file1, hash1, file2, message.commit, message.fileName);

                p_mission->stepForward();  // file merge over
            });
            t.detach();
                

            // 向任务中添加该文件的结果文件名
            p_mission->addFile(message.fileName);

        }
    }
}

string VersionControlLayer::handlePushResult(HTTPMessage& message, std::vector<std::string>& filenames)
{
    // 从map中找到相关的任务
    auto it = missionMap.find(message.commit);
    // 如果该任务存在
    if(it != missionMap.end())
    {
        auto p_mission = it->second;
        
        // 检查是否完成
        if(p_mission->isOver())  // 如果完成
        {
            // 从任务中获取结果文件名，加入filenames
            for(auto filename : p_mission->filenames)
            {
                filenames.push_back(filename + ".conflict");
                filenames.push_back(filename + ".merge");
                filenames.push_back(filename + ".res");
            }

	    string res = "OK=========\n" + message.commit;
            missionMap.erase(it);
            return res;
        }
        else return "processing";
    }
    return "error";
}

int VersionControlLayer::compareFile(std::string& file1, std::string& file2,
                                     std::string filename)
{
    return 0;
}

int VersionControlLayer::mergeFile(const std::string& file0,
                                   const std::string& file1,
                                   const std::string& hash1,
                                   const std::string& file2,
                                   const std::string& hash2,
                                   const std::string& filename)
{
    cout<<"merge file"<<endl;
    // 
    Document conflictDoc, mergeDoc;
    conflictDoc.SetObject();
    mergeDoc.SetObject();
    cout<<"set obj over"<<endl;

    Value conflict_node, merged_node, merged_navi;
    conflict_node.SetArray();
    merged_node.SetArray();
    merged_navi.SetArray();
    assert(merged_node.IsArray());

    cout<<"set array over"<<endl;

    // 将json字符串转成文档
    Document doc0, doc1, doc2;
    doc0.Parse(file0.c_str());
    doc1.Parse(file1.c_str());
    doc2.Parse(file2.c_str());

    cout<<"navi start"<<endl;
    // ------------------navi--------------->>>
    assert(doc0.IsObject());
    Value& navi0 = doc0["description"]["-diagram"]["navi"];
    Value& navi1 = doc1["description"]["-diagram"]["navi"];
    Value& navi2 = doc2["description"]["-diagram"]["navi"];

    for(int n=0;n<navi1.Size();n++)
    {
        Value mergeNaviObj(kObjectType);
        bool merged = true;
        
        Value& href = navi1[n]["businessObjects"]["@href"];
        // 在navi2中查找同样的跳转
        if(findNaviByHref(navi2, href))  // 如果存在
        {
            // nothing to do
            merged = false;
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
        if(merged == true)
            merged_navi.PushBack(mergeNaviObj, mergeDoc.GetAllocator());
    }  // for

    for(auto m=navi2.Begin();m!=navi2.End();m++)
    {
        Value mergeNaviObj(kObjectType);
        bool merged = true;
        
        Value& href = (*m)["businessObjects"]["@href"];
        // 在navi1中查找同样的跳转
        if(findNaviByHref(navi1, href))  //
        {
            // nothing to do
            merged = false;
        }
        else  // if navi2 doesn't have this obj
        {
            // check the obj in navi0
            if(findNaviByHref(navi0, href))  // navi0存在该跳转，则这是被版本1删除的跳转
            {
                // 记录合并：删除
                mergeNaviObj.AddMember("version",
                                       Value(kStringType).SetString(hash1.c_str(),hash1.length()),
                                       mergeDoc.GetAllocator());
                mergeNaviObj.AddMember("action", Value("delete"), mergeDoc.GetAllocator());
                mergeNaviObj.AddMember("href",
                                       Value(kStringType).CopyFrom(href, mergeDoc.GetAllocator()),
                                       mergeDoc.GetAllocator());

                // 合并：从版本2中删除该跳转，并存入结果
                m = navi2.Erase(m);
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
        if(merged == true)
            merged_navi.PushBack(mergeNaviObj, mergeDoc.GetAllocator());
        if(m == navi2.End()) break;
    }  // for
    // <<<---------------navi------------------

    cout<<"navi over, conn start"<<endl;
    // -----------------connections--------->>>
    Value& src_conn = doc1["description"]["-diagram"]["connections"];
    Value& dst_conn = doc2["description"]["-diagram"]["connections"];
    Value& filt_conn = doc0["description"]["-diagram"]["connections"];
    mergeConnections(src_conn, dst_conn, filt_conn, doc2);

//    StringBuffer bufferConn;
//    Writer<StringBuffer> writerConn(bufferConn);
//    doc2.Accept(writerConn);
//    // Output
//    std::cout << bufferConn.GetString() << std::endl;
    // <<<--------------connections------------
    cout<<"conn over"<<endl;

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
        cout<<"-------while-----------"<<endl;
        // 从队列获取nodelist
        Value* nodelist0 = nodeQueue0.front();
        Value* nodelist1 = nodeQueue1.front();
        Value* nodelist2 = nodeQueue2.front();

        // ------------ nodelist1----------->>>
        cout<<"-------1-----------"<<endl;
        assert((*nodelist1).IsArray());
        for(SizeType i = 0;i < (*nodelist1).Size(); i++)
        {
            cout<<"-------for-----------"<<endl;
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
                cout<<"-------j>=0-----------"<<endl;
                // 从nodelist0中取出该节点（1、2有共同节点，一定是从0继承的，0一定存在该点）
                int k = findNodeByID((*nodelist0), id1_string);

                // merge anchors
                mergeAnchors((*nodelist1)[i]["anchors"], (*nodelist2)[j]["anchors"], (*nodelist0)[k]["anchors"], doc2);

                // --------将子节点数组加入队列---->>>
                Value& subn0 = (*nodelist0)[k]["node"];
                Value& subn1 = (*nodelist1)[i]["node"];
                Value& subn2 = (*nodelist2)[j]["node"];

                // 加入队列
                nodeQueue0.push(&subn0);
                nodeQueue1.push(&subn1);
                nodeQueue2.push(&subn2);
                // <<<-----将子节点数组加入队列-------

                // ------对比text------>>>
                cout<<"-------text-----------"<<endl;
                Value* v_text2;
                bool propertySame12 = compareProperty((*nodelist1)[i], (*nodelist2)[j], "text", &v_text2);
                // 如果不相同则需要各自检查相对于基础版本的变动情况
                if(propertySame12 == false)
                {
                    cout<<"-------same12 == false-----------"<<endl;
                    bool propertySame01 = false, propertySame02 = false;

                    // 比较0和1的text
                    Value* v_text1;
                    propertySame01 = compareProperty((*nodelist0)[k], (*nodelist1)[j], "text", &v_text1);

                    // 比较0和2的text
                    propertySame02 = compareProperty((*nodelist0)[k], (*nodelist2)[j], "text", &v_text2);

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
                        //temp.Clear();
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
                        cout<<"-------same01==true,same02==false-----------"<<endl;
                        // 记录合并，保留版本2的值
                        cout<<"-------get hash2-----------";
                        Value mergeObj(kObjectType);
                        //temp.Clear();
                        temp.SetString(hash2.c_str(), hash2.length());  // 获取版本2的hash号
                        mergeObj.AddMember("version", temp, mergeDoc.GetAllocator());  // 记录所保留的是哪个版本的值
                        cout<<"-------done-----------"<<endl;
                        mergeObj.AddMember("action", Value("modify"), mergeDoc.GetAllocator());
                        mergeObj.AddMember("object", Value("text"), mergeDoc.GetAllocator());
                        cout<<"-------get id-----------";
                        temp.CopyFrom(id1_value, mergeDoc.GetAllocator());
                        mergeObj.AddMember("shape_id", temp, mergeDoc.GetAllocator());
                        cout<<"-------done-----------"<<endl;
                        cout<<"-------get attr-----------";
                        temp.CopyFrom(*v_text2, mergeDoc.GetAllocator());
                        mergeObj.AddMember("attr_name", temp, mergeDoc.GetAllocator());
                        cout<<"-------done-----------"<<endl;

                        merged_node.PushBack(mergeObj, mergeDoc.GetAllocator());  // 添加到 merge array
                        cout<<"-------push done-----------"<<endl;

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
                        //temp.Clear();
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
//                    temp.Clear();
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
            {
                conflictNode_i.AddMember("conflict_key", conflict_key, conflictDoc.GetAllocator());
                conflict_node.PushBack(conflictNode_i, conflictDoc.GetAllocator());
            }

        }  // for
        // <<<--------- nodelist1--------------

        // ------------ nodelist2----------->>>
        cout<<"-------2-----------"<<endl;
        auto it = nodelist2->Begin();
        for(SizeType j = 0;j < (*nodelist2).Size(); j++, it++)
        {
            cout<<"-------for-----------"<<endl;
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
                cout<<"-------i<0-----------"<<endl;
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
//                        temp.Clear();
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

                    // 记录合并
                    Value mergeObj(kObjectType);
//                        temp.Clear();
                    temp.SetString(hash2.c_str(), hash2.length());  // 获取版本2的hash号
                    mergeObj.AddMember("version", temp, mergeDoc.GetAllocator());  // 记录所保留的是哪个版本的值
                    mergeObj.AddMember("action", Value("add"), mergeDoc.GetAllocator());
                    mergeObj.AddMember("object", Value("node"), mergeDoc.GetAllocator());
                    temp.CopyFrom(id2_value, mergeDoc.GetAllocator());
                    mergeObj.AddMember("shape_id", temp, mergeDoc.GetAllocator());

                    merged_node.PushBack(mergeObj, mergeDoc.GetAllocator());  // 添加到 merge array
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
    cout<<"node over"<<endl;

    // add result to responding doc
    conflictDoc.AddMember("conflict_node", conflict_node, conflictDoc.GetAllocator());
    mergeDoc.AddMember("merged_navi", merged_navi, mergeDoc.GetAllocator());
    mergeDoc.AddMember("merged_node", merged_node, mergeDoc.GetAllocator());

    // 将conflictDoc 和 mergeDoc 存入以filename命名的结果文件
    StringBuffer conflictDocBuffer;
    Writer<StringBuffer> writer1(conflictDocBuffer);
    conflictDoc.Accept(writer1);
    // Output
    //std::cout << conflictDocBuffer.GetString() << std::endl;
    saveStringToFile(conflictDocBuffer.GetString(), filename+".conflict");
    cout<<"save conflict over"<<endl;

    StringBuffer mergeDocBuffer;
    Writer<StringBuffer> writer2(mergeDocBuffer);
    mergeDoc.Accept(writer2);
    // Output
    //std::cout << mergeDocBuffer.GetString() << std::endl;
    saveStringToFile(mergeDocBuffer.GetString(), filename+".merge");

    cout<<"------------------"<<endl;

    StringBuffer doc2Buffer;
    Writer<StringBuffer> writer3(doc2Buffer);
    doc2.Accept(writer3);
    // Output
    std::cout << doc2Buffer.GetString() << std::endl;
    saveStringToFile(doc2Buffer.GetString(), filename+".res");


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
bool VersionControlLayer::mergeConnections(Value& src_conn,
                                           Value& dst_conn,
                                           Value& filt_conn,
                                           Document& doc)
{
//    Value& src_conn = src_doc["description"]["-diagram"]["connections"];
//    Value& dst_conn = dst_doc["description"]["-diagram"]["connections"];
//
    //Document tempDoc(kObjectType);
    auto findByID = [&](Value& conn, Value& cid)->int
    {
        for(SizeType i=0;i<conn.Size();i++)
        {
            Value& v_id2 = conn[i]["@conn_id"];
            if(cid == v_id2)
                return i;
        }
        return -1;
    };

    for(int i=0;i<src_conn.Size();i++)
    {
        Value& id1 = src_conn[i]["@conn_id"];

        int k = findByID(filt_conn, id1);
        if(k == -1)
        {
            int j = findByID(dst_conn, id1);
            if(j < 0)
            {
                Value temp;
                temp.CopyFrom(src_conn[i], doc.GetAllocator());
                dst_conn.PushBack(temp, doc.GetAllocator());

    //            StringBuffer buffer1;
    //            Writer<StringBuffer> writer1(buffer1);
    //            dst_conn.Accept(writer1);
    //            // Output
    //            std::cout << buffer1.GetString() << std::endl;
            }
        }
    }

    for(int i=0;i<filt_conn.Size();i++)
    {
        Value& id1 = filt_conn[i]["@conn_id"];

        int k = findByID(src_conn, id1);
        if(k == -1)
        {
            int j = findByID(dst_conn, id1);
            if(j >= 0)
            {
                dst_conn.Erase(&dst_conn[j]);
            }
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
bool VersionControlLayer::mergeAnchors(Value& src_anchor,
                                       Value& dst_anchor,
                                       Value& filt_anchor,
                                       Document& doc)
{
    Value& v_src_incomming  = src_anchor["@incomingConnections"];
    Value& v_src_outgoing   = src_anchor["@outgoingConnections"];
    Value& v_dst_incomming  = dst_anchor["@incomingConnections"];
    Value& v_dst_outgoing   = dst_anchor["@outgoingConnections"];
    Value& v_filt_incomming = filt_anchor["@incomingConnections"];
    Value& v_filt_outgoing  = filt_anchor["@outgoingConnections"];

    string s_src_incomming(v_src_incomming.GetString());
    string s_src_outgoing(v_src_outgoing.GetString());
    string s_dst_incomming(v_dst_incomming.GetString());
    string s_dst_outgoing(v_dst_outgoing.GetString());
    string s_filt_incomming(v_filt_incomming.GetString());
    string s_filt_outgoing(v_filt_outgoing.GetString());

    set<string> srcIncomming, srcOutgoing;
    set<string> mergeIncomming, mergeOutgoing, mergeFilter;

    splitStringToSet(s_filt_incomming, " ", &mergeFilter, NULL);
    splitStringToSet(s_dst_incomming, " ", &mergeIncomming, NULL);
    splitStringToSet(s_src_incomming, " ", &srcIncomming, NULL);
    filtId(&mergeIncomming, &mergeFilter, &srcIncomming);
    splitStringToSet(s_src_incomming, " ", &mergeIncomming, &mergeFilter);

    mergeFilter.clear();
    splitStringToSet(s_filt_outgoing, " ", &mergeFilter, NULL);
    splitStringToSet(s_dst_outgoing, " ", &mergeOutgoing, NULL);
    splitStringToSet(s_src_outgoing, " ", &srcOutgoing, NULL);
    filtId(&mergeOutgoing, &mergeFilter, &srcOutgoing);
    splitStringToSet(s_src_outgoing, " ", &mergeOutgoing, &mergeFilter);

    // ---------incomminng---------->>>
    string s_mergeIncomming = "";
    for(auto it=mergeIncomming.begin();it!=mergeIncomming.end();it++)
    {
        s_mergeIncomming += *it;
        s_mergeIncomming += " ";
    }
    v_dst_incomming.SetString(s_mergeIncomming.c_str(), s_mergeIncomming.size(), doc.GetAllocator());
    // <<<-------incomming--------------

    // ---------outgoing---------->>>
    string s_mergeOutgoing = "";
    for(auto it=mergeOutgoing.begin();it!=mergeOutgoing.end();it++)
    {
        s_mergeOutgoing += *it;
        s_mergeOutgoing += " ";
    }
    v_dst_outgoing.SetString(s_mergeOutgoing.c_str(), s_mergeOutgoing.size(), doc.GetAllocator());
    // <<<-------outgoing--------------

//    StringBuffer buffer1;
//    Writer<StringBuffer> writer1(buffer1);
//    dst_anchor.Accept(writer1);
//    // Output
//    std::cout << buffer1.GetString() << std::endl;
    
    return true;
}

bool VersionControlLayer::removeConnections(Document& dst_doc, Value& anchors)
{
    Value& dst_conn = dst_doc["description"]["-diagram"]["connections"];

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

void VersionControlLayer::splitStringToSet(string& s,
                                const string delim,
                                set<string>* ret,
                                set<string>* filt)
{
    size_t last = 0;
    size_t index=s.find_first_of(delim,last);
    while (index!=string::npos)
    {
        string cut = s.substr(last,index-last);
        if(!filt || filt->find(cut) == filt->end())
        {
            ret->insert(cut);
        }

        last=index+1;
        index=s.find_first_of(delim,last);
    }
//    if (index-last>0)
//    {
//        ret->push_back(s.substr(last,index-last));
//    }
}

void VersionControlLayer::filtId(set<string>* ret,
                                 set<string>* src,
                                 set<string>* filt)
{
    for(auto sit = src->begin();sit!=src->end();sit++)
    {
        auto fit = filt->find(*sit);
        if(fit == filt->end())
        {
            ret->erase(*sit);
        }
    }
}

int VersionControlLayer::saveStringToFile(std::string content, std::string path)
{
    ofstream of(path);
    if(of.is_open())
    {
        of<<content;
        of.close();
        cout<<"close"<<endl;
        cout<<"path:"<<path<<endl;
        return 0;
    }
    else
    {
        return -1;
    }
}
// <<----------- tools --------------->>
