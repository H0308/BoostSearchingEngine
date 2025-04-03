#ifndef bs_SearchIndex_h
#define bs_SearchIndex_h

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <string_view>
#include <mutex>
#include <boost/algorithm/string.hpp>

#include "PublicData.h"
#include "LogSystem.h"
#include "include/cppjieba/Jieba.hpp" // 引入Jieba分词

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

    // 频率结构
    struct WordCount
    {
        int title_cnt;
        int body_cnt;
    };

    class SearchIndex
    {
    private:
        static const int title_weight_per = 10;
        static const int body_weight_per = 1;

        // 私有构造函数
        SearchIndex()
        {
        }

        // 禁用拷贝和赋值
        SearchIndex(const SearchIndex &si) = delete;
        SearchIndex operator=(SearchIndex &si) = delete;
        SearchIndex(SearchIndex &&si) = delete;

        static SearchIndex *si;

    public:
        // 获取单例对象
        static SearchIndex *getSearchIndexInstance()
        {
            if (!si)
            {
                // 加锁
                mtx.lock();
                if (!si)
                    si = new SearchIndex();
                mtx.unlock();
            }

            return si;
        }

        // 获取正排索引结果
        SelectedDocInfo *getForwardIndexDocInfo(uint64_t id)
        {
            if (id < 0 || id > forward_index_.size())
            {
                ls::LOG(ls::LogLevel::WARNING) << "不存在对应的文档ID";
                // std::cout << "不存在对应的文档ID" << std::endl;
                return nullptr;
            }

            return &forward_index_[id];
        }

        // 获取倒排索引结果
        std::vector<BackwardIndexElement> *getBackwardIndexElement(const std::string &keyword)
        {
            auto pos = backward_index_.find(keyword);
            if (pos == backward_index_.end())
            {
                ls::LOG(ls::LogLevel::WARNING) << "不存在对应的关键字";
                // std::cout << "不存在对应的关键字" << std::endl;
                return nullptr;
            }

            return &backward_index_[keyword];
        }

        // 构建索引
        bool buildIndex()
        {
            // debug
            ls::LOG(ls::LogLevel::DEBUG) << "开始建立索引";
            // std::cout << "开始建立索引" << std::endl;
            // 以二进制方式读取文本文件中的内容
            std::fstream in(pd::g_rawfile_path, std::ios::in | std::ios::binary);

            if (!in.is_open())
            {
                ls::LOG(ls::LogLevel::WARNING) << "打开文本文件失败";
                // std::cout << "打开文本文件失败" << std::endl;
                return false;
            }

            // 读取每一个ResultData对象
            std::string line;
            int count = 0;
            while (getline(in, line))
            {
                // 构建正排索引
                struct SelectedDocInfo *s = buildForwardIndex(line);

                if (!s)
                {
                    ls::LOG(ls::LogLevel::WARNING) << "构建正排索引失败";
                    // std::cout << "构建正排索引失败" << std::endl;
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

                if (!flag)
                {
                    ls::LOG(ls::LogLevel::WARNING) << "构建倒排索引失败";
                    // std::cout << "构建倒排索引失败" << std::endl;
                    continue;
                }

                count++;
                if (count % 50 == 0)
                {
                    ls::LOG(ls::LogLevel::DEBUG) << "已经建立：" << count;
                    // std::cout << "已经建立：" << count << std::endl;
                }
            }
            ls::LOG(ls::LogLevel::DEBUG) << "建立索引完成";
            // std::cout << "建立索引完成" << std::endl;

            return true;
        }

    private:
        SelectedDocInfo *buildForwardIndex(std::string &line)
        {
            // debug
            // ls::LOG(ls::LogLevel::DEBUG) << "开始建立正排索引";

            std::vector<std::string> out_string;

            split(out_string, line, pd::g_rd_sep);
            // for(auto & s : out_string)
            //     ls::LOG(ls::LogLevel::DEBUG) << s;

            if (out_string.size() != 3)
            {
                ls::LOG(ls::LogLevel::WARNING) << "无法读取元信息";
                // std::cout << "无法读取元信息" << std::endl;
                return nullptr;
            }

            SelectedDocInfo sd;
            // 注意填充顺序
            sd.rd.title = out_string[0];
            sd.rd.body = out_string[1];
            sd.rd.url = out_string[2];
            // 先设置id时直接使用正排索引数组长度
            sd.id = forward_index_.size();
            // 再添加SelectedDocInfo对象
            forward_index_.push_back(std::move(sd));

            // debug
            // ls::LOG(ls::LogLevel::DEBUG) << "建立正排索引结束";

            return &forward_index_.back();
        }

        // 构建倒排索引
        bool buildBackwardIndex(SelectedDocInfo &sd)
        {
            word_cnt_.clear();
            // debug
            // ls::LOG(ls::LogLevel::DEBUG) << "开始建立倒排索引";

            // 统计标题中关键字出现的次数

            std::vector<std::string> title_words;
            jieba_.CutForSearch(sd.rd.title, title_words);
            for (auto &tw : title_words)
            {
                // 忽略大小写
                boost::to_lower(tw);
                word_cnt_[tw].title_cnt++;
            }

            // 统计内容中关键字出现的次数
            std::vector<std::string> body_words;
            jieba_.CutForSearch(sd.rd.body, body_words);
            for (auto &bw : body_words)
            {
                boost::to_lower(bw);
                word_cnt_[bw].body_cnt++;
            }

            // 遍历关键字哈希表获取关键字填充对应的倒排索引节点
            for (auto &word : word_cnt_)
            {
                BackwardIndexElement b;
                b.id = sd.id;
                b.word = word.first;
                // 权重统计按照公式计算
                b.weight = word.second.title_cnt * title_weight_per + word.second.body_cnt * body_weight_per;

                backward_index_[b.word].push_back(std::move(b));
            }

            // debug
            // ls::LOG(ls::LogLevel::DEBUG) << "建立倒排索引结束";

            return true;
        }
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
        std::vector<SelectedDocInfo> forward_index_;                                        // 正排索引结果
        std::unordered_map<std::string, std::vector<BackwardIndexElement>> backward_index_; // 倒排索引结果
        std::unordered_map<std::string, WordCount> word_cnt_;                               // 词频统计
        static std::mutex mtx;
        cppjieba::Jieba jieba_;
    };

    SearchIndex *SearchIndex::si = nullptr;
    std::mutex SearchIndex::mtx;
}

#endif