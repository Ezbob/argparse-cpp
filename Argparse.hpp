#ifndef ARGPARSE_H_adkasdlkasdaooiwe230123fe
#define ARGPARSE_H_adkasdlkasdaooiwe230123fe

#include <string>
#include <map>
#include <memory>
#include <iostream>
#include <vector>

namespace argparse {

    enum class ArgType {
        TRUE_SWITCH,
        FALSE_SWITCH,
        KEY_VALUE
    };

    struct ArgInfo {
        std::string short_form;
        std::string long_form;
        ArgType type;
        std::string description;
    };

    class Argparse {
    private:
        std::map<std::string, std::string> m_args_seen;
        std::vector<ArgInfo> m_arg_infos;

        void showHelp(int exit_code);
        bool hasHelp();
        void load(int argc, char **argv);
        bool hasUnknownArg();
        void stopIfDuplicateArg();
        void addArgInfo(ArgType type, const char *short_form, const char *long_form, const char *description);

    public:
        void ParseAll(int argc, char **argv);

    protected:
        virtual void Parse() = 0;

        template<typename T>
        void arg(T &argument, ArgType type, const char *short_form, const char *long_form, const char *description = nullptr) {

            std::string dashed_short = std::string("-") + short_form;
            std::string dashed_long = std::string("--") + long_form;

            auto short_entry = m_args_seen.find(dashed_short);
            auto long_entry = m_args_seen.find(dashed_long);

            if (long_entry != m_args_seen.end()) {
                if (type == ArgType::KEY_VALUE) {
                    std::string raw_value = long_entry->second;
                    argument = std::stoull(raw_value);
                }
            } else if (short_entry != m_args_seen.end()) {
                if (type == ArgType::KEY_VALUE) {
                    std::string raw_value = short_entry->second;
                    argument = std::stoull(raw_value);
                }
            }

            addArgInfo(type, short_form, long_form, description);
        }

        void arg(bool &arg, ArgType type, const char *short_form, const char *long_form, const char *description = nullptr);
        void arg(double &arg, ArgType type, const char *short_form, const char *long_form, const char *description = nullptr);
        void arg(float &arg, ArgType type, const char *short_form, const char *long_form, const char *description = nullptr);
        void arg(std::string &arg, ArgType type, const char *short_form, const char *long_form, const char *description = nullptr);
    };
}

#endif