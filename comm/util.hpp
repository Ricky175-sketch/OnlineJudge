#pragma once

#include <iostream>
#include <string>
#include <atomic>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

namespace ns_util
{
    const std::string temp_path = "./temp/";

    class TimeUtil
    {
    public:
        /*
            获得秒级时间戳
        */ 
        static std::string GetTimeStamp()
        {
            struct timeval _time;
            gettimeofday(&_time, nullptr);
            return std::to_string(_time.tv_sec);
        }

        /*
            获得毫秒级时间戳
        */
        static std::string GetTimeMs()
        {
            struct timeval _time;
            gettimeofday(&_time, nullptr);
            return std::to_string(_time.tv_sec * 1000 + _time.tv_usec / 1000);
        }
    };

    class PathUtil
    {
    public:
        // 添加后缀
        static std::string AddSuffix(const std::string &file_name, const std::string &suffix)
        {
            std::string path = temp_path + file_name;
            path += suffix;
            return path;
        }
        // 构建源文件路径 + 后缀的完整文件名
        static std::string Src(const std::string &file_name)
        {
            return AddSuffix(file_name, ".cpp");
        }
        // 构建可执行程序的完整路径 + 后缀名
        static std::string Exe(const std::string &file_name)
        {
            return AddSuffix(file_name, ".exe");
        }
        // 构建编译后报错信息的完整路径 + 后缀名
        static std::string CompileError(const std::string &file_name)
        {
            return AddSuffix(file_name, ".compile_error");
        }

        // 构建该程序的对应的标准输入完整的路径 + 后缀名
        static std::string Stdin(const std::string &file_name)
        {
            return AddSuffix(file_name, ".stdin");
        }
        // 构建该程序的对应的标准输出完整的路径 + 后缀名
        static std::string Stdout(const std::string &file_name)
        {
            return AddSuffix(file_name, ".stdout");
        }
        // 构建该程序的对应的标准错误完整的路径 + 后缀名
        static std::string Stderr(const std::string &file_name)
        {
            return AddSuffix(file_name, ".stderr");
        }
    };

    class FileUtil
    {
    public:
        /*
            判断文件是否存在
            参数：文件所在的的路径
            返回值：存在-true 不存在-false
        */
        static bool IsFileExists(const std::string &path_name)
        {
            struct stat st;
            if (stat(path_name.c_str(), &st) == 0)
            {
                // 获取文件属性成功，就代表文件存在
                return true;
            }
            return false;
        }

        /*
            生成唯一的文件名，毫秒级时间戳+原子性递增唯一值
        */
        static std::string UniqueFileName()
        {
            static std::atomic_uint id(0);
            id++;

            std::string ms = TimeUtil::GetTimeMs();
            std::string unique_id = std::to_string(id);

            return ms + "_" + unique_id;
        }

        /*
            将内容写入到目标文件中
        */
        static bool WriteFile(const std::string &target, const std::string &content)
        {
            std::ofstream out(target);
            if (!out.is_open())
                return false;
            out.write(content.c_str(), content.size());
            out.close();
            return true;
        }

        /*
            从指定文件中读取内容
        */
        static bool ReadFile(const std::string &target, std::string *content, bool keep_line_break = false)
        {
            (*content).clear();

            std::ifstream in(target);
            if (!in.is_open())
                return false;

            std::string line;
            // 使用getline进行读取时不会保留"\n"
            while (std::getline(in, line))
            {
                (*content) += line;
                (*content) += (keep_line_break ? "\n" : "");
            }
            in.close();
            return true;
        }
    };
}