#include "DataParse.h"

using namespace data_parse;

int main()
{
    DataParse d;
    d.getHtmlSourceFiles();
    d.readInfoFromHtml();
    d.writeToRawFile();

    return 0;
}