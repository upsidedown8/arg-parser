#include "arg_parser/arg_parser.hpp"

#include <exception>
#include <algorithm>

using namespace cpp_arg_parser;

const size_t maxOptionLen = 15;
const std::string shortPrefix = "-";
const std::string ucscorePrefix = "--";

/* -------------------------------------------------------------------------- */
/*                                TestCriteria                                */
/* -------------------------------------------------------------------------- */
void test_criteria_base::error(const std::string& msg) {
    if (parent == nullptr) {
        printf("%s\n", msg.c_str());
    } else {
        printf("%s: %s\n", getFullName(parent->fullName).c_str(), msg.c_str());
        parent->printCriteria();
    }
    exit(1);
}

custom_test_criteria::custom_test_criteria(const std::string &errorMsg, const std::string &desc, bool (*evalFnPtr)(const std::string&)) {
    if (desc.length() > 100) {
        error("The maximum description length is 100 chars");
    }
    this->desc = desc;
    this->evalFnPtr = evalFnPtr;
}
std::string custom_test_criteria::toString() {
    return "Custom test: " + desc;
}
void custom_test_criteria::check(const std::string &value) {
    if (!evalFnPtr(value)) {
        error(errorMsg);
    }
}
custom_test_criteria &createCustom(const std::string &errorMsg, const std::string &desc, bool (*evalFnPtr)(const std::string&)) {
    return *new custom_test_criteria(errorMsg, desc, evalFnPtr);
}

type_test_criteria::type_test_criteria(TestTypes type) {
    this->type = type;
}
std::string type_test_criteria::toString() {
    std::string result = "Type: ";
    switch (type) {
    case TestTypes::Test_int:
        result += "int";
        break;
    case TestTypes::Test_double:
        result += "double";
        break;
    case TestTypes::Test_string:
    default:
        result += "string";
        break;
    }
    return result;
}
void type_test_criteria::check(const std::string &value) {
    switch (type) {
    case TestTypes::Test_int:
        try {
            std::stoi(value);
        } catch (const std::exception&) {
            error("Should be an int");
        }
        break;
    case TestTypes::Test_double:
        try {
            std::stod(value);
        } catch (const std::exception&) {
            error("%s: should be a double");
        }
        break;
    case TestTypes::Test_string:
    default:
        break;
    }
}
type_test_criteria &cpp_arg_parser::createTypeTest(TestTypes type) {
    return *new type_test_criteria( type);
}

std::string number_list_test_criteria::toString() {
    std::stringstream ss;
    ss << "Options:  ";
    for (int n : numbers)
        ss << n << ", ";
    std::string result = ss.str().substr(0, ss.str().length()-2); 
    return result;
}
void number_list_test_criteria::check(const std::string &value) {
    try {
        int n = std::stoi(value);
        if (!std::count(numbers.begin(), numbers.end(), n)) {
            error("The chosen number is not allowed");
        }
    } catch (const std::exception&) {
        error("Failed to parse the number");
    }
}
number_list_test_criteria &number_list_test_criteria::add(int number) {
    if (std::count(numbers.begin(), numbers.end(), number))
        error("The same number cannot be added twice.");
    numbers.push_back(number);
    return *this;
}
number_list_test_criteria &cpp_arg_parser::createNumberList(const std::string &optionName) {
    return *new number_list_test_criteria;
}

range_test_criteria::range_test_criteria(int start, int end) {
    this->start = start;
    this->end = end;
}
std::string range_test_criteria::toString() {
    std::stringstream ss;
    ss << "Range: " << start << '-' << end;
    return ss.str();
}
void range_test_criteria::check(const std::string &value) {
    try {
        int n = std::stoi(value);
        if (n < start || n > end) {
            error("The chosen number was not in the correct range");
        }
    } catch (const std::exception&) {
        error("Failed to parse the number");
    }
}
range_test_criteria &cpp_arg_parser::createRange(int start, int end) {
    return *new range_test_criteria(start, end);
}

std::string number_range_test_criteria::toString() {
    std::stringstream ss;
    ss << "Options:  ";
    for (int n: numbers)
        ss << n << ", ";
    for (std::pair<int, int> *r : ranges)
        ss << r->first << '-' << r->second << ", ";
    return ss.str().substr(0, ss.str().length()-2);
}
void number_range_test_criteria::check(const std::string &value) {
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
            error("The chosen number was not in the correct range");
        }
    } catch (const std::exception&) {
        error("Failed to parse the number");
    }
}
number_range_test_criteria &number_range_test_criteria::add(int number) {
    for (int n : numbers)
        if (n == number)
            error("The same number cannot be added twice.");
    numbers.push_back(number);
    return *this;
}
number_range_test_criteria &number_range_test_criteria::addRange(int start, int end) {
    for (std::pair<int, int> *range : ranges)
        if (range->first == start && range->second == end)
            error("The same range cannot be added twice.");
    ranges.push_back(new std::pair<int, int>(start, end));
    return *this;
}
number_range_test_criteria &cpp_arg_parser::createNumberRange() {
    return *new number_range_test_criteria;
}

one_of_string_test_criteria::one_of_string_test_criteria(bool matchCase) {
    this->matchCase = matchCase;
}
std::string one_of_string_test_criteria::toString() {
    std::stringstream ss;
    ss << "Options: ";
    for (std::string &p : possibilities)
        ss << p << ", ";
    return ss.str().substr(0, ss.str().length()-2);
}
void one_of_string_test_criteria::check(const std::string &value) {
    std::string v = value;
    if (!matchCase)
        for (size_t i = 0; i < v.size(); i++)
            v[i] = tolower(v[i]);
    if (possibilities.size() && !std::count(possibilities.begin(), possibilities.end(), v))
        error("The chosen value was not found in the configured options: " + v);
}
one_of_string_test_criteria &one_of_string_test_criteria::add(const std::string &possibility) {
    std::string v = possibility;
    if (!matchCase)
        for (size_t i = 0; i < v.size(); i++)
            v[i] = tolower(v[i]);
    if (std::count(possibilities.begin(), possibilities.end(), v))
        error("The same possibility cannot be added twice.");
    possibilities.push_back(v);
    return *this;
}
one_of_string_test_criteria &cpp_arg_parser::createOneOfString(bool matchCase) {
    return *new one_of_string_test_criteria(matchCase);
}

/* -------------------------------------------------------------------------- */
/*                                    Verbs                                   */
/* -------------------------------------------------------------------------- */
verb::verb(const std::string name, const std::string desc) {
    if (desc.length() > 100) {
        error("The maximum description length is 100 chars");
    } else if (name.length() > 16) {
        error("Don't you think that verb is a bit ucscore?: %s\n", name);
    } else if (name == "" || name == ucscorePrefix || name[0] == '-') {
        error("Verb name cannot be empty or start with '-': %s", name);
    }
    for (char c : name) {
        if (!(isalnum(c) || c == '-' || c == '_')) {
            cpp_arg_parser::error("The verb name (" + name +") contains an invalid character: " + c);
        }
    }
    this->name = name;
    this->desc = desc;
    this->isPresent = false;
}

verb &verb::addAction(void (*action)(verb *)) {
    if (action != nullptr) {
        actionFn = action;
    }
    return *this;
}
verb &verb::addOption(option &opt) {
    std::string ucscoreName = getFullName(opt.fullName);
    std::string shortName = getFullName(opt.chrName);
    if (optionsMap.count(ucscoreName))
        error("An option with the same name has already been added: %s", ucscoreName);
    optionsMap[ucscoreName] = &opt;
    if (opt.chrName)
        optionsMap[shortName] = &opt;
    options.push_back(&opt);
    opt.parent = this;
    return *this;
}
verb &verb::addVerb(verb &v) {
    if (parent != nullptr && parent == &v)
        error("Cannot add the parent of a verb as its child: %s\n", v.name);
    if (verbsMap.count(v.name))
        error("A verb with the same name has already been added: %s\n", v.name);
    verbs.push_back(&v);
    verbsMap[v.name] = &v;
    v.parent = this;
    return *this;
}

void verb::reset() {
    isPresent = false;
    for (option *option : options) {
        option->isPresent = false;
        option->value = "";
    }
    for (verb *verb : verbs) {
        verb->reset();
    }
}
void verb::clear() {
    for (verb *child : verbs)
        child->clear();
    for (option *opt : options)
        opt->clear();
    verbs.clear();
    options.clear();
    verbsMap.clear();
    optionsMap.clear();
    delete this;
}
void verb::runAction() {
    if (actionFn != nullptr && isPresent) {
        actionFn(this);
    }
    for (verb *v: verbs) {
        v->runAction();
    }
}
void verb::printHelp() {
    for (option *option : options) {
        option->printHelp();
    }
}
void verb::printVerbs(std::string prefix, bool isLast) {
    if (desc.length()) {
        printf("%s%s %s: (%s)\n", 
            prefix.c_str(),
            isLast ? "└──" : "├──",
            name.c_str(),
            desc.c_str());
    } else {
        printf("%s%s %s\n", 
            prefix.c_str(),
            isLast ? "└──" : "├──",
            name.c_str());
    }
    for (size_t i = 0; i < verbs.size(); i++)
        verbs[i]->printVerbs(
            prefix + (isLast ? "    " : "│   "), 
            i+1 >= verbs.size());
}

verb &cpp_arg_parser::createVerb(const std::string &name, const std::string &desc) {
    return *new verb(name, desc);
}

/* -------------------------------------------------------------------------- */
/*                                   Options                                  */
/* -------------------------------------------------------------------------- */
option::option(const std::string &fullName, const char chrName, const std::string &desc, bool expectsValue, bool required) {
    if (desc.length() > 100) {
        error("The maximum description length is 100 chars");
    } else if (fullName.length() > maxOptionLen || fullName.length() < 2) {
        cpp_arg_parser::error(
            "The option name must be at least 2 chars and less than the maximum length of: " + 
            std::to_string(maxOptionLen) + "\n");
    } else if (fullName[0] == '-') {
        error("Option name cannot start with '-': %s\n", fullName);
    } else if (fullName == "help" || fullName == "verbs" || chrName == '?' || chrName == '-') {
        error("Invalid name as '--help', '--verbs'. '--' and '-?' are reserved: --%s\n", 
            getFullName(fullName) + " / -" + getFullName(chrName));
    }
    for (char c : fullName) {
        if (!(isalnum(c) || c == '-' || c == '_')) {
            cpp_arg_parser::error("The option name (" + getFullName(fullName) +") contains an invalid character: " + c);
        }
    }
    this->fullName = fullName;
    this->chrName = chrName;
    this->desc = desc;
    this->expectsValue = expectsValue;
    this->required = required;
    this->isPresent = false;
}

option &option::addAction(void (*action)(option *)) {
    if (action != nullptr) {
        actionFn = action;
    }
    return *this;
}
option &option::addTestCriteria(test_criteria_base &test) {
    if (std::count(testCriteria.begin(), testCriteria.end(), &test))
        error("Two of the same criteria cannot be added");
    testCriteria.push_back(&test);
    test.parent = this;
    return *this;
}
void option::check() {
    for (test_criteria_base *test: testCriteria)
        test->check(value);
}
void option::clear() {
    for (test_criteria_base *b : testCriteria)
        delete b;
    delete this;
}
void option::runAction() {
    if (actionFn != nullptr && isPresent) {
        actionFn(this);
    }
}
void option::printHelp() {
    std::string optionLenStr = std::to_string(maxOptionLen);
    if (chrName) {
        std::string format = "    %2s, --%-" + optionLenStr + "s    %s\n";
        printf(format.c_str(), getFullName(chrName).c_str(), fullName.c_str(), desc.c_str());
    } else {
        std::string format = "        --%-" + optionLenStr + "s    %s\n";
        printf(format.c_str(), fullName.c_str(), desc.c_str());
    }
}
void option::printCriteria() {
    if (testCriteria.size())
        printf("Criteria:\n");

    for (test_criteria_base *criteria: testCriteria)
        printf("\t%s\n", criteria->toString().c_str());
}

option &cpp_arg_parser::createOption(const std::string &fullName, const char chrName, const std::string &desc, bool expectsValue, bool required) {
    return *new option(fullName, chrName, desc, expectsValue, required);
}

/* -------------------------------------------------------------------------- */
/*                                    Error                                   */
/* -------------------------------------------------------------------------- */
void cpp_arg_parser::error(const std::string msg) {
    error("%s\n", msg);   
}
void cpp_arg_parser::error(const std::string format, const std::string msg) {
    printf(format.c_str(), msg.c_str());
    exit(1);
}

/* -------------------------------------------------------------------------- */
/*                                 getFullName                                */
/* -------------------------------------------------------------------------- */
std::string cpp_arg_parser::getFullName(const char chrName) {
    return shortPrefix + chrName;
}
std::string cpp_arg_parser::getFullName(const std::string &fullName) {
    return ucscorePrefix + fullName;
}

/* -------------------------------------------------------------------------- */
/*                                  arg_parser                                 */
/* -------------------------------------------------------------------------- */
arg_parser::arg_parser(bool autoPrintHelp) {
    root = new verb(programName == "" ? "root" : programName, "");
    selected = root;
    this->autoPrintHelp = autoPrintHelp;
}
arg_parser::~arg_parser() {
    root->clear();
    after.clear();
    programName.clear();
    header.clear();
    footer.clear();
    verbPatternStr.clear();
    verbPattern.clear();
}

void arg_parser::reset() {
    root->reset();
    verbPattern.clear();
    verbPatternStr.clear();
    selected = root;
}

arg_parser &arg_parser::setProgramName(const std::string &programName) {
    this->programName = programName;
    return *this;
}
arg_parser &arg_parser::setHelpHeader(const std::string &header) {
    this->header = header;
    return *this;
}
arg_parser &arg_parser::setHelpFooter(const std::string &footer) {
    this->footer = footer;
    return *this;
}
arg_parser &arg_parser::addOption(option &o) {
    root->addOption(o);
    return *this;
}
arg_parser &arg_parser::addVerb(verb &v) {
    root->addVerb(v);
    return *this;
}

void arg_parser::parse(const int argc, char **argv) {
    reset();
    if (argc < 2 && autoPrintHelp) {
        printHelp(root);
        exit(0);
    } else {
        if (programName == "")
            programName = argv[0];
        int start = 1;
        for (; start < argc && argv[start][0] != shortPrefix[0]; start++) {
            selected->isPresent = true;
            if (selected->verbsMap.count(argv[start])) {
                selected = selected->verbsMap[argv[start]];
                verbPatternStr.push_back(argv[start]);
                verbPattern.push_back(selected);
            } else {
                error("The provided verb was not recognised: %s\n", argv[start]);
            }
        }

        // print help and exit if no args supplied
        if (start >= argc && autoPrintHelp) { 
            printHelp(selected);
            exit(0);
        }

        // stores whether the arguments are options, not checking if they are configured
        bool* argIsOption = new bool[argc] {};
        bool* argIsHelp = new bool[argc] {};
        for (int i = start; i < argc; i++) {
            argIsOption[i] = argv[i][0] == shortPrefix[0];
        }
        for (int i = start; i < argc; i++) {
            if (argIsOption[i] && argv[i] == ucscorePrefix && i+1 < argc && argIsOption[i+1]) {
                argIsOption[i+1] = false;
            }
        }

        // finds the position of the last option
        int lastOption = 1;
        for (int i = start; i < argc; i++) {
            if (argIsOption[i] && argv[i] != ucscorePrefix) {
                lastOption = i;
            }
        };

        // check for help keywords
        for (int i = start; i < argc; i++) {
            argIsHelp[i] = true;
            if (argIsOption[i] && (argv[i] == getFullName('?') || argv[i] == getFullName("help"))) {
                printHelp(selected);
                exit(0);
            } else if (argv[i] == getFullName("verbs")) {
                printVerbs();
                exit(0);
            } else {
                argIsHelp[i] = false;
            }
        }

        // parses the options and any arguments after
        for (int i = start; i < argc; i++) {
            if (argIsHelp[i]) {
                continue;
            } else if (argIsOption[i]) {
                if (argv[i] == ucscorePrefix) { // escape sequence
                    if (i+1 > argc) { // no option after
                        error("An escape sequences was detected, but not followed by a value.");
                    }
                } else if (selected->optionsMap.count(argv[i])) {
                    option *option = selected->optionsMap[argv[i]];
                    if (!option->isPresent) {
                        option->isPresent = true;
                        if (option->expectsValue) { // expects a value
                            if (i+1 < argc && argIsOption[i+1] && argv[i+1] == ucscorePrefix) {
                                i++; // skip, escape sequence
                            }

                            if (i+1 < argc && !argIsOption[i+1]) {
                                option->value = argv[++i];
                            } else { // no valid option
                                printf("A required argument was not present for the option: %s\n", argv[i]);
                                option->printCriteria();
                                exit(1);
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
        for (option *option: selected->options) {
            std::string optionName = getFullName(option->chrName) + " / " + getFullName(option->fullName);
            if (option->required && (!option->isPresent || (option->expectsValue && option->value == ""))) {
                printf("A required option was missing: %s\n", optionName.c_str());
                option->printCriteria();
                exit(1);
            }

            if (option->value != "") {
                option->check();
            }
        }

        // run any actions (recursive)
        root->runAction();
        for (option *option: selected->options) {
            if (option->isPresent) {
                option->runAction();
            }
        }
    }
}
bool arg_parser::isPresent(const char chrName) {
    std::string name = getFullName(chrName);
    return selected->optionsMap.count(name) && selected->optionsMap[name]->isPresent;
}
bool arg_parser::isPresent(const std::string &fullName) {
    std::string name = getFullName(fullName);
    return selected->optionsMap.count(name) && selected->optionsMap[name]->isPresent;
}
bool arg_parser::verbPresent(const std::string &name) {
    for (const auto verbPtr : verbPattern)
        if (verbPtr->name == name)
            return true;
    return false;
}

std::string arg_parser::getString(const char chrName) {
    std::string name = getFullName(chrName);
    if (!selected->optionsMap.count(name))
        error("The specified option was not recognised: %s\n", name);
    option *option = selected->optionsMap[name];
    if (!option->expectsValue)
        error("The selected option does not accept a parameter: %s\n", name);
    return option->value;
}
std::string arg_parser::getString(const std::string &fullName) {
    std::string name = getFullName(fullName);
    if (!selected->optionsMap.count(name))
        error("The specified option was not recognised: %s\n", name);
    option *option = selected->optionsMap[name];
    if (!option->expectsValue)
        error("The selected option does not accept a parameter: %s\n", name);
    return option->value;
}

void arg_parser::printHelp(verb *v) {
    if (v != root) { // sub verb
        std::stringstream ss;
        for (std::string verb : verbPatternStr)
            ss << verb << ' ';
        printf("Usage: %s %s[options] [args]\n%s\n", programName.c_str(), ss.str().c_str(), header.c_str());
        printf("\nSelected verb pattern: %s %s\n", programName.c_str(), ss.str().c_str());
        if (v->desc.length()) {
            printf("Verb description: %s\n", v->desc.c_str());
        }
    } else {
        printf("Usage: %s [verbs] [options] [args]\n%s\n", programName.c_str(), header.c_str());
    }
    printf("\noptions:\n");
    v->printHelp();
    printf("        --verbs              Open the verbs tree\n");
    printf("    -?, --help               Open this help message\n");
    printf("%s", footer.c_str());
}
void arg_parser::printVerbs() {
    printf("%s: (program name)\n", programName.c_str());
    for (size_t i = 0; i < root->verbs.size(); i++)
        root->verbs[i]->printVerbs(
            "    ",
            i+1 >= root->verbs.size());
}
