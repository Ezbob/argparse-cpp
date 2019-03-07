#include <iostream>
#include "Argparse.hpp"

struct Arguments : public argparse::Argparse {
    bool isDone = true;
    bool isMoo = false;
    int count = 0;
    uint64_t extra_count = 0;
    double floating_value = 0.2;
    std::string name = "No one";

    Arguments() : Argparse("Epilogue text / Copyright text") {}

    void Parse() {
        using namespace argparse;
        arg(isDone,         ArgType::FALSE_SWITCH,  'd',    "done",              "Whether the stuff is done or not");
        arg(isMoo,          ArgType::TRUE_SWITCH,   'm',    "moo");
        arg(extra_count,    ArgType::KEY_VALUE,     'l',    "long-count",        "LONG COUNT!");
        arg(count,          ArgType::KEY_VALUE,     'c',    "count");
        arg(floating_value, ArgType::KEY_VALUE,     'f',    "floating-value");
        arg(name,           ArgType::KEY_VALUE,     'n',    "name");
    }

    void Validate() {
        constraint([this]() { return isDone != false; }, "\'done\' must not be false");
        constraint([this]() { return extra_count <= 10; }, "\'long_count\' must be less than or equal to 10");
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

