# Argparse-cpp

A neat way of creating a command line argument parser for C++ applications.

## Creating a command line parser

Creating a command line parser is as easy as providing a `struct` that contains the values that you can expect from the command line.

This `struct` should then inherit from the base class `Argparser` provided by the included header file `Argparse.hpp`, and then implement the `Parse` method.

Within the `Parse` method you have to describe how the different value should be parsed via the `arg` method.

An example:
```cpp
struct Arguments : public argparse::Argparse {
    bool is_done = false;
    int count = 0;
    double floating_value = 0.2;
    std::string name = "No one";

    void Parse() {
        using namespace argparse;
        arg(is_done, ArgType::TRUE_SWITCH, "d", "done", "Whether the stuff is done or not");
        arg(count, ArgType::KEY_VALUE, "c", "count");
        arg(floating_value, ArgType::KEY_VALUE, "f", "floating-value");
        arg(name, ArgType::KEY_VALUE, "n", "name");
    }
};
```

To run the the parser, simply instanciate the `struct` and then call the `ParseAll` method on the instance to run the parser. Parsed values are then simply accessed as members of the instance.

```cpp
int main(int argc, char **argv) {

    Arguments args;
    args.ParseAll(argc, argv);

    /* do something with the parsed arguments here */

    return 0;
}
```