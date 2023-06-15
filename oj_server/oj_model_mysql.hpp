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
#include "include/mysql.h"

namespace ns_model_mysql
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

    const std::string table_name = "questions";
    const  std::string host = "127.0.0.1";
    const unsigned int port = 3306;
    const std::string user = "oj_client";
    const std::string passwd = "****";
    const std::string db = "oj";


    class Model
    {
    private:
    public:
        Model()
        {}

        bool QueryMySQL(const std::string &sql, std::vector<struct Question> *out)
        {
            MYSQL *sock = mysql_init(nullptr);

            // 连接数据库
            if (mysql_real_connect(sock, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), port, nullptr, 0) == nullptr)
            {
                LOG(FATAL) << "数据库连接失败！" << "\n";
                return false;
            }
            LOG(INFO) << "数据库连接成功" << "\n";

            // 设置本次连接的编码格式
            mysql_set_character_set(sock, "utf8");

            // 执行SQL语句
            if (mysql_query(sock, sql.c_str()) != 0)
            {
                LOG(WARNING) << "SQL语句执行出错" << "\n";
                return false;
            }

            // 提取结果
            MYSQL_RES *res = mysql_store_result(sock);

            // 分析结果
            int rows = mysql_num_rows(res); // 获取行数
            int cols = mysql_num_fields(res); // 获取列数

            for (int i = 0; i < rows; i++)
            {
                MYSQL_ROW row = mysql_fetch_row(res);
                struct Question q;
                q.number = row[0];
                q.title = row[1];
                q.star = row[2];
                q.desc = row[3];
                q.header = row[4];
                q.tail = row[5];
                q.cpu_limit = atoi(row[6]);
                q.mem_limit = atoi(row[7]);
                out->push_back(q);
            }

            // 释放结果空间
            free(res);
            // 关闭数据库连接
            mysql_close(sock);

            return true;
        }
        /*
            获取所有的题目
        */
        bool GetAllQuestions(std::vector<Question> *out)
        {
            std::string sql = "SELECT * FROM ";
            sql += table_name;
            return QueryMySQL(sql, out);
        }

        /*
            获取指定题号的题目
        */
        bool GetOneQuestion(const std::string &number, Question *q)
        {
            std::string sql = "SELECT * FROM ";
            sql += table_name;
            sql += " WHERE number=";
            sql += number;
            std::vector<struct Question> res;
            if (QueryMySQL(sql, &res))
            {
                if (res.size() == 1)
                {
                    *q = res[0];
                    return true;
                }
            }
            return false;
        }

        ~Model() = default;
    };
}
