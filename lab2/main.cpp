#include <iostream>
#include <string>
#include <ast.h>
#include <builder.h>
#include <NFA.h>
#include <DFA.h>
#include <memory>
#include <my_regex.h>
#include <gtest/gtest.h>
using namespace regex;

int main(int argc, char* argv[])
{
    regex::Regex regex("(ab)+");
    regex.compile();


    if (regex.match("ababd")) {
        std::cout << "Строка подходит!\n";
    } else {
        std::cout << "Строка не подходит.\n";
    }
    if (regex.search("abab")) {
        std::cout << "Строка подходит!\n";
    } else {
        std::cout << "Строка не подходит.\n";
    }
    regex::Regex reg("a(b|c)*");
    reg.compile();
    std::string recovered = reg.recoverRegex();
    std::cout << "Recovered regex: " << recovered << "\n";

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

    return 0;
}
