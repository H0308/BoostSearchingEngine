#ifndef bs_SearchEngin_h
#define bs_SearchEngin_h

#include "SearchIndex.h"

namespace search_engine
{
    namespace si = search_index;
    class SearchEngine
    {
    public:
        SearchEngine()
        {
            search_index = si::SearchIndex::getSearchIndexInstance();
            // 构建索引
            search_index->buildIndex();
        }

        // 根据关键字进行搜索
        void search(std::string& keyword, std::string& json_string)
        {

        }

        ~SearchEngine()
        {

        }

    private:
        si::SearchIndex* search_index;
    };
}

#endif