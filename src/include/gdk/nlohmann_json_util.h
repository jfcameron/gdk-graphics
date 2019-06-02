// © 2018 Joseph Cameron - All Rights Reserved

#ifndef NLOHMANN_JSON_UTIL_H
#define NLOHMANN_JSON_UTIL_H

#include <nlohmann/json.hpp>

#include <sstream>

namespace jfc
{
    template<typename Arg>
    nlohmann::json insertion_operator_to_nlohmann_json_object(Arg &&arg)
    {
        std::ostringstream stringStream;
        
        stringStream << arg;

        return nlohmann::json::parse(stringStream.str());
    }
}

#endif
