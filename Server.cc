#include "SearchEngine.h"
#include "cpp-httplib/httplib.h"
#include "PublicData.h"

namespace pd = public_data;
namespace se = search_engine;

void run(se::SearchEngine& s_engine, const httplib::Request& req, httplib::Response &resp)
{
    // 如果不存在word，说明在请求不存在的页面，返回404
    if(!req.has_param("keyword"))
    {
        se::ls::LOG(se::ls::LogLevel::INFO) << "请求不存在的资源";
        resp.status = httplib::StatusCode::NotFound_404;
        resp.set_file_content("wwwroot/404.html", "text/html");
        return;
    }

    // 此时说明存在对应的值，获取值
    auto val = req.get_param_value("keyword");

    // 执行搜索
    std::string json_string;
    s_engine.search(val, json_string);

    se::ls::LOG(se::ls::LogLevel::INFO) << "搜索关键词: " << val;
    resp.set_content(json_string, "application/json");
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        se::ls::LOG(se::ls::LogLevel::ERROR) << "启动方式错误";
        return 1;
    }

    // 设置网页根路径
    httplib::Server s;
    s.set_base_dir(pd::root_path);
    se::SearchEngine s_engine;

    s.Get("/search", std::bind(run, std::ref(s_engine), std::placeholders::_1, std::placeholders::_2));

    int port = std::stoi(argv[1]);

    s.listen("0.0.0.0", port);

    return 0;
}