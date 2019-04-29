#include <iostream>
#include <string>
#include "dicebot/dicebot.h"

int main(int argc, char const *argv[]) {
    std::string in;

    dicebot::event_info ei(123456);
    ei.nickname = "dynilath";
    dicebot::initialize("./build/test_db/");

    bool flag = true;
    while (flag) {
        std::cout << "> ";
        char buf[1024];
        std::cin.getline(buf, 1024);
        in.assign(buf);
        if (in.size() < 1) continue;
        if (in == "quit" | in == "q") break;
        std::string out;
        clock_t start = clock();
        dicebot::message_pipeline(in, ei, out);
        clock_t end = clock();
        std::cout << " <" << end - start << "> " << out << std::endl;
    }

    dicebot::salvage();
    return 0;
}
