#include "compiler.hpp"
#include "runner.hpp"

using namespace ns_compiler;
using namespace ns_runner;

int main()
{
    std::string code = "code";
    Compiler::Compile(code);
    Runner::Run(code, 1, 20);

    return 0;
}