#pragma once

#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <unordered_map>

#include "../comm/log.hpp"

namespace ns_model
{
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

        }

        /*
            获取所有的题目
        */
        void GetAllQuestions(std::vector<Question> *out)
        {

        }

        /*
            获取指定题号的题目
        */
        void GetOneQuestion(const std::string &number, Question *q)
        {

        }

        ~Model() = default;
    };
}