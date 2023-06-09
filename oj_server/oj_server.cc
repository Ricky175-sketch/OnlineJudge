#include <iostream>

#include "../comm/httplib.h"
#include "oj_controller.hpp"

using namespace httplib;
using namespace ns_controller;

int main()
{
    Server server;
    Controller controller;

    server.Get("/all_questions", [&controller](const Request &req, Response &resp) {
        resp.set_content("", "text/plain; charset=utf-8");
    });

    server.Get(R"(/questions/(\d+))", [&controller](const Request &req, Response &resp) {
        std::string number = req.matches[1];
        resp.set_content(number, "text/plain; charset=utf-8");
    });

    server.Get(R"(/judge/(\d+))", [&controller](const Request &req, Response &resp) {
        std::string number = req.matches[1];
        resp.set_content(number, "text/plain; charset=utf-8");
    });

    server.set_base_dir("./wwwroot");

    server.listen("0.0.0.0", 8000);

    return 0;
}