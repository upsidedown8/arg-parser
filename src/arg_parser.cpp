#include "arg_parser/arg_parser.hpp"

using namespace arg_parser;

/* -------------------------------------------------------------------------- */
/*                                TestCriteria                                */
/* -------------------------------------------------------------------------- */
void TestCriteriaBase::error(const std::string &optionName, const std::string& msg) {
    arg_parser::error(optionName + ": " + msg);
}

TypeTestCriteria::TypeTestCriteria(const std::string &optionName, TestTypes type) {
    this->type = type;
    this->optionName = optionName;
}
std::string TypeTestCriteria::toString() {
    std::string result = "";
    switch (type) {
    case Test_int:
        result = "Expects an int.";
        break;
    case Test_double:
        result = "Expects a double.";
        break;
    case Test_string:
    default:
        result = "Expects a string.";
        break;
    }
    return result;
}
void TypeTestCriteria::check(const std::string &value) {
    switch (type) {
    case Test_int:    
        try {
            std::stoi(value);
        } catch (const std::exception& e) {
            error(optionName, "Should be an int.");
        }
        break;
    case Test_double:
        try {
            std::stod(value);
        } catch (const std::exception& e) {
            error(optionName, "Should be a double.");
        }
        break;
    case Test_string:
    default:
        break;
    }
}

NumberListTestCritera::NumberListTestCritera(const std::string &optionName) {
    this->optionName = optionName;
}
std::string NumberListTestCritera::toString() {
    std::stringstream ss;
    ss << "options:  ";
    for (int n : numbers)
        ss << n << ", ";
    std::string result = ss.str().substr(0, ss.str().length()-2); 
    return result;
}
void NumberListTestCritera::check(const std::string &value) {
    try {
        int n = std::stoi(value);
        if (!std::count(numbers.begin(), numbers.end(), n)) {
            error(optionName, "The chosen number is not allowed.");
        }
    } catch (const std::exception& e) {
        error(optionName, "Failed to parse the number.");
    }
}
NumberListTestCritera *NumberListTestCritera::add(int number) {
    if (std::count(numbers.begin(), numbers.end(), number))
        error(optionName, "The same number cannot be added twice.");
    numbers.push_back(number);
    return this;
}

RangeTestCriteria::RangeTestCriteria(const std::string &optionName, int start, int end) {
    this->start = start;
    this->end = end;
    this->optionName = optionName;
}
std::string RangeTestCriteria::toString() {
    std::stringstream ss;
    ss << start << '-' << end;
    return ss.str();
}
void RangeTestCriteria::check(const std::string &value) {
    try {
        int n = std::stoi(value);
        if (n < start || n > end) {
            error(optionName, "The chosen number was not in the correct range.");
        }
    } catch (const std::exception& e) {
        error(optionName, "Failed to parse the number.");
    }
}

NumberRangeTestCriteria::NumberRangeTestCriteria(const std::string &optionName) {
    this->optionName = optionName;
}
std::string NumberRangeTestCriteria::toString() {
    std::stringstream ss;
    ss << "options:  ";
    for (std::pair<int, int> *r : ranges)
        ss << r->first << '-' << r->second << ", ";
    return ss.str().substr(0, ss.str().length()-2);
}
void NumberRangeTestCriteria::check(const std::string &value) {
    try {
        int n = std::stoi(value);
        bool found = false;
        for (int number : numbers) {
            if (number == n) {
                found = true;
            }
        }
        for (std::pair<int, int> *p : ranges) {
            if (n >= p->first && n <= p->second) {
                found = true;
            }
        }
        if (!found) {
            error(optionName, "The chosen number was not in the correct range.");
        }
    } catch (const std::exception& e) {
        error(optionName, "Failed to parse the number.");
    }
}
NumberRangeTestCriteria *NumberRangeTestCriteria::add(int number) {
    for (int n : numbers)
        if (n == number)
            error(optionName, "The same number cannot be added twice.");
    numbers.push_back(number);
    return this;
}
NumberRangeTestCriteria *NumberRangeTestCriteria::addRange(int start, int end) {
    for (std::pair<int, int> *range : ranges)
        if (range->first == start && range->second == end)
            error(optionName, "The same range cannot be added twice.");
    ranges.push_back(new std::pair<int, int>(start, end));
    return this;
}

OneOfStringTestCriteria::OneOfStringTestCriteria(const std::string &optionName, bool matchCase) {
    this->matchCase = matchCase;
    this->optionName = optionName;
}
std::string OneOfStringTestCriteria::toString() {
    std::stringstream ss;
    ss << "options:  ";
    for (std::string &p : possibilities)
        ss << p << ", ";
    return ss.str().substr(0, ss.str().length()-2);
}
void OneOfStringTestCriteria::check(const std::string &value) {
    std::string v = value;
    if (!matchCase)
        for (size_t i = 0; i < v.size(); i++)
            v[i] = tolower(v[i]);
    if (possibilities.size() && !std::count(possibilities.begin(), possibilities.end(), v))
        error(optionName, "The chosen value was not found in the configured options: " + v);
}
OneOfStringTestCriteria *OneOfStringTestCriteria::add(const std::string &possibility) {
    std::string v = possibility;
    if (!matchCase)
        for (size_t i = 0; i < v.size(); i++)
            v[i] = tolower(v[i]);
    if (std::count(possibilities.begin(), possibilities.end(), v))
        error(optionName, "The same possibility cannot be added twice.");
    possibilities.push_back(v);
    return this;
}

/* -------------------------------------------------------------------------- */
/*                                    Verbs                                   */
/* -------------------------------------------------------------------------- */
Verb::Verb(const std::string name, const std::string desc) {
    this->name = name;
    this->desc = desc;
    this->isPresent = false;
}

/* -------------------------------------------------------------------------- */
/*                                   Options                                  */
/* -------------------------------------------------------------------------- */
Option::Option(const std::string &fullName, const char chrName, const std::string &desc, bool expectsValue, bool required, bool valueRequired) {
    this->fullName = fullName;
    this->chrName = chrName;
    this->desc = desc;
    this->expectsValue = expectsValue;
    this->required = required;
    this->isPresent = false;
    this->valueRequired = valueRequired;
}

Option &Option::addTestCriteria(TestCriteriaBase *test) {
    if (std::count(testCriteria.begin(), testCriteria.end(), test))
        error("Two of the same criteria cannot be added");
    testCriteria.push_back(test);
    return *this;
}
void Option::check() {
    for (TestCriteriaBase *test: testCriteria)
        test->check(value);
}

/* -------------------------------------------------------------------------- */
/*                                    Error                                   */
/* -------------------------------------------------------------------------- */
void arg_parser::error(const std::string msg) {
    error("%s\n", msg);   
}
void arg_parser::error(const std::string format, const std::string msg) {
    printf(format.c_str(), msg.c_str());
    exit(1);
}

/* -------------------------------------------------------------------------- */
/*                                 getFullName                                */
/* -------------------------------------------------------------------------- */
std::string arg_parser::getFullName(const char chrName) {
    return shortPrefix + chrName;
}
std::string arg_parser::getFullName(const std::string fullName) {
    return longPrefix + fullName;
}

/* -------------------------------------------------------------------------- */
/*                                  ArgParser                                 */
/* -------------------------------------------------------------------------- */
ArgParser::~ArgParser() {
    for (Option *option: options)
        delete option;
    for (Verb *verb: verbs)
        delete verb;
    verbs.clear();
    verbsMap.clear();
    options.clear();
    optionsMap.clear();
    after.clear();
    programName.clear();
    header.clear();
    footer.clear();
}

void ArgParser::reset() {
    for (Option *option : options) {
        option->isPresent = false;
        option->value = "";
    }
    for (Verb *verb : verbs) {
        verb->isPresent = false;
    }
}

void ArgParser::setProgramName(const std::string programName) {
    this->programName = programName;
}
void ArgParser::setHelpHeader(const std::string header) {
    this->header = header;
}
void ArgParser::setHelpFooter(const std::string footer) {
    this->footer = footer;
}
Option &ArgParser::addOption(const std::string &fullName, const char chrName, const std::string &desc, bool expectsValue, bool required, bool valueRequired) {
    if (fullName == "help" || chrName == '?')
        error("Both '--help' and '-?' are reserved.");
    std::string longName = getFullName(fullName);
    std::string shortName = getFullName(chrName);
    if (optionsMap.count(longName))
        error("An option with the same name has already been added: %s", longName);
    Option *option = new Option(fullName, chrName, desc, expectsValue, required, valueRequired);
    optionsMap[longName] = option;
    if (chrName)
        optionsMap[shortName] = option;
    options.push_back(option);
    return *option;
}
Verb &ArgParser::addVerb(const std::string name, const std::string desc) {
    if (verbsMap.count(name))
        error("A verb with the same name has already been added: %s", name);
    Verb *verb = new Verb(name, desc);
    verbs.push_back(verb);
    verbsMap[name] = verb;
    return *verb;
}

void ArgParser::parse(const int argc, const char **argv) {
    reset();
    if (argc < 2) {
        printHelp();
        exit(0);
    } else {
        int start = 1;
        for (; start < argc && argv[start][0] != shortPrefix[0]; start++) {
            if (verbsMap.count(argv[start])) {
                Verb *verb = verbsMap[argv[start]];
                if (!verb->isPresent) {
                    verb->isPresent = true;
                } else {
                    error("Multiple occurances of a verb: %s\n", verb->name);
                }
            } else {
                error("The provided verb was not recognised: %s\n", argv[start]);
            }
        }
        // stores whether the arguments are options, not checking if they are configured
        bool *argIsOption = new bool[std::max(0, argc-1)];
        for (int i = start; i < argc; i++) {
            argIsOption[i] = argv[i][0] == shortPrefix[0];
        }
        for (int i = start; i < argc; i++) {
            if (argIsOption[i] && argv[i] == longPrefix && i+1 < argc && argIsOption[i+1]) {
                argIsOption[i+1] = false;
            }
        }

        // finds the position of the last option
        int lastOption = 1;
        for (int i = start; i < argc; i++) {
            if (argIsOption[i] && argv[i] != longPrefix) {
                lastOption = i;
            }
        };

        // parses the options and any arguments after
        for (int i = start; i < argc; i++) {
            if (argIsOption[i]) {
                if (argv[i] == longPrefix) { // escape sequence
                    if (i+1 > argc) { // no option after
                        error("An escape sequences was detected, but not followed by a value.");
                    }
                } else if (argv[i] == getFullName('?') || argv[i] == getFullName("help")) {
                    printHelp();
                    exit(0);
                } else if (optionsMap.count(argv[i])) {
                    Option *option = optionsMap[argv[i]];
                    if (!option->isPresent) {
                        option->isPresent = true;
                        if (option->expectsValue) { // expects a value
                            if (i+1 < argc && argIsOption[i+1] && argv[i+1] == longPrefix) {
                                i++; // skip, escape sequence
                            }

                            if (i+1 < argc && !argIsOption[i+1]) {
                                option->value = argv[++i];
                            } else { // no valid option
                                error("A required argument was not present for the option: %s\n", argv[i]);
                            }
                        }
                    } else {
                        std::string optionName = getFullName(option->chrName) + " / " + getFullName(option->fullName);
                        error("Multiple occurances of an option: %s\n", optionName);
                    }
                } else {
                    error("Unrecognised option: %s\n", argv[i]);
                }
            } else if (i >= lastOption) {
                after.push_back(argv[i]);
            } else {
                error("Parameter without option: %s\n", argv[i]);
            }
        }
        delete [] argIsOption;

        // check that option conditions have been met
        for (Option *option: options) {
            std::string optionName = getFullName(option->chrName) + " / " + getFullName(option->fullName);
            if (option->required && !option->isPresent) {
                error("A required option was missing: %s\n", optionName);
            } else if (option->valueRequired && option->value == "") {
                error("A required option was missing a value: %s\n", optionName);
            }

            if (option->value != "") {
                option->check();
            }
        }
    }
}
bool ArgParser::isPresent(const char chrName) {
    std::string name = getFullName(chrName);
    if (!optionsMap.count(name))
        error("%s\n", "The selected option is not recognised.");
    return optionsMap[name]->isPresent;
}
bool ArgParser::isPresent(const std::string fullName) {
    std::string name = getFullName(fullName);
    if (!optionsMap.count(name))
        error("%s\n", "The selected option is not recognised.");
    return optionsMap[name]->isPresent;
}
bool ArgParser::verbPresent(const std::string name) {
    if (!verbsMap.count(name))
        error("%s\n", "The selected verb was not recognised.");
    return verbsMap[name]->isPresent;
}

std::string ArgParser::get(const char chrName) {
    std::string name = getFullName(chrName);
    if (!optionsMap.count(name))
        error("The specified option was not recognised: %s\n", name);
    Option *option = optionsMap[name];
    if (!option->expectsValue)
        error("The selected option does not accept a parameter: %s\n", name);
    return option->value;
}
std::string ArgParser::get(const std::string fullName) {
    std::string name = getFullName(fullName);
    if (!optionsMap.count(name))
        error("The specified option was not recognised: %s\n", name);
    Option *option = optionsMap[name];
    if (!option->expectsValue)
        error("The selected option does not accept a parameter: %s\n", name);
    return option->value;
}

void ArgParser::printHelp() {
    printf("Usage: %s [verb] [options]\n%s\noptions:\n", programName.c_str(), header.c_str());
    for (Option *option : options) {
        printf("    %2s, %-12s\t%s\n", 
            (shortPrefix + option->chrName).c_str(),
            (longPrefix + option->fullName).c_str(),
            option->desc.c_str());
    }
    printf("%s", footer.c_str());
}
