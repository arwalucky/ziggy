#include "auxiliary_functions.hpp"


std::string parseData(std::string str, std::string FLAG)
{
    std::string s = str;
    std::string delimiter = "||";

    int counter = 0;
    std::string str1;

    if (FLAG == "TIMESTAMP")
    {
        counter = 3;
    }
    else if(FLAG == "DISTANCE")
    {
        counter = 2;
    }
    else if(FLAG == "TAGID")
    {
        counter = 1;
    }
    else if(FLAG == "ANCHORID")  
    {
        counter = 0;
    }
    else
    {
        std::cout << "FLAG not found" << std::endl;
        return 0;
    }

    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos)
    {
        if (counter == 0)
        {
            str1 = s.substr(0, pos);
            s.erase(0, pos + delimiter.length());
        }
         s.erase(0, pos + delimiter.length());
         counter--;
    }

    return str1;
}
