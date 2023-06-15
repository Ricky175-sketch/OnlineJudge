#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <ctemplate/template.h>

#include "oj_model_mysql.hpp"

namespace ns_view
{
    using namespace ns_model_mysql;

    class View
    {
    public:
        View() = default;
        ~View() = default;


        const std::string templates_dir = "./templates/";
        /*
            渲染所有的题目到html
        */
        void ExpandAllQuestions(const std::vector<struct Question> &questions, std::string *html)
        {
            std::string src_html = templates_dir + "all_questions.html";

            // 形成ctemplate数据字典
            ctemplate::TemplateDictionary root("all_questions");
            for (const auto &q : questions)
            {
                ctemplate::TemplateDictionary *sub = root.AddSectionDictionary("question_list");
                sub->SetValue("number", q.number);
                sub->SetValue("title", q.title);
                sub->SetValue("star", q.star);
            }

            // 获取需要被渲染的网页
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(src_html, ctemplate::DO_NOT_STRIP);

            // 完成渲染并获得渲染后的网页
            tpl->Expand(html, &root);
        }

        /*
            渲染一条题目到html
        */
        void ExpandOneQuestion(const struct Question &q, std::string *html)
        {
            std::string src_html = templates_dir + "one_question.html";

            // 形成ctemplate数据字典
            ctemplate::TemplateDictionary root("one_question");
            root.SetValue("number", q.number);
            root.SetValue("title", q.title);
            root.SetValue("star", q.star);
            root.SetValue("desc", q.desc);
            root.SetValue("pre_code", q.header);

            // 获取需要被渲染的网页
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(src_html, ctemplate::DO_NOT_STRIP);

            // 完成渲染并获得渲染后的网页
            tpl->Expand(html, &root);
        }
    };
}