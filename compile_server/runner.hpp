#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../comm/log.hpp"
#include "../comm/util.hpp"

namespace ns_runner
{
    // 引入日志功能
    using namespace ns_log;
    // 引入路径拼接功能
    using namespace ns_util;

    class Runner
    {
    public:
        Runner()
        {}
        ~Runner()
        {}
        /*
            输入参数：运行的文件名，不需要带文件后缀
            返回值：打开文件失败（-1）创建子进程失败（-2）程序出现异常（>0，返回值为退出时收到的信号编号）程序正常运行完毕（0）
        */
        static int Run(const std::string &file_name)
        {
            std::string _execute = PathUtil::Exe(file_name);
            std::string _stdin = PathUtil::Stdin(file_name);
            std::string _stdout = PathUtil::Stdout(file_name);
            std::string _stderr = PathUtil::Stderr(file_name);

            // 防止平台的不同对生成文件的权限产生影响
            umask(0);
            int _stdin_fd = open(_stdin.c_str(), O_CREAT | O_WRONLY, 0644);
            int _stdoutr_fd = open(_stdout.c_str(), O_CREAT | O_WRONLY, 0644);
            int _stderr_fd = open(_stderr.c_str(), O_CREAT | O_WRONLY, 0644);

            if (_stdin_fd < 0 || _stdoutr_fd < 0 || _stderr_fd < 0)
            {
                LOG(ERROR) << "打开文件失败" << "\n";
                return -1;
            }

            pid_t pid = fork();
            if (pid < 0)
            {
                LOG(ERROR) << "创建子进程失败" << "\n";
                close(_stdin_fd);
                close(_stdoutr_fd);
                close(_stderr_fd);
                LOG(INFO) << "文件描述符已关闭" << "\n";
                return -2;
            }
            else if (pid == 0) // 子进程
            {
                dup2(_stdin_fd, 0);
                dup2(_stdoutr_fd, 1);
                dup2(_stderr_fd, 2);

                execl(_execute.c_str(), _execute.c_str(), nullptr);

                exit(1);
            }
            else
            {
                close(_stdin_fd);
                close(_stdoutr_fd);
                close(_stderr_fd);
                
                int status = 0;
                waitpid(pid, &status, 0);
                // 获取到子进程的退出状态码并返回
                return status & 0x7F;
            }
        }
    };
}