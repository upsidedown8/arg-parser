#include "cpp-arg-parser/cpp-arg-parser.hpp"

using namespace cpp_arg_parser;

template<class T>
void ArgumentParser::addOption(std::string fullName, char shortName, std::string desc, bool required, T default) {
    if (optionsMap.count(fullName)) throw std::exception("multiple definition: "+fullName);
    optionsMap[fullName]=new option_with_value_with_reader<T, F>(name, short_name, need, def, desc, reader);
    ordered.push_back(options[fullName]);
}

void ArgumentParser::setProgramName(std::string programName) {
    this->programName = programName;
}