#include "compile_run.hpp"

using namespace ns_compile_and_run;

int main()
{
    std::string in_json;

    Json::Value in_value;
    in_value["code"] = R"(#include <iostream>
int main()
{
    std::cout << "test" << std::endl;
    aaaaaa
    return 0;
})";
    in_value["input"] = "";
    in_value["cpu_limit"] = 1;
    in_value["mem_limit"] = 1024 * 1024 * 2;

    Json::FastWriter writer;
    in_json = writer.write(in_value);

    std::cout << in_json << std::endl;

    std::string out_json;
    CompileAndRun::Start(in_json, &out_json);

    std::cout << out_json << std::endl;

    return 0;
}