#ifndef ARGPARSE_H_adkasdlkasdaooiwe230123fe
#define ARGPARSE_H_adkasdlkasdaooiwe230123fe

#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

namespace argparse {

    enum class ArgType {
        TRUE_SWITCH,
        FALSE_SWITCH,
        KEY_VALUE
    };

    struct ArgInfo {
        ArgType type;
        std::string short_form;
        std::string long_form;
        std::string description;
    };

    struct ArgRaw {
        bool isShort;
        std::string key;
        std::string raw_value;
    };

    class Argparse {
    private:
        std::vector<ArgRaw> m_args_seen;
        std::vector<ArgInfo> m_args_expected;
        std::string epilogue;

        void showHelp(int exit_code);
        bool hasHelp();
        void load(int argc, char **argv);
        bool hasUnknownArg();
        void stopIfDuplicateArg();
        void addArgInfo(ArgType type, const char short_form, const char *long_form, const char *description);

    public:
        void ParseArgs(int argc, char **argv);

        Argparse(const char *epilogue) : epilogue(std::string(epilogue)) {};
        Argparse(std::string epilogue) : epilogue(epilogue) {};
        Argparse() : epilogue("") {};

    protected:
        virtual void Parse() = 0;

        template<typename T>
        void arg(T &argument, ArgType type, const char short_form, const char *long_form, const char *description = nullptr) {

            std::string dashed_short = std::string("") + short_form;
            std::string dashed_long = long_form;

            auto long_entry = std::find_if(m_args_seen.begin(), m_args_seen.end(), [&dashed_long](ArgRaw &raw) {
                return raw.key == dashed_long;
            });
            auto short_entry = std::find_if(m_args_seen.begin(), m_args_seen.end(), [&dashed_short](ArgRaw &raw) {
                return raw.key == dashed_short;
            });;

            if (short_entry != m_args_seen.end() && type == ArgType::KEY_VALUE) {
                std::string raw_value = short_entry->raw_value;
                argument = std::stoull(raw_value);
            } else if (long_entry != m_args_seen.end() && type == ArgType::KEY_VALUE) {
                std::string raw_value = long_entry->raw_value;
                argument = std::stoull(raw_value);
            }

            addArgInfo(type, short_form, long_form, description);
        }

        void arg(bool &arg, ArgType type, const char short_form, const char *long_form, const char *description = nullptr);
        void arg(double &arg, ArgType type, const char short_form, const char *long_form, const char *description = nullptr);
        void arg(float &arg, ArgType type, const char short_form, const char *long_form, const char *description = nullptr);
        void arg(std::string &arg, ArgType type, const char short_form, const char *long_form, const char *description = nullptr);
    };

    /**                                                                                                                           **\
     * =========================================================================================================================== *
     *                                                                                                                             *
     *                                                      Implementation                                                         *
     *                                                                                                                             *
     * =========================================================================================================================== *
    \**                                                                                                                           **/

    void Argparse::load(int argc, char **argv) {
        int last_arg_pos = -1;

        for (int i = 1; i < argc; ++i) {
            bool is_short = false;
            std::string current = argv[i];

            if (current[0] == '-') {

                if (current[1] == '-') { // clipping dashes
                    current = current.substr(2);
                } else {
                    current = current.substr(1);
                    is_short = true;
                }

                // found option
                size_t equal_pos = current.find('=');
                if ( equal_pos != std::string::npos && equal_pos != 0 ) {
                    // parsing key=value arguments
                    std::string lefthandside = current.substr(0, equal_pos);
                    std::string righthandside = current.substr(equal_pos + 1);

                    //auto it = m_args_seen.find(lefthandside);
                    auto it = std::find_if(m_args_seen.begin(), m_args_seen.end(), [&lefthandside](ArgRaw &raw) {
                        return raw.key == lefthandside;
                    });
                    if (it == m_args_seen.end()) {
                        ArgRaw new_arg;
                        new_arg.isShort = is_short;
                        new_arg.key = lefthandside;
                        new_arg.raw_value = righthandside;
                        m_args_seen.push_back(new_arg);
                    }

                } else {
                    // parsing argument maybe followed by some value
                    auto it =  std::find_if(m_args_seen.begin(), m_args_seen.end(), [&current](ArgRaw &raw) {
                        return raw.key == current;
                    });

                    if (it == m_args_seen.end()) {
                        ArgRaw new_arg;
                        new_arg.isShort = is_short;
                        new_arg.key = current;
                        new_arg.raw_value = "";
                        m_args_seen.push_back(new_arg);
                    }

                    last_arg_pos = i;
                }

            } else if (last_arg_pos != -1) {
                // no option but is followed by an option
                std::string last_arg = argv[last_arg_pos];
                if (last_arg[0] == '-' && last_arg[1] == '-') { // clipping dashes
                    last_arg = last_arg.substr(2);
                } else {
                    last_arg = last_arg.substr(1);
                    is_short = true;
                }

                auto it = std::find_if(m_args_seen.begin(), m_args_seen.end(), [&last_arg](ArgRaw &raw) {
                    return raw.key == last_arg;
                });

                if (it != m_args_seen.end()) {
                    it->raw_value = current;
                }

                last_arg_pos = -1;
            }

        }
    }

    void Argparse::showHelp(int exit_code) {
        int width = 42;
        std::cout << "Available arguments are:" << std::endl;
        std::cout << std::setw(width) << std::left << "    -h, --help" << std::right << "Show this help text and exit." << std::endl;
        for ( ArgInfo &info : m_args_expected ) {
            std::string leftside;
            if (info.type == ArgType::KEY_VALUE) {
                leftside = "    -" + info.short_form + ", --" + info.long_form + "=VALUE";
            } else {
                leftside = "    -" + info.short_form + ", --" + info.long_form;
            }
            std::cout << std::setw(width) << std::left << leftside << std::right << info.description << std::endl;
        }

        if (epilogue.length() > 0) {
            std::cout << "\n" << epilogue << std::endl;
        }

        exit(exit_code);
    }

    bool Argparse::hasHelp() {
        auto long_it = std::find_if(m_args_seen.begin(), m_args_seen.end(), [](ArgRaw &raw) {
            return raw.key == "help";
        });
        auto short_it = std::find_if(m_args_seen.begin(), m_args_seen.end(), [](ArgRaw &raw) {
            return raw.key == "h";
        });;

        if (long_it != m_args_seen.end() || short_it != m_args_seen.end()) {
            return true;
        }
        return false;
    }

    void Argparse::addArgInfo(ArgType type, const char short_form, const char *long_form, const char *description) {
        const char *safe_description = description == nullptr ? "" : description;
        std::string dashed_short = std::string("") + short_form;
        std::string dashed_long = long_form;
        m_args_expected.push_back({type, dashed_short, dashed_long, safe_description});
    }

    bool Argparse::hasUnknownArg() {
        for (auto it : m_args_seen) {

            bool notSeen = true;
            for (ArgInfo &a : m_args_expected) {
                if (a.long_form == it.key || a.short_form == it.key) {
                    notSeen = false;
                    break;
                }
            }
            if ( notSeen ) {
                if (it.isShort) {
                    std::cout << "Error: Unknown argument '-" << it.key << "'" << std::endl;
                } else {
                    std::cout << "Error: Unknown argument '--" << it.key << "'" << std::endl;
                }
                return true;
            }
        }
        return false;
    }

    void Argparse::stopIfDuplicateArg() {

        std::vector<ArgInfo> checking_stack(m_args_expected);

        while (!checking_stack.empty()) {
            ArgInfo back = checking_stack.back();
            checking_stack.pop_back();
            for (ArgInfo entry : checking_stack) {
                if (entry.short_form == back.short_form || entry.long_form == back.long_form) {
                    std::cout << "Error: Duplicate entry for '" << back.long_form << "' or '" << entry.short_form << "'" << std::endl;
                    std::exit(2);
                }
            }
        }
    }

    void Argparse::ParseArgs(int argc, char **argv) {
        load(argc, argv);
        Parse();

        stopIfDuplicateArg();

        if ( hasHelp() ) {
            showHelp(0);
        } else if ( hasUnknownArg() ) {
            showHelp(1);
        }
    }

    void Argparse::arg(bool &arg, ArgType type, const char short_form, const char *long_form, const char *description) {
        std::string dashed_short = std::string("") + short_form;
        std::string dashed_long = long_form;

        //auto short_entry = m_args_seen.find(dashed_short);
        //auto long_entry = m_args_seen.find(dashed_long);
        auto long_entry = std::find_if(m_args_seen.begin(), m_args_seen.end(), [&dashed_long](ArgRaw &raw) {
            return raw.key == dashed_long;
        });
        auto short_entry = std::find_if(m_args_seen.begin(), m_args_seen.end(), [&dashed_short](ArgRaw &raw) {
            return raw.key == dashed_short;
        });;

        if (long_entry != m_args_seen.end() || short_entry != m_args_seen.end()) {
            if (type == ArgType::TRUE_SWITCH) {
                arg = true;
            } else {
                arg = false;
            }
        }

        addArgInfo(type, short_form, long_form, description);
    }

    void Argparse::arg(std::string &arg, ArgType type, const char short_form, const char *long_form, const char *description) {
        std::string dashed_short = std::string("") + short_form;
        std::string dashed_long = long_form;

        auto long_entry = std::find_if(m_args_seen.begin(), m_args_seen.end(), [&dashed_long](ArgRaw &raw) {
            return raw.key == dashed_long;
        });
        auto short_entry = std::find_if(m_args_seen.begin(), m_args_seen.end(), [&dashed_short](ArgRaw &raw) {
            return raw.key == dashed_short;
        });

        if (long_entry != m_args_seen.end()) {
            if (type == ArgType::KEY_VALUE) {
                arg = long_entry->raw_value;
            }
        } else if (short_entry != m_args_seen.end()) {
            if (type == ArgType::KEY_VALUE) {
                arg = short_entry->raw_value;
            }
        }

        addArgInfo(type, short_form, long_form, description);
    }

    void Argparse::arg(float &argument, ArgType type, const char short_form, const char *long_form, const char *description) {
        std::string dashed_short = std::string("") + short_form;
        std::string dashed_long = long_form;

        auto long_entry = std::find_if(m_args_seen.begin(), m_args_seen.end(), [&dashed_long](ArgRaw &raw) {
            return raw.key == dashed_long;
        });
        auto short_entry = std::find_if(m_args_seen.begin(), m_args_seen.end(), [&dashed_short](ArgRaw &raw) {
            return raw.key == dashed_short;
        });

        if (long_entry != m_args_seen.end()) {
            if (type == ArgType::KEY_VALUE) {
                std::string raw_value = long_entry->raw_value;
                argument = std::atof(raw_value.c_str());
            }
        } else if (short_entry != m_args_seen.end()) {
            if (type == ArgType::KEY_VALUE) {
                std::string raw_value = short_entry->raw_value;
                argument = std::atof(raw_value.c_str());
            }
        }

        addArgInfo(type, short_form, long_form, description);
    }

    void Argparse::arg(double &argument, ArgType type, const char short_form, const char *long_form, const char *description) {
        std::string dashed_short = std::string("") + short_form;
        std::string dashed_long = long_form;

        auto long_entry = std::find_if(m_args_seen.begin(), m_args_seen.end(), [&dashed_long](ArgRaw &raw) {
            return raw.key == dashed_long;
        });
        auto short_entry = std::find_if(m_args_seen.begin(), m_args_seen.end(), [&dashed_short](ArgRaw &raw) {
            return raw.key == dashed_short;
        });

        if (long_entry != m_args_seen.end()) {
            if (type == ArgType::KEY_VALUE) {
                std::string raw_value = long_entry->raw_value;
                argument = std::atof(raw_value.c_str());
            }
        } else if (short_entry != m_args_seen.end()) {
            if (type == ArgType::KEY_VALUE) {
                std::string raw_value = short_entry->raw_value;
                argument = std::atof(raw_value.c_str());
            }
        }

        addArgInfo(type, short_form, long_form, description);
    }

}



#endif