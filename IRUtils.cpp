//
// Created by gao on 6/29/23.
//

#include "IRUtils.h"

namespace IRCtrl
{
    int IRCtrl::Utils::parseInteger(const std::string &str)
    {
        if (str.empty())
        {
            throw std::invalid_argument("Empty string");
        }

        size_t startPos = 0;
        int base = 10;

        if (str.size() > 2 && (str.substr(0, 2) == "0x" || str.substr(0, 2) == "0X"))
        {
            startPos = 2;
            base = 16;
        } else if (str[0] == '0')
        {
            startPos = 1;
            base = 8;
        }

        try
        {
            size_t pos = 0;
            int num = std::stoi(str.substr(startPos), &pos, base);

            // Check Invalid Chars
            if (pos != str.size() - startPos)
            {
                throw std::invalid_argument("Invalid characters in the string");
            }

            return num;
        } catch (const std::exception &e)
        {
            throw std::invalid_argument("Failed to parse the integer");
        }
    }

    int Utils::tryParseInteger(const std::string &str)
    {
        int a = 0;
        try
        {
            parseInteger(str);
        } catch (const std::invalid_argument &e)
        {
            float f = std::strtof(str.c_str(), nullptr);
            a = (int) f;
        }
        return a;
    }
} // Utils