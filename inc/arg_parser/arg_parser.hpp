#pragma once
#include <string>
#include <vector>
#include <map>
#include <exception>
#include <sstream>
#include <algorithm>

namespace arg_parser {

const std::string shortPrefix = "-";
const std::string longPrefix = "--";

/* -------------------------------------------------------------------------- */
/*                                TestCriteria                                */
/* -------------------------------------------------------------------------- */
class TestCriteriaBase {
public:
    std::string optionName;

    void error(const std::string &optionName, const std::string& msg);

    virtual std::string toString() = 0;
    virtual void check(const std::string &value) = 0;
};

enum TestTypes {
    Test_int, Test_double, Test_string
};

class TypeTestCriteria : public TestCriteriaBase {
private:
    TestTypes type;

public:
    TypeTestCriteria(const std::string &optionName, TestTypes type);

    std::string toString();
    void check(const std::string &value);
};

class NumberListTestCritera : public TestCriteriaBase {
private:
    std::vector<int> numbers;

public:
    NumberListTestCritera(const std::string &optionName);

    std::string toString();
    void check(const std::string &value);
    NumberListTestCritera *add(int number);
};

class RangeTestCriteria : public TestCriteriaBase {
public:
    int start, end;

    RangeTestCriteria(const std::string &optionName, int start, int end);

    std::string toString();
    void check(const std::string &value);
};

class NumberRangeTestCriteria : public TestCriteriaBase {
private:
    std::vector<int> numbers;
    std::vector<std::pair<int, int>*> ranges;

public:
    NumberRangeTestCriteria(const std::string &optionName);

    std::string toString();
    void check(const std::string &value);
    NumberRangeTestCriteria *add(int number);
    NumberRangeTestCriteria *addRange(int start, int end);
};

class OneOfStringTestCriteria : public TestCriteriaBase {
private:
    bool matchCase;
    std::vector<std::string> possibilities;

public:
    OneOfStringTestCriteria(const std::string &optionName, bool matchCase);

    std::string toString();
    void check(const std::string &value);
    OneOfStringTestCriteria *add(const std::string &possibility);
};

/* -------------------------------------------------------------------------- */
/*                                    Verbs                                   */
/* -------------------------------------------------------------------------- */
struct Verb {
    bool isPresent;
    std::string name, desc;

    Verb(const std::string name, const std::string desc);
};

/* -------------------------------------------------------------------------- */
/*                                   Options                                  */
/* -------------------------------------------------------------------------- */
struct Option {
    bool expectsValue, isPresent, valueRequired, required;
    char chrName;
    std::string desc, fullName, value;
    std::vector<TestCriteriaBase*> testCriteria;

    Option(const std::string &fullName, const char chrName, const std::string &desc, bool expectsValue, bool required, bool valueRequired);

    Option &addTestCriteria(TestCriteriaBase *test);
    void check();
};

/* -------------------------------------------------------------------------- */
/*                                    Error                                   */
/* -------------------------------------------------------------------------- */
void error(const std::string msg);
void error(const std::string format, const std::string msg);

/* -------------------------------------------------------------------------- */
/*                                 getFullName                                */
/* -------------------------------------------------------------------------- */
std::string getFullName(const char chrName);
std::string getFullName(const std::string fullName);

/* -------------------------------------------------------------------------- */
/*                                  ArgParser                                 */
/* -------------------------------------------------------------------------- */

class ArgParser {
private:
    std::string programName, header, footer;
    std::vector<Verb*> verbs;
    std::vector<Option*> options;
    std::vector<std::string> after;
    std::map<std::string, Verb*> verbsMap;
    std::map<std::string, Option*> optionsMap;

public:
    ~ArgParser();

    void reset();

    void setProgramName(const std::string programName);
    void setHelpHeader(const std::string header);
    void setHelpFooter(const std::string footer);
    Option &addOption(const std::string &fullName, const char chrName, const std::string &desc, bool expectsValue, bool required, bool valueRequired);
    Verb &addVerb(const std::string name, const std::string desc);

    void parse(const int argc, const char **argv);
    bool isPresent(const char chrName);
    bool isPresent(const std::string fullName);
    bool verbPresent(const std::string name);

    std::string get(const char chrName);
    std::string get(const std::string fullName);

    void printHelp();
};

} // namespace arg_parser
