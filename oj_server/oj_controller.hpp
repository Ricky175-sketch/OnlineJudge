#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <fstream>
#include <cassert>
#include <unistd.h>
#include <jsoncpp/json/json.h>

#include "../comm/util.hpp"
#include "../comm/log.hpp"
#include "../comm/httplib.h"
#include "oj_model.hpp"
#include "oj_view.hpp"

namespace ns_controller
{
    // 引入路径拼接功能
    using namespace ns_util;
    // 引入日志功能
    using namespace ns_log;
    // 引入Model模块
    using namespace ns_model;
    // 引入View模块
    using namespace ns_view;
    // 引入http库
    using namespace httplib;

    // 提供服务的主机
    class Machine
    {
    public:
        std::string ip;  // 编译服务的ip
        int port;        // 编译服务的port
        uint64_t load;   // 编译服务的负载
        std::mutex *mtx; // 负载锁（这里使用指针，因为mutex是无法被复制的）
    public:
        Machine() : ip(""), port(0), load(0), mtx(nullptr)
        {}
        ~Machine() = default;

        // 安全地递增负载
        void IncreaseLoad()
        {
            if (mtx)
                mtx->lock();
            ++load;
            if (mtx)
                mtx->unlock();
        }
        // 安全地递减负载
        void DecreaseLoad()
        {
            if (mtx)
                mtx->lock();
            --load;
            if (mtx)
                mtx->unlock();
        }
        // 安全地获取负载
        uint64_t Load()
        {
            uint64_t ret = 0;
            if (mtx)
                mtx->lock();
            ret = load;
            if (mtx)
                mtx->unlock();
            return ret;
        }
    };

    const std::string machine_conf_path = "./conf/service_machine.conf";
    // 负载均衡模块
    class LoadBalance
    {
    private:
        std::vector<Machine> machines; // 可以提供编译服务的机器
        std::vector<int> online;       // 所有在线主机的id（id为每一台主机在machines中的下标）
        std::vector<int> offline;      // 所有离线主机的id
        std::mutex mtx;                // 保证LoadBalance数据的安全
    public:
        LoadBalance()
        {
            assert(LoadConf(machine_conf_path));
            LOG(INFO) << "加载 " << machine_conf_path << " 成功" << "\n";
        }
        ~LoadBalance() = default;

        bool LoadConf(const std::string &machine_conf)
        {
            std::ifstream in(machine_conf);
            if (!in.is_open())
            {
                LOG(FATAL) << "加载 " << machine_conf << " 文件失败" << "\n";
                return false;
            }
            std::string line;
            while (std::getline(in, line))
            {
                std::vector<std::string> tokens;
                StringUtil::SplitString(line, &tokens, ":");
                if (tokens.size() != 2)
                {
                    LOG(WARNING) << "切分 " << line << " 失败" << "\n";
                    continue;
                }
                Machine m;
                m.ip = tokens[0];
                m.port = atoi(tokens[1].c_str());
                m.load = 0;
                m.mtx = new std::mutex();

                online.push_back(machines.size());
                machines.push_back(m);
            }

            in.close();
            return true;
        }
        bool SmartChoice(int *id, Machine **m)
        {
            // 使用选择好的主机
            mtx.lock();
            int online_machine_num = online.size();
            if (online_machine_num == 0)
            {
                mtx.unlock();
                LOG(FATAL) << "编译服务主机全部离线" << "\n";
                return false;
            }

            // 找到负载最小的机器
            uint64_t min_load = machines[online[0]].Load();
            *id = online[0];
            *m = &machines[online[0]];
            for (int i = 1; i < online.size(); i++)
            {
                uint64_t cur_load = machines[online[i]].Load();
                if (cur_load < min_load)
                {
                    min_load = cur_load;
                    *id = online[i];
                    *m = &machines[online[i]];
                }
            }
            mtx.unlock();
            return true;
        }
        void OfflineMachine(int id)
        {
            mtx.lock();
            for (auto it = online.begin(); it != online.end(); it++)
            {
                if (*it == id)
                {
                    online.erase(it);
                    offline.push_back(id);
                    break;
                }
            }
            mtx.unlock();
        }
        void OnlineMachine(int id)
        {
            // TODO
        }
        // 用于测试的函数
        void ShowMachines()
        {
            mtx.lock();
            std::cout << "当前在线主机列表：";
            for (auto id : online)
                std::cout << id << " ";
            std::cout << std::endl;
            std::cout << "当前离线主机列表：";
            for (auto id : offline)
                std::cout << id << " ";
            std::cout << std::endl;
            mtx.unlock();
        }
    };

    // 核心业务逻辑的控制器
    class Controller
    {
    private:
        Model model_;              // 提供后台数据
        View view_;                // 渲染html页面
        LoadBalance load_balance_; // 负载均衡器
    public:
        Controller() = default;
        ~Controller() = default;

        /*
            根据所有的题目数据构建网页
        */
        bool AllQuestions(std::string *html)
        {
            std::vector<struct Question> all;
            if (model_.GetAllQuestions(&all))
            {
                view_.ExpandAllQuestions(all, html);
                return true;
            }
            else
            {
                *html = "加载所有题目失败";
                return false;
            }
        }

        /*
            根据给定的题目编号来构建网页
        */
        bool Question(const std::string &number, std::string *html)
        {
            struct Question q;
            if (model_.GetOneQuestion(number, &q))
            {
                view_.ExpandOneQuestion(q, html);
                return true;
            }
            else
            {
                *html = "加载该条题目失败";
                return false;
            }
        }

        /*
            提供判题功能
        */
        void Judge(const std::string &number, const std::string in_json, std::string *out_json)
        {
            // 根据题目编号获取题目信息
            struct Question q;
            model_.GetOneQuestion(number, &q);

            // 解析json得到用户提交的代码
            Json::Reader reader;
            Json::Value in_value;
            reader.parse(in_json, in_value);

            // 代码拼接
            Json::Value compile_value;
            compile_value["input"] = in_value["input"];
            compile_value["code"] = in_value["code"].asString() + q.tail;
            compile_value["cpu_limit"] = q.cpu_limit;
            compile_value["mem_limit"] = q.mem_limit;
            Json::FastWriter writer;
            std::string compile_json = writer.write(compile_value);

            // 选择负载最低的主机（一直选择，直到找到可用的主机）
            while (true)
            {
                int id = 0;
                Machine *m = nullptr;
                if (!load_balance_.SmartChoice(&id, &m))
                    break;
                
                LOG(INFO) << "选择了" << id << "号主机，地址：" << m->ip << ":" << m->port << "\n";
                
                // 发起http请求，得到结果
                Client client(m->ip, m->port);
                m->IncreaseLoad();
                if (auto res = client.Post("/compile_and_run", compile_json, "application/json;charset=utf-8"))
                {
                    // 将请求得到的结果向上返回
                    if (res->status == 200)
                    {
                        *out_json = res->body;
                        m->DecreaseLoad();
                        LOG(INFO) << "请求编译和运行服务成功" << "\n";
                        break;
                    }
                        
                    m->DecreaseLoad();
                }
                else
                {
                    LOG(ERROR) << "当前请求的" << id << "号主机" << m->ip << ":" << m->port  << "可能已经离线" << "\n";
                    m->DecreaseLoad();
                    // 离线主机
                    load_balance_.OfflineMachine(id);
                    load_balance_.ShowMachines();
                }
            }

        }
    };
}