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
    std::string name = "LAN Play";

    bool lobbies = false;

    int port = 46980;

    config(const char* path)
    {
        // check if config exists
        if (FILE *file = fopen(path, "r")) {
            // close the file so we can open with an ifstream
            fclose(file);

            // load file to string
            std::ifstream t(path);
            std::stringstream buffer;
            buffer << t.rdbuf();

            // parse json
            rapidjson::Document d;
            d.Parse(buffer.str().c_str());

            // instantiate config
            if (d.FindMember("name") != d.MemberEnd()) name = d["name"].GetString();
            if (d.FindMember("lobbies") != d.MemberEnd()) lobbies = d["lobbies"].GetBool();
            if (d.FindMember("port") != d.MemberEnd()) port = d["port"].GetInt();
        }
    }
};