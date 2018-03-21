#pragma once

#include <SFML\Network.hpp>
#include <string>
#include <fstream>
#include <streambuf>

#include "TwoStateDotocol.hpp"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

class config
{
public:
    std::string name = "Server";

    int type = 0;

    config(const char* path)
    {
        // load file to string
        std::ifstream t(path);
        std::string str;

        t.seekg(0, std::ios::end);
        str.reserve(t.tellg());
        t.seekg(0, std::ios::beg);

        str.assign(
            (std::istreambuf_iterator<char>(t)),
            std::istreambuf_iterator<char>()
        );

        // parse json
        rapidjson::Document d;
        d.Parse(str.c_str());

        // instantiate config
        if (d.FindMember("name") != d.MemberEnd()) name = d["name"].GetString();
        if (d.FindMember("type") != d.MemberEnd()) type = d["type"].GetInt();
    }
};