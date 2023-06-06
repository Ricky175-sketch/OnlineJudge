#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

namespace ns_util
{
    const std::string temp_path = "./temp/";

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
    };

    class TimeUtil
    {
    public:
        static std::string GetTimeStamp()
        {
            struct timeval _time;
            gettimeofday(&_time, nullptr);
            return std::to_string(_time.tv_sec);
        }
    };
}