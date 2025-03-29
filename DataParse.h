#ifndef bs_DataParse_h
#define bs_DataParse_h

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>

#include "LogSystem.h"

namespace data_parse
{
    namespace fs = std::filesystem;
    namespace ls = log_system;
    // HTML源文件路径
    const fs::path g_datasource_path = "data/source/html";
    // HTML文件后缀
    const std::string g_html_extension = ".html";
    // 用于拼接的官网URL
    const std::string g_url_to_concat = "https://www.boost.org/doc/libs/1_78_0/doc/html";
    // 文本文件路径
    const fs::path g_rawfile_path = "data/raw";
    // 结构体字段间的分隔符
    const std::string g_rd_sep = "\3";
    // 不同HTML文件的分隔符
    const std::string g_html_sep = "\n";

    // 结果基本内容结构
    struct ResultData
    {
        std::string title; // 结构标题
        std::string body;  // 结果内容或描述
        std::string url;   // 网址
    };

    // 内容状态
    enum ContentStatus
    {
        Label,          // 标签状态
        OrdinaryContent // 普通文本状态
    };

    class DataParse
    {
    public:
        // 获取HTML文件路径函数
        bool getHtmlSourceFiles()
        {
            // 如果路径不存在，直接返回false
            if (!fs::exists(g_datasource_path))
            {
                ls::LOG(ls::LogLevel::WARNING) << "不存在指定路径";
                return false;
            }

            // 递归目录结束位置，相当于空指针nullptr
            for (const auto &entry : fs::recursive_directory_iterator(g_datasource_path))
            {
                // 1. 不是普通文件就是目录，继续遍历
                if (!fs::is_regular_file(entry))
                    continue;

                // 2. 是普通文件，但是不是HTML文件，继续遍历
                if (entry.path().extension() != g_html_extension)
                    continue;

                // debug
                // ls::LOG(ls::LogLevel::DEBUG) << entry.path();

                // 3. 是普通文件并且是HTML文件，插入结果
                sources_.push_back(std::move(entry.path()));
            }

            // 空结果返回false
            return !sources_.empty();
        }

        // 读取HTML文件
        bool readHtmlFile(const fs::path &p, std::string &out)
        {
            if (p.empty())
                return false;

            std::fstream f(p);

            if (!f.is_open())
                return false;

            // 读取文件
            std::string buffer;
            while (getline(f, buffer))
                out += buffer;

            f.close();

            return true;
        }

        // 读取标题
        // &为输入型参数，*为输出型参数
        bool getTitleFromHtml(std::string &in, std::string *title)
        {
            if (in.empty())
                return false;

            // 找到开始标签<title>
            auto start = in.find("<title>");
            if (start == std::string::npos)
                return false;

            // 找到终止标签</title>
            auto end = in.find("</title>");
            if (end == std::string::npos || start > end)
                return false;

            // 截取出其中的内容，左闭右开
            *title = in.substr(start + std::string("<title>").size(), end - (start + std::string("<title>").size()));

            return true;
        }

        // 读取HTML文件内容
        bool getContentFromHtml(std::string &out, std::string *body)
        {
            // 默认状态为标签
            ContentStatus cs = ContentStatus::Label;

            // 注意，因为文档没有中文，直接用char没有问题
            for (char ch : out)
            {
                switch (cs)
                {
                // 读取到右尖括号且状态为标签说明接下来为文本内容
                case ContentStatus::Label:
                    if (ch == '>')
                        cs = ContentStatus::OrdinaryContent; // 切换状态
                    break;
                case ContentStatus::OrdinaryContent:
                    // 去除\n
                    if (ch == '<')
                        cs = ContentStatus::Label; // 切换状态
                    else
                    {
                        if (ch == '\n')
                            *body += ' ';
                        else
                            *body += ch;
                    }
                    break;
                default:
                    break;
                }
            }

            return true;
        }

        // 构建URL
        bool constructHtmlUrl(const fs::path &p, std::string *url)
        {
            // 在本地路径中找到"/文件"
            std::string t_path = p.string();
            auto pos = t_path.rfind("/");

            if (pos == std::string::npos)
                return false;

            // 从/开始截取一直到结尾
            std::string source_path = t_path.substr(pos);
            *url = g_url_to_concat + source_path;

            return true;
        }

        // 从HTML文件中读取信息
        bool readInfoFromHtml()
        {
            // 不存在路径返回false
            if (sources_.empty())
            {
                ls::LOG(ls::LogLevel::WARNING) << "文件路径不存在";
                return false;
            }

            for (const auto &path : sources_)
            {
                std::string out; // 存储HTML文件内容
                struct ResultData rd;
                // 1. 读取文件
                if (!readHtmlFile(path, out))
                {
                    ls::LOG(ls::LogLevel::WARNING) << "打开文件：" << path << "失败";
                    // 读取当前文件失败时继续读取后面的文件
                    continue;
                }

                // 2. 获取标题
                if (!getTitleFromHtml(out, &rd.title))
                {
                    ls::LOG(ls::LogLevel::WARNING) << "获取文件标题失败";
                    continue;
                }

                // 3. 读取文件内容
                if (!getContentFromHtml(out, &rd.body))
                {
                    ls::LOG(ls::LogLevel::WARNING) << "获取文件内容失败";
                    continue;
                }

                // 4. 构建URL
                if (!constructHtmlUrl(path, &rd.url))
                {
                    ls::LOG(ls::LogLevel::WARNING) << "构建文件URL失败";
                    continue;
                }

                results_.push_back(std::move(rd));
            }

            // debug
            // ls::LOG(ls::LogLevel::DEBUG) << results_[0].title;
            // ls::LOG(ls::LogLevel::DEBUG) << results_[0].body;
            // ls::LOG(ls::LogLevel::DEBUG) << results_[0].url;

            return true;
        }

        // 将结构体字段写入文本文件中
        bool writeToRawFile()
        {
            // 以二进制形式打开文件
            std::fstream f(g_rawfile_path);

            if (!f.is_open())
            {
                ls::LOG(ls::LogLevel::WARNING) << "文本文件不存在";
                return false;
            }

            // 写入结构化数据
            for (auto &rd : results_)
            {
                std::string temp;
                temp += rd.title;
                temp += g_rd_sep;
                temp += rd.body;
                temp += g_rd_sep;
                temp += rd.url;
                temp += g_html_sep;

                f.write(temp.c_str(), temp.size());
            }

            f.close();

            return true;
        }

    private:
        std::vector<fs::path> sources_;
        std::vector<ResultData> results_;
    };
}

#endif