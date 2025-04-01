#include "DataParse.h"

#include <boost/algorithm/string.hpp>

using namespace data_parse;

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

int main()
{
    std::string str = "aaaaa\3\3\3\3\3bbbbb\3ccccc"; // 原始数据
    std::vector<std::string> out; // 存储切割后的数据
    // 以\3作为切割符切割，压缩
    boost::split(out, str, boost::is_any_of("\3"));

    std::for_each(out.begin(), out.end(), [&out](std::string s){
        std::cout << s << std::endl;
    });
    
    return 0;
}