#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "../comm/util.hpp"
#include "../comm/log.hpp"
#include "oj_model.hpp"

namespace ns_controller
{
    // 引入路径拼接功能
    using namespace ns_util;
    // 引入日志功能
    using namespace ns_log;
    // 引入Model模块
    using namespace ns_model;

    class Controller
    {
    private:
        Model model_;
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
                
            }
            else
            {

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

            }
            else
            {

            }
        }

    };
}