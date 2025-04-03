#ifndef bs_SearchEngin_h
#define bs_SearchEngin_h

#include <algorithm>

#include "SearchIndex.h"
#include "include/cppjieba/Jieba.hpp"
#include "jsoncpp/json/json.h"
#include "LogSystem.h"

namespace search_engine
{
    namespace si = search_index;
    namespace ls = log_system;
    class SearchEngine
    {
    public:
        SearchEngine()
            : search_index_(si::SearchIndex::getSearchIndexInstance())
        {
            // 构建索引
            search_index_->buildIndex();
        }

        // 根据关键字进行搜索
        void search(std::string &keyword, std::string &json_string)
        {
            // 对用户输入的关键字进行切分

            std::vector<std::string> keywords;
            jieba_.CutForSearch(keyword, keywords);

            // 查询哈希表得到结果
            std::vector<si::BackwardIndexElement> results;
            std::unordered_map<uint64_t, si::BackwardIndexElement> select_map;
            for (auto &word : keywords)
            {
                // 忽略大小写
                boost::to_lower(word);
                // 查倒排索引
                std::vector<si::BackwardIndexElement> *ret_ptr = search_index_->getBackwardIndexElement(word);
                if (!ret_ptr)
                    continue;
                // 插入结果
                // results.insert(results.end(), ret_ptr->begin(), ret_ptr->end());
                for (auto &bi : *ret_ptr)
                {
                    // 如果文档ID已经存在，说明已经存在，否则不存在
                    if (select_map.find(bi.id) == select_map.end())
                        select_map[bi.id] = bi;
                }
            }

            // 遍历select_map存储结果
            for (auto &pair : select_map)
                results.push_back(std::move(pair.second));

            // 按照权重排序
            std::stable_sort(results.begin(), results.end(), [](const si::BackwardIndexElement &b1, const si::BackwardIndexElement &b2)
                             { return b1.weight > b2.weight; });

            // 转换为JSON字符串
            Json::Value root;
            for (auto &bi : results)
            {
                // 通过正排索引获取文章内容
                si::SelectedDocInfo *sd = search_index_->getForwardIndexDocInfo(bi.id);

                Json::Value item;
                // debug
                // item["id"] = sd->id;
                // item["word"] = bi.word;
                // item["weight"] = bi.weight;

                item["title"] = sd->rd.title;
                item["body"] = getPartialBodyWithKeyword(sd->rd.body, bi.word);
                item["url"] = sd->rd.url;

                // 将item作为一个JSON对象插入到root中作为子JSON对象
                root.append(item);
            }

            json_string = root.toStyledString();
        }

        ~SearchEngine()
        {
        }

    private:
        static const int prev_words = 50;
        static const int after_words = 100;
        std::string getPartialBodyWithKeyword(std::string_view body, std::string_view keyword)
        {
            // 找到关键字
            size_t pos = body.find(keyword);
            if (pos == std::string_view::npos)
            {
                ls::LOG(ls::LogLevel::WARNING) << "无法找到关键字，无法截取文章内容";
                // std::cout << "无法找到关键字，无法截取文章内容" << std::endl;
                return "Fail to cut body, can't find keyword";
            }

            // 默认起始位置为0，终止位置为body字符串最后一个字符
            int start = 0;
            int end = static_cast<int>(body.size() - 1);

            // 如果pos位置前有50个字符，就取前50个字符
            if (pos - prev_words > start)
                start = pos - prev_words;
            // 如果pos位置后有100个字符，就取后100个字符
            if (pos + static_cast<int>(keyword.size()) + after_words < end)
                end = pos + static_cast<int>(keyword.size()) + after_words;

            if (start > pos)
            {
                ls::LOG(ls::LogLevel::WARNING) << "内容不足，无法截取文章内容";
                // std::cout << "内容不足，无法截取文章内容" << std::endl;
                return "Fail to cut body, body is not enough";
            }

            // 左闭右闭区间
            return std::string(body.substr(start, end - start + 1));
        }

    private:
        si::SearchIndex *search_index_;
        cppjieba::Jieba jieba_;
    };
}

#endif