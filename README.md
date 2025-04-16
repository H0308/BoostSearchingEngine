# BoostSearchingEngine

## 简介

本项目是基于C++扩展库Boost的搜索引擎项目，主要目的是了解站内搜索基本原理

## 资源准备

1. 后端：C++、[cpp-httplib开源库](https://github.com/yhirose/cpp-httplib)、[Jieba分词](https://github.com/fxsjy/jieba)、[Jsoncpp](https://www.help-doc.top/%E5%85%B6%E5%AE%83/%E5%85%B3%E4%BA%8EJSONCPP%E5%BA%93/%E5%85%B3%E4%BA%8EJSONCPP%E5%BA%93.html#jsoncpp)
2. 前端：HTML、CSS和JavaScript
3. 搜索内容：[1.78版本的Boost库](https://archives.boost.io/release/1.78.0/source/)中的`doc/html`中的内容。也可以到[数据源链接](https://github.com/H0308/Boostv1.78)下载
4. 日志：在Linux下实现的[日志系统](https://www.help-doc.top/Linux/19.%20Linux%E7%BA%BF%E7%A8%8B/6.%20%E6%97%A5%E5%BF%97%E7%B3%BB%E7%BB%9F/6.%20%E6%97%A5%E5%BF%97%E7%B3%BB%E7%BB%9F.html#_1)

> 当前项目仓库中不包含用于搜索的Boost库完整内容

## 使用

```shell
git clone https://github.com/H0308/BoostSearchingEngine.git
cd BoostSearchingEngine
make
./server 自定义端口号
```

> 运行之前需要先检查环境和依赖，对于软链接需要自行配置。需要注意，如果系统是CentOS，可能会因为gcc/g\+\+版本不足导致无法正常编译或者运行，请自行升级gcc/g\+\+

## 文件

- `Parse.cc`与`DataParse.h`：数据清洗模块
- `SearchIndex.h`：构建索引模块
- `SearchEngine.h`：搜索引擎模块
- `Server.cc`：服务端模块

## 运行示例

主页：

![主页](README_Assets/image.png)

搜索：

![搜索](README_Assets/image1.png)

404：

![404](README_Assets/image2.png)

## 项目结构图
```mermaid
graph TD
    Client["Client (Browser)"]:::client

    subgraph "Backend Core"
        HTTPServer["HTTP Server (Server.cc)"]:::backend
        SearchEngine["Search Engine (SearchEngine.h)"]:::backend
        SearchIndex["Index Construction (SearchIndex.h)"]:::backend
        subgraph "Data Cleaning Module"
            Parse["Parse.cc"]:::backend
            DataParse["DataParse.h"]:::backend
        end
        RawData["Raw Data (data)"]:::data
    end

    subgraph "Frontend"
        StaticInterface["Static Web Interface (wwwroot)"]:::frontend
    end

    subgraph "Utilities"
        LogSys["Logging (LogSystem.h)"]:::utility
        Mutex["Mutex (Mutex.h)"]:::utility
    end

    subgraph "External Libraries"
        Boost["Boost"]:::external
        cppHTTPLib["cpp-httplib"]:::external
        Jieba["Jieba"]:::external
        JsonCpp["JsonCpp"]:::external
    end

    %% Main Flow
    Client --> HTTPServer
    HTTPServer -->|"serves"| StaticInterface
    HTTPServer -->|"for_search"| SearchEngine

    RawData --> Parse
    RawData --> DataParse
    Parse --> SearchIndex
    DataParse --> SearchIndex
    SearchIndex --> SearchEngine

    %% External Libraries connections
    cppHTTPLib --> HTTPServer
    Boost --> SearchIndex
    Jieba --> Parse
    JsonCpp --> SearchEngine

    %% Utilities connections (dashed lines)
    HTTPServer --- LogSys
    HTTPServer --- Mutex
    SearchEngine --- LogSys
    SearchEngine --- Mutex
    SearchIndex --- LogSys
    SearchIndex --- Mutex
    Parse --- LogSys
    Parse --- Mutex
    DataParse --- LogSys
    DataParse --- Mutex

    %% Click Events
    click HTTPServer "https://github.com/h0308/boostsearchingengine/blob/main/Server.cc"
    click Parse "https://github.com/h0308/boostsearchingengine/blob/main/Parse.cc"
    click DataParse "https://github.com/h0308/boostsearchingengine/blob/main/DataParse.h"
    click SearchIndex "https://github.com/h0308/boostsearchingengine/blob/main/SearchIndex.h"
    click SearchEngine "https://github.com/h0308/boostsearchingengine/blob/main/SearchEngine.h"
    click LogSys "https://github.com/h0308/boostsearchingengine/blob/main/LogSystem.h"
    click Mutex "https://github.com/h0308/boostsearchingengine/blob/main/Mutex.h"
    click StaticInterface "https://github.com/h0308/boostsearchingengine/tree/main/wwwroot"

    %% Styles
    classDef backend fill:#F5F5DC,stroke:#333,stroke-width:2px;
    classDef frontend fill:#D0E6A5,stroke:#333,stroke-width:2px;
    classDef data fill:#FFCCCB,stroke:#333,stroke-width:2px;
    classDef external fill:#ADD8E6,stroke:#333,stroke-width:2px;
    classDef utility fill:#E6E6FA,stroke:#333,stroke-width:2px;
    classDef client fill:#FFD700,stroke:#333,stroke-width:2px;
```

> 上图由[GitDiagram](https://gitdiagram.com/)提供
