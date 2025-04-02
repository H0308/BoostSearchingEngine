#ifndef bs_SearchIndex_h
#define bs_SearchIndex_h

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <string_view>

#include <boost/algorithm/string.hpp>

#include "PublicData.h"
#include "LogSystem.h"

namespace search_index
{
    namespace pd = public_data;
    namespace ls = log_system;

    // 当前筛选出的文档信息
    struct SelectedDocInfo
    {
        struct pd::ResultData rd;
        uint64_t id;
    };

    // 倒排索引时当前关键字的信息
    struct BackwardIndexElement
    {
        uint64_t id;      // 文档ID
        std::string word; // 关键字
        int weight;       // 权重信息
    };

    class SearchIndex
    {
    public:
        // 获取正排索引结果
        SelectedDocInfo *getForwardIndexDocInfo()
        {
        }

        // 获取倒排索引结果
        BackwardIndexElement *getBackwardIndexElement()
        {
        }

        // 构建索引
        bool buildIndex()
        {
            // 以二进制方式读取文本文件中的内容
            std::fstream in(pd::g_rawfile_path, std::ios::in | std::ios::binary);

            if (!in.is_open())
            {
                ls::LOG(ls::LogLevel::WARNING) << "打开文本文件失败";
                return false;
            }

            // 读取每一个ResultData对象
            std::string line;
            while (getline(in, line))
            {
                // 构建正排索引
                struct SelectedDocInfo *s = buildForwardIndex(line);
                
                if (s == nullptr)
                {
                    ls::LOG(ls::LogLevel::WARNING) << "构建正排索引失败";
                    continue;
                }

                // debug
                // ls::LOG(ls::LogLevel::DEBUG) << s->rd.title;
                // ls::LOG(ls::LogLevel::DEBUG) << s->rd.body;
                // ls::LOG(ls::LogLevel::DEBUG) << s->rd.url;
                // ls::LOG(ls::LogLevel::DEBUG) << s->id;
                // break;

                // 构建倒排索引
                bool flag = buildBackwardIndex(*s);
            }

            return true;
        }

        SelectedDocInfo *buildForwardIndex(std::string &line)
        {
            std::vector<std::string> out;
            split(out, line, pd::g_rd_sep);

            if (out.size() != 3)
            {
                ls::LOG(ls::LogLevel::WARNING) << "无法读取元信息";
                return nullptr;
            }

            SelectedDocInfo sd;
            // 注意填充顺序
            sd.rd.title = out[0];
            sd.rd.body = out[1];
            sd.rd.url = out[2];
            // 先设置id时直接使用正排索引数组长度
            sd.id = forward_index_.size();
            // 再添加SelectedDocInfo对象
            forward_index_.push_back(std::move(sd));

            return &forward_index_.back();
        }

        // 构建倒排索引
        bool buildBackwardIndex(SelectedDocInfo &sd)
        {
        }

    private:
#if 0
        // boost中的split
        void split(std::vector<std::string> &out, std::string &line, std::string sep)
        {
            boost::split(out, line, boost::is_any_of("\3"), boost::token_compress_on);
        }
#endif

        // 使用string_view自主实现split
        void split(std::vector<std::string> &out, std::string_view line, std::string_view sep)
        {
            if (line.empty())
                return;

            size_t pos = 0;
            size_t found;

            while ((found = line.find(sep, pos)) != std::string_view::npos)
            {
                // 添加当前位置到分隔符之间的子字符串
                out.push_back(std::string(line.substr(pos, found - pos)));
                // 更新位置到分隔符之后
                pos = found + sep.length();
            }

            // 添加最后一个分隔符之后的子字符串
            if (pos < line.length())
                out.push_back(std::string(line.substr(pos)));
        }

    private:
        std::vector<SelectedDocInfo> forward_index_;                                         // 正排索引结果
        std::unordered_map<std::string, std::vector<BackwardIndexElement>> _backward_index_; // 倒排索引结果
    };
}

#endif