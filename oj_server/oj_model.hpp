#pragma once

#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <cstdlib>

#include "../comm/log.hpp"
#include "../comm/util.hpp"

namespace ns_model
{
    // 引入路径拼接功能
    using namespace ns_util;
    // 引入日志功能
    using namespace ns_log;

    struct Question
    {
        std::string number; // 题目编号
        std::string title;  // 题目标题
        std::string star;   // 题目难度
        int cpu_limit;      // 题目的时间要求
        int mem_limit;      // 题目的空间要求
        std::string desc;   // 题目的描述
        std::string header; // 题目预设的代码
        std::string tail;   // 题目的测试用例
    };

    const std::string questions_list_file = "./questions/questions.list";
    const std::string question_path = "./questions/";
    class Model
    {
    private:
        std::unordered_map<std::string, Question> quesions; // {题号, 题目详细信息}
    public:
        Model()
        {
            assert(LoadQuestionList(questions_list_file));
        }

        /*
            加载配置文件questions.list到quesions容器中
        */
        bool LoadQuestionList(const std::string &question_list)
        {
            std::ifstream in(questions_list_file);
            if (!in.is_open())
            {
                LOG(FATAL) << "题库文件加载失败，请检查questions.list文件是否存在" << "\n";
                return false;
            }
            
            std::string line;
            while (std::getline(in, line))
            {
                std::vector<std::string> tokens;
                StringUtil::SplitString(line, &tokens, " ");

                if (tokens.size() != 5)
                {
                    LOG(WARNING) << "加载questions.list时部分题目加载失败，请检查格式是否正确" << "\n";
                    continue;
                }
                
                Question q;
                q.number = tokens[0];
                q.title = tokens[1];
                q.star = tokens[2];
                q.cpu_limit = atoi(tokens[3].c_str());
                q.mem_limit = atoi(tokens[4].c_str());

                std::string this_question_path = question_path + q.number + "/";
                FileUtil::ReadFile(this_question_path + "desc.txt", &q.desc, true);
                FileUtil::ReadFile(this_question_path + "header.cpp", &q.header, true);
                FileUtil::ReadFile(this_question_path + "tail.cpp", &q.tail, true);

                 quesions.insert({q.number, q});
            }

            LOG(INFO) << "题库加载成功" << "\n";
            in.close();

            return true;
        }

        /*
            获取所有的题目
        */
        bool GetAllQuestions(std::vector<Question> *out)
        {
            if (quesions.empty())
            {
                LOG(ERROR) << "获取题库失败" << "\n";
                return false;
            }
            
            for (const auto &q : quesions)
                out->push_back(q.second);

            return true;
        }

        /*
            获取指定题号的题目
        */
        bool GetOneQuestion(const std::string &number, Question *q)
        {
            const auto &iter = quesions.find(number);

            if (iter == quesions.end())
            {
                LOG(ERROR) << "获取该题失败" << "\n";
                return false;
            }
            
            (*q) = iter->second;
            return true;
        }

        ~Model() = default;
    };
}