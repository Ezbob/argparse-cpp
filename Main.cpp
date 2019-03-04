#include <iostream>
#include "Argparse.hpp"

struct Arguments : public argparse::Argparse {
    bool isDone = false;
    int count = 0;
    uint64_t extra_count = 0;
    double floating_value = 0.2;
    std::string name = "No one";

    void Parse() {
        using namespace argparse;
        arg(isDone, ArgType::TRUE_SWITCH, "d", "done", "Whether the stuff is done or not");
        arg(extra_count, ArgType::KEY_VALUE, "lc", "long-count", "LONG COUNT!");
        arg(count, ArgType::KEY_VALUE, "c", "count");
        arg(floating_value, ArgType::KEY_VALUE, "f", "floating-value");
        arg(name, ArgType::KEY_VALUE, "n", "name");
    }
};

int main(int argc, char **argv) {

    Arguments args;
    args.ParseArgs(argc, argv);

    std::cout << "Is done " << args.isDone << std::endl;
    std::cout << "float " << args.floating_value << std::endl;
    std::cout << "int " << args.extra_count << std::endl;
    std::cout << "name: " << args.name << std::endl;

    return 0;
}

