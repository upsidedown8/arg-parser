#pragma once
#include <string>
#include <vector>
#include <map>
#include <exception>

namespace cpp_arg_parser {

const std::string shortPrefix = "-";
const std::string longPrefix = "--";

struct BoolOption {
    std::string fullName, desc;
    char shortName;
};

template<typename T>
struct TOption : BoolOption {
    bool required;
    T default;
};

class ArgumentParser {
private:
    std::string programName;
    std::vector<BoolOption*> options;
    std::map<std::string, BoolOption*> optionsMap;

public:
    template<class T>
    void addOption(std::string fullName, char shortName, std::string desc, bool required, T default = new T());

    void setProgramName(std::string programName);
};

}