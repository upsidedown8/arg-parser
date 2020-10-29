#pragma once
#include <string>
#include <vector>
#include <map>
#include <sstream>

namespace cpp_arg_parser {

struct option;
struct verb;

/* -------------------------------------------------------------------------- */
/*                                TestCriteria                                */
/* -------------------------------------------------------------------------- */
class test_criteria_base {
public:
    option *parent = nullptr;

    virtual ~test_criteria_base() {}

    void error(const std::string& msg);

    virtual std::string toString() = 0;
    virtual void check(const std::string &value) = 0;
};

class custom_test_criteria : public test_criteria_base {
private:
    std::string desc, errorMsg;
    bool (*evalFnPtr)(const std::string&);

public:
    custom_test_criteria(const std::string &errorMsg, const std::string &desc, bool (*evalFnPtr)(const std::string&));

    std::string toString();
    void check(const std::string &value);
};
custom_test_criteria &createCustom(const std::string &errorMsg, const std::string &desc, bool (*evalFnPtr)(const std::string&));

enum class TestTypes {
    Test_int, Test_double, Test_string
};

class type_test_criteria : public test_criteria_base {
private:
    TestTypes type;

public:
    type_test_criteria(TestTypes type);

    std::string toString();
    void check(const std::string &value);
};
type_test_criteria &createTypeTest(TestTypes type);

class number_list_test_criteria : public test_criteria_base {
private:
    std::vector<int> numbers;

public:
    std::string toString();
    void check(const std::string &value);
    number_list_test_criteria &add(int number);
};
number_list_test_criteria &createNumberList(const std::string &optionName);

class range_test_criteria : public test_criteria_base {
public:
    int start, end;

    range_test_criteria(int start, int end);

    std::string toString();
    void check(const std::string &value);
};
range_test_criteria &createRange(int start, int end);

class number_range_test_criteria : public test_criteria_base {
private:
    std::vector<int> numbers;
    std::vector<std::pair<int, int>*> ranges;

public:
    std::string toString();
    void check(const std::string &value);
    number_range_test_criteria &add(int number);
    number_range_test_criteria &addRange(int start, int end);
};
number_range_test_criteria &createNumberRange();

class one_of_string_test_criteria : public test_criteria_base {
private:
    bool matchCase;
    std::vector<std::string> possibilities;

public:
    one_of_string_test_criteria(bool matchCase);

    std::string toString();
    void check(const std::string &value);
    one_of_string_test_criteria &add(const std::string &possibility);
};
one_of_string_test_criteria &createOneOfString(bool matchCase);

/* -------------------------------------------------------------------------- */
/*                                   Options                                  */
/* -------------------------------------------------------------------------- */
struct option {
    bool expectsValue, isPresent, valueRequired, required;
    char chrName;
    verb *parent;
    void (*actionFn)(option *) = nullptr;
    std::string desc, fullName, value;
    std::vector<test_criteria_base*> testCriteria;

    option(const std::string &fullName, const char chrName, const std::string &desc, bool expectsValue, bool required);

    option &addAction(void (*action)(option *));
    option &addTestCriteria(test_criteria_base &test);
    void check();
    void clear();
    void runAction();
    void printHelp();
    void printCriteria();
};

option &createOption(const std::string &fullName, const char chrName, const std::string &desc, bool expectsValue, bool required);

/* -------------------------------------------------------------------------- */
/*                                    Verbs                                   */
/* -------------------------------------------------------------------------- */
struct verb {
    bool isPresent;
    void (*actionFn)(verb *);
    verb *parent;
    std::string name, desc;
    std::map<std::string, verb*> verbsMap;
    std::map<std::string, option*> optionsMap;
    std::vector<verb*> verbs;
    std::vector<option*> options;

    verb(const std::string name, const std::string desc);

    verb &addAction(void (*action)(verb *));
    verb &addOption(option &opt);
    verb &addVerb(verb &v);

    void reset();
    void clear();
    void runAction();
    void printHelp();
    void printVerbs(std::string prefix = "", bool isLast = true);
};

verb &createVerb(const std::string &name, const std::string &desc);

/* -------------------------------------------------------------------------- */
/*                                    Error                                   */
/* -------------------------------------------------------------------------- */
void error(const std::string msg);
void error(const std::string format, const std::string msg);

/* -------------------------------------------------------------------------- */
/*                                 getFullName                                */
/* -------------------------------------------------------------------------- */
std::string getFullName(const char chrName);
std::string getFullName(const std::string &fullName);

/* -------------------------------------------------------------------------- */
/*                                  arg_parser                                */
/* -------------------------------------------------------------------------- */
class arg_parser {
private:
    bool autoPrintHelp;
    std::string programName, header, footer;
    std::vector<std::string> after, verbPatternStr;
    std::vector<verb*> verbPattern;
    verb *root, *selected;

public:
    arg_parser(bool autoPrintHelp = false);
    ~arg_parser();

    void reset();

    arg_parser &setProgramName(const std::string &programName);
    arg_parser &setHelpHeader(const std::string &header);
    arg_parser &setHelpFooter(const std::string &footer);
    arg_parser &addOption(option &o);
    arg_parser &addVerb(verb &v);

    void parse(const int argc, char **argv);
    bool isPresent(const char chrName);
    bool isPresent(const std::string &fullName);
    bool verbPresent(const std::string &name);

    std::string getString(const char chrName);
    std::string getString(const std::string &fullName);

    template<typename T>
    T get(const char chrName) {
        T t;
        std::stringstream ss;
        ss << getString(chrName);
        ss >> t;
        return t;
    }
    template<typename T>
    T get(const std::string &fullName) {
        T t;
        std::stringstream ss;
        ss << getString(fullName);
        ss >> t;
        return t;
    }

    void printHelp(verb *v);
    void printVerbs();
};

} // namespace cpp_arg_parser
