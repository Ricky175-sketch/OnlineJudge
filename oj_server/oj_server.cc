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
        std::string html;
        controller.AllQuestions(&html);
        resp.set_content(html, "text/html; charset=utf-8");
    });

    server.Get(R"(/question/(\d+))", [&controller](const Request &req, Response &resp) {
        std::string html;
        std::string number = req.matches[1];
        controller.Question(number, &html);
        resp.set_content(html, "text/html; charset=utf-8");
    });

    server.Get(R"(/judge/(\d+))", [&controller](const Request &req, Response &resp) {
        std::string number = req.matches[1];
        resp.set_content(number, "text/plain; charset=utf-8");
    });

    server.set_base_dir("./wwwroot");

    server.listen("0.0.0.0", 8000);

    return 0;
}