#pragma once

#include <iostream>
#include <SFML\Network.hpp>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "TwoStateDotocol.hpp"

namespace authkit {
    sf::Http http;
    void check(char* data, sf::IpAddress& sender, unsigned short& port, sf::UdpSocket& socket)
    {
        // extract id and token from data
        char id[37];
        char token[37];

        memcpy(id, &data[1], 36);
        memcpy(token, &data[37], 36);

        id[36] = '\0';
        token[36] = '\0';

        // build path
        std::string path("/api/auth/");
        path += id;
        path += "/";
        path += token;

        // make request
        http.setHost("http://dotto.mcfish.space");
        sf::Http::Request request;
        request.setMethod(sf::Http::Request::Post);
        request.setUri(path.c_str());
        request.setHttpVersion(1, 1); // HTTP 1.1

        sf::Http::Response response = http.sendRequest(request);

        rapidjson::Document resp;
        resp.Parse(response.getBody().c_str());
        std::string status = resp["s"].GetString();

        if (status == "ok")
        {
            tsd::construct(id);
            std::string msg = "-ok";
            const char* buff = msg.c_str();
            socket.send(buff, msg.size(), sender, port);
        }
        else
        {
            std::string msg = "-foul";
            const char* buff = msg.c_str();
            socket.send(buff, msg.size(), sender, port);
        }
    }
}