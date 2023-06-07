#pragma once

#include <signal.h>
#include <unistd.h>
#include <signal.h>
#include <jsoncpp/json/json.h>

#include "compiler.hpp"
#include "runner.hpp"
#include "../comm/util.hpp"
#include "../comm/log.hpp"


namespace ns_compile_and_run
{
    // 引入路径拼接功能
    using namespace ns_util;
    // 引入日志功能
    using namespace ns_log;
    // 引入编译功能
    using namespace ns_compiler;
    // 引入运行功能
    using namespace ns_runner;

    class CompileAndRun
    {
    public:
        CompileAndRun() = default;
        ~CompileAndRun() = default;

        /*
            参数：
                in_json：
                    code：用户提交的代码
                    input：用户给自己提交的代码对应的输入，暂不做处理
                    cpu_limit：程序运行时可以使用的CPU时间上限（s）
                    mem_limit：程序运行时最大可以使用的进程地址空间（kb）
                out_json：
                    status：状态码 代码为空(-1) 未知错误(-2) 编译失败(-3)
                    reason：请求结果
                    stdout：程序运行完的结果
                    stderr：程序运行完的错误结果
        */
        static void Start(const std::string &in_json, std::string *out_json)
        {
            Json::Value in_value;
            Json::Reader reader;
            reader.parse(in_json, in_value);

            // 反序列化 取出每一个值
            std::string code = in_value["code"].asString();
            std::string input = in_value["input"].asString();
            int cpu_limit = in_value["cpu_limit"].asInt();
            int mem_limit = in_value["mem_limit"].asInt();

            // 生成唯一的文件名
            std::string file_name;
            // 状态码
            int status_code = 0;
            // 代码运行后的结果
            int run_result;

            Json::Value out_value;

            if (code.size() == 0)
            {
                status_code = -1;
                goto END;
            }

            file_name = FileUtil::UniqueFileName();
            // 形成临时src文件
            if (!FileUtil::WriteFile(PathUtil::Src(file_name), code))
            {
                status_code = -2;
                goto END;
            }

            // 代码编译
            if (!Compiler::Compile(file_name))
            {
                status_code = -3;
                goto END;
            }

            // 代码运行
            run_result = Runner::Run(file_name, cpu_limit, mem_limit);
            if (run_result < 0)
            {
                status_code = -2;
            }
            else if (run_result > 0)
            {
                status_code = run_result;
            }
            else
            {
                // 运行成功
                status_code = 0;
            }

        END:
            out_value["status"] = status_code;
            out_value["reason"] = CodeToSesc(status_code, file_name);
            if (status_code == 0)
            {
                // TODO错误处理
                std::string out_content;
                FileUtil::ReadFile(PathUtil::Stdout(file_name), &out_content, true);
                out_value["stdout"] = out_content;
                
                std::string err_content;
                FileUtil::ReadFile(PathUtil::Stderr(file_name), &err_content, true);
                out_value["stderr"] = err_content;
            }

            Json::StyledWriter writer;
            *out_json = writer.write(out_value);
        }

        /*
            将状态码转换为具体的描述信息
        */
        static std::string CodeToSesc(int code, const std::string file_name)
        {
            std::string desc;
            switch (code)
            {
            case 0:
                desc = "编译运行成功";
                break;
            case -1:
                desc = "提交的代码为空";
                break;
            case -2:
                desc = "未知错误";
                break;
            case -3:
                // TODO错误处理
                FileUtil::ReadFile(PathUtil::CompileError(file_name), &desc, true);
                break;
            case SIGABRT:
                desc = "内存使用超过限制";
                break;
            case SIGXCPU:
                desc = "CPU超时";
                break;
            case SIGFPE:
                desc = "浮点数溢出";
                break;
            // ...可继续补充其它异常信号的处理
            default:
                desc = "未知状态码：" + std::to_string(code);
                break;
            }
            return desc;
        }
    };
}