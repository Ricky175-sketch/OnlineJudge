#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <cassert>

#include "../comm/util.hpp"
#include "../comm/log.hpp"
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

    // 提供服务的主机
    class Machine
    {
    private:
        std::string ip;  // 编译服务的ip
        int port;        // 编译服务的port
        uint64_t load;   // 编译服务的负载
        std::mutex *mtx; // 负载锁
    public:
        Machine() : ip(""), port(0), load(0), mtx(nullptr)
        {}
        ~Machine() = default;
        const std::string &getIp() const
        {
            return ip;
        }

        void setIp(const std::string &ip)
        {
            Machine::ip = ip;
        }

        int getPort() const
        {
            return port;
        }

        void setPort(int port)
        {
            Machine::port = port;
        }

        uint64_t getLoad() const
        {
            return load;
        }

        void setLoad(uint64_t load)
        {
            Machine::load = load;
        }

        std::mutex *getMtx() const
        {
            return mtx;
        }

        void setMtx(std::mutex *mtx)
        {
            Machine::mtx = mtx;
        }
    };

    const std::string machine_conf_path = "./conf/service_machine.conf"
    // 负载均衡模块
    class LoadBalance
    {
    private:
        std::vector<Machine> machines; // 可以提供编译服务的机器
        std::vector<int> online;       // 所有在线主机的id（id为每一台主机在machines中的下标）
        std::vector<int> offline;      // 所有离线主机的id
    public:
        LoadBalance()
        {
            assert(LoadConf(machine_conf_path));
        }
        ~LoadBalance() = default;

        bool LoadConf(const std::string &machine_list)
        {

        }
        bool SmartChoice()
        {

        }
        void OfflineMachine()
        {

        }
        void OnlineMachine()
        {
            
        }
    };

    // 核心业务逻辑的控制器
    class Controller
    {
    private:
        Model model_;
        View view_;
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
        void Judge(const std::string in_json, std::string *out_json)
        {
            
        }
    };
}