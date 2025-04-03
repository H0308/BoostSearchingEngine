// #include "DataParse.h"
// #include "SearchIndex.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "SearchEngine.h"

// using namespace data_parse;
// using namespace search_index;
using namespace search_engine;

#pragma region 数据清洗
// int main()
// {
//     DataParse d;
//     d.getHtmlSourceFiles();
//     d.readInfoFromHtml();
//     d.writeToRawFile();

//     return 0;
// }
#pragma endregion

#pragma region boost中的split函数
// int main()
// {
//     std::string str = "aaaaa\3\3\3\3\3bbbbb\3ccccc"; // 原始数据
//     std::vector<std::string> out; // 存储切割后的数据
//     // 以\3作为切割符切割，压缩
//     boost::split(out, str, boost::is_any_of("\3"));

//     std::for_each(out.begin(), out.end(), [&out](std::string s){
//         std::cout << s << std::endl;
//     });

//     return 0;
// }
#pragma endregion

#pragma region 建立索引测试
// int main()
// {
//     SearchIndex si;

//     si.buildIndex();

//     return 0;
// }
#pragma endregion

#pragma region 测试Jieba分词
// #include "include/cppjieba/Jieba.hpp"

// using namespace std;

// int main(int argc, char **argv)
// {
//     // cppjieba::Jieba jieba("./dict/jieba.dict.utf8",
//     //                       "./dict/hmm_model.utf8",
//     //                       "./dict/user.dict.utf8",
//     //                       "./dict/idf.utf8",
//     //                       "./dict/stop_words.utf8");
//     cppjieba::Jieba jieba;
//     vector<string> words;
//     vector<cppjieba::Word> jiebawords;
//     string s;
//     string result;

//     s = "他来到了网易杭研大厦";
//     cout << s << endl;
//     cout << "[demo] Cut With HMM" << endl;
//     jieba.Cut(s, words, true);
//     cout << limonp::Join(words.begin(), words.end(), "/") << endl;

//     cout << "[demo] Cut Without HMM " << endl;
//     jieba.Cut(s, words, false);
//     cout << limonp::Join(words.begin(), words.end(), "/") << endl;

//     s = "我来到北京清华大学";
//     cout << s << endl;
//     cout << "[demo] CutAll" << endl;
//     jieba.CutAll(s, words);
//     cout << limonp::Join(words.begin(), words.end(), "/") << endl;

//     s = "小明硕士毕业于中国科学院计算所，后在日本京都大学深造";
//     cout << s << endl;
//     cout << "[demo] CutForSearch" << endl;
//     jieba.CutForSearch(s, words);
//     cout << limonp::Join(words.begin(), words.end(), "/") << endl;

//     return EXIT_SUCCESS;
// }
#pragma endregion
int main()
{
    SearchEngine se;

    std::string line;
    std::string out;
    while (true)
    {
        std::cout << "请输入关键字：";
        getline(std::cin, line);
        se.search(line, out);
        // ls::LOG(ls::LogLevel::DEBUG) << out;
        std::cout << out << std::endl;
    }

    return 0;
}