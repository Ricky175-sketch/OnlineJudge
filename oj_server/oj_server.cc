#include <iostream>
#include <signal.h>
#include "../comm/httplib.h"
#include "oj_controller.hpp"

using namespace httplib;
using namespace ns_controller;

static Controller *pcontroller = nullptr;

void Recovry(int signum)
{
    pcontroller->RecoveryMachine();
}

int main()
{
    signal(SIGQUIT, Recovry);

    Server server;
    Controller controller;
    pcontroller = &controller;

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

    server.Post(R"(/judge/(\d+))", [&controller](const Request &req, Response &resp) {
        std::string number = req.matches[1];
        std::string result_json;
        controller.Judge(number, req.body, &result_json);
        resp.set_content(result_json, "application/json; charset=utf-8");
    });

    server.set_base_dir("./wwwroot");

    server.listen("0.0.0.0", 8000);

    return 0;
}