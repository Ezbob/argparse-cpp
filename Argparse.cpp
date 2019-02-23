#include "Argparse.hpp"
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <stack>
#include <iomanip>

using namespace argparse;

void Argparse::load(int argc, char **argv) {
    const char *last_option = nullptr;

    for (int i = 1; i < argc; ++i) {
        std::string current = argv[i];
        if (current.length() > 0) {

            if (current[0] == '-') {
                // found option
                size_t equal_pos = current.find('=');
                if ( equal_pos != std::string::npos && equal_pos != 0 ) {
                    std::string lefthandside = current.substr(0, equal_pos);
                    std::string righthandside = current.substr(equal_pos + 1);

                    auto it = m_args_seen.find(lefthandside);
                    if (it == m_args_seen.end()) {
                        m_args_seen[lefthandside] = righthandside;
                    }

                } else {
                    auto it = m_args_seen.find(current);

                    if (it == m_args_seen.end()) {
                        m_args_seen[current] = "";
                    }

                    last_option = current.c_str();
                }

            } else if (last_option != nullptr) {
                // no option but is followed by an option
                auto it = m_args_seen.find(last_option);

                if (it != m_args_seen.end()) {
                    m_args_seen[std::string(last_option)] = current;
                }

                last_option = nullptr;
            }
        }
    }
}

void Argparse::showHelp(int exit_code) {
    int width = 42;
    std::cout << "Available arguments are:" << std::endl;
    std::cout << std::setw(width) << std::left << "    -h, --help" << std::right << "Show this help text and exit." << std::endl;
    for ( ArgInfo &info : m_arg_infos ) {
        std::string leftside;
        if (info.type == ArgType::KEY_VALUE) {
            leftside = "    " + info.short_form + ", " + info.long_form + "=VALUE";
        } else {
            leftside = "    " + info.short_form + ", " + info.long_form;
        }
        std::cout << std::setw(width) << std::left << leftside << std::right << info.description << std::endl;
    }
    exit(exit_code);
}

bool Argparse::hasHelp() {
    auto long_it = m_args_seen.find("--help");
    auto short_it = m_args_seen.find("-h");

    if (long_it != m_args_seen.end() || short_it != m_args_seen.end()) {
        return true;
    }
    return false;
}

void Argparse::addArgInfo(ArgType type, const char *short_form, const char *long_form, const char *description) {
    const char *safe_description = description == nullptr ? "" : description;
    std::string dashed_short = std::string("-") + short_form;
    std::string dashed_long = std::string("--") + long_form;
    m_arg_infos.push_back({dashed_short, dashed_long, type, safe_description});
}

bool Argparse::hasUnknownArg() {
    for (auto it : m_args_seen) {
        std::string arg = it.first;

        bool notSeen = true;
        for (ArgInfo &a : m_arg_infos) {
            if (a.long_form == arg || a.short_form == arg) {
                notSeen = false;
                break;
            }
        }
        if ( notSeen ) {
            std::cout << "Error: Unknown argument '" << arg << "'" << std::endl;
            return true;
        }
    }
    return false;
}

void Argparse::stopIfDuplicateArg() {

    std::vector<ArgInfo> checking_stack(m_arg_infos);

    while (!checking_stack.empty()) {
        ArgInfo back = checking_stack.back();
        checking_stack.pop_back();
        for (ArgInfo entry : checking_stack) {
            if (entry.short_form == back.short_form || entry.long_form == back.long_form) {
                std::cout << "Error: Duplicate entry for '" + back.long_form + "' or '" + entry.short_form + "'" << std::endl;
                std::exit(2);
            }
        }
    }
}

void Argparse::ParseAll(int argc, char **argv) {
    load(argc, argv);
    Parse();

    stopIfDuplicateArg();

    if ( hasHelp() ) {
        showHelp(0);
    } else if ( hasUnknownArg() ) {
        showHelp(1);
    }
}

void Argparse::arg(bool &arg, ArgType type, const char *short_form, const char *long_form, const char *description) {

    std::string dashed_short = std::string("-") + short_form;
    std::string dashed_long = std::string("--") + long_form;

    auto short_entry = m_args_seen.find(dashed_short);
    auto long_entry = m_args_seen.find(dashed_long);

    if (long_entry != m_args_seen.end() || short_entry != m_args_seen.end()) {
        if (type == ArgType::TRUE_SWITCH) {
            arg = true;
        } else {
            arg = false;
        }
    }

    addArgInfo(type, short_form, long_form, description);
}

void Argparse::arg(std::string &arg, ArgType type, const char *short_form, const char *long_form, const char *description) {

    std::string dashed_short = std::string("-") + short_form;
    std::string dashed_long = std::string("--") + long_form;

    auto short_entry = m_args_seen.find(dashed_short);
    auto long_entry = m_args_seen.find(dashed_long);

    if (long_entry != m_args_seen.end()) {
        if (type == ArgType::KEY_VALUE) {
            arg = long_entry->second;
        }
    } else if (short_entry != m_args_seen.end()) {
        if (type == ArgType::KEY_VALUE) {
            arg = short_entry->second;
        }
    }

    addArgInfo(type, short_form, long_form, description);
}

void Argparse::arg(float &argument, ArgType type, const char *short_form, const char *long_form, const char *description) {
    std::string dashed_short = std::string("-") + short_form;
    std::string dashed_long = std::string("--") + long_form;

    auto short_entry = m_args_seen.find(dashed_short);
    auto long_entry = m_args_seen.find(dashed_long);

    if (long_entry != m_args_seen.end()) {
        if (type == ArgType::KEY_VALUE) {
            std::string raw_value = long_entry->second;
            argument = std::atof(raw_value.c_str());
        }
    } else if (short_entry != m_args_seen.end()) {
        if (type == ArgType::KEY_VALUE) {
            std::string raw_value = short_entry->second;
            argument = std::atof(raw_value.c_str());
        }
    }

    addArgInfo(type, short_form, long_form, description);
}

void Argparse::arg(double &argument, ArgType type, const char *short_form, const char *long_form, const char *description) {
    std::string dashed_short = std::string("-") + short_form;
    std::string dashed_long = std::string("--") + long_form;

    auto short_entry = m_args_seen.find(dashed_short);
    auto long_entry = m_args_seen.find(dashed_long);

    if (long_entry != m_args_seen.end()) {
        if (type == ArgType::KEY_VALUE) {
            std::string raw_value = long_entry->second;
            argument = std::atof(raw_value.c_str());
        }
    } else if (short_entry != m_args_seen.end()) {
        if (type == ArgType::KEY_VALUE) {
            std::string raw_value = short_entry->second;
            argument = std::atof(raw_value.c_str());
        }
    }

    addArgInfo(type, short_form, long_form, description);
}
