#ifndef AUXILIARY_FUNCTIONS_HPP
#define AUXILIARY_FUNCTIONS_HPP

#include <iostream>

#include "json.hpp"
using json = nlohmann::json;
using namespace nlohmann::literals;



/*
    * @brief Returns anchor Id, tag Id and tag position based on the FLAG
    *
    * @return  Returns anchor Id, tag Id and tag position based on the FLAG
    *
    
*/
std::string parseData(std::string str, std::string FLAG);

#endif