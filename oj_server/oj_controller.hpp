#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <fstream>
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
        std::mutex *mtx; // 负载锁（这里使用指针，因为mutex是无法被复制的）
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
            uint64_t ret = -1;
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
                m.setIp(tokens[0]);
                m.setPort(atoi(tokens[1].c_str()));
                m.setLoad(0);
                m.setMtx(new std::mutex());

                online.push_back(machines.size());
                machines.push_back(m);
            }

            in.close();
            return true;
        }
        bool SmartChoice(int *id, Machine *m)
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
            uint64_t min_load = machines[online[0]].getLoad();
            for (int i = 0; i < online.size(); i++)
            {
                uint64_t cur_load = machines[online[i]].getLoad();
                if (cur_load < min_load)
                {
                    min_load = cur_load;
                    *id = online[i];
                    *m = machines[online[i]];
                }
            }
            mtx.unlock();
            return true;
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
        void Judge(const std::string in_json, std::string *out_json)
        {
            
        }
    };
}