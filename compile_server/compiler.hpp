// 只负责进行代码的编译

#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../comm/util.hpp"
#include "../comm/log.hpp"

namespace ns_compiler
{
    // 引入路径拼接功能
    using namespace ns_util;
    // 引入日志功能
    using namespace ns_log;

    class Compiler
    {
    public:
        Compiler()
        {
        }
        ~Compiler()
        {
        }
        /*
            输入参数：编译的文件名，不需要带文件后缀
            返回值：编译成功-true，编译失败-false
        */
        static bool Compile(const std::string &file_name)
        {
            pid_t pid = fork();
            if (pid < 0)
            {
                LOG(ERROR) << "内部错误，创建子进程失败" << "\n";
                return false;
            }
            else if (pid == 0) // 子进程
            {
                // 防止平台的不同对生成文件的权限产生影响
                umask(0);
                int _stderr_fd = open(PathUtil::CompileError(file_name).c_str(), O_CREAT | O_WRONLY, 0644);
                if (_stderr_fd < 0) {
                    LOG(WARNING) << "stderr文件生成失败" << "\n";
                    exit(1);
                }
                // 重定向标准错误到_stderr_fd
                dup2(_stderr_fd, 2);

                // 子进程：调用编译器，完成对代码的编译
                execlp("g++", "g++", "-o", PathUtil::Exe(file_name).c_str(), PathUtil::Src(file_name).c_str(), "-std=c++11", nullptr/*参数传递完毕必须以null作为结束*/);

                LOG(ERROR) << "g++启动失败，可能是参数错误" << "\n";

                exit(2);
            }
            else
            {
                waitpid(pid, nullptr, 0);

                // 判断编译是否成功，就看是否形成对应的可执行程序
                if (FileUtil::IsFileExists(PathUtil::Exe(file_name)))
                {
                    LOG(INFO) << PathUtil::Src(file_name) << " 编译成功" << "\n";
                    return true;
                }
                 
            }

            LOG(DEBUG) << PathUtil::Src(file_name) << "\n";
            LOG(ERROR) << "编译失败，没有形成可执行程序" << "\n";

            return false;
        }
    };
}