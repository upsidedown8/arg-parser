#include "arg_parser/arg_parser.hpp"
#include <iostream>

using namespace cpp_arg_parser;

void action(cpp_arg_parser::option *opt) {
    printf("Option name: %s\n", cpp_arg_parser::getFullName(opt->fullName).c_str());
}

int main(int argc, const char **argv) {
    cpp_arg_parser::arg_parser argParser;

    argParser
        .setProgramName("crypto")
        .setHelpHeader(
            "\nWelcome to the crypto cli, an application for using and solving\n"
            "classical and modern ciphers. See below for how to use.")
        .setHelpFooter("\nFooter\n")
        .addOption(createOption("cipher", 'c', "The cipher to use", true, true)
            .addTestCriteria(createOneOfString(false)
                .add("affine")
                .add("atbash")
                .add("caesar"))
            .addTestCriteria(createTypeTest(Test_string)))
        .addOption(createOption("mode", 'm', "The mode to use", true, true)
            .addTestCriteria(createOneOfString(false)
                .add("encrypt")
                .add("decrypt")
                .add("solve")
                .add("analyse")))
        .addOption(createOption("action", '\0', "Run a test action", false, false)
            .addAction(action))
        .addOption(createOption("nopunc", '\0', "Don't store the punctuation", false, false))
        .addOption(createOption("key",    'k', "Pass the key argument to the cipher", true, false))
        .addOption(createOption("verbose",'v', "Show everything", false, false))
        .addOption(createOption("number", 'n', "A test to pass a number", true, false)
            .addTestCriteria(createNumberRange()
                .addRange(10, 20)
                .add(7)))
        .addVerb(createVerb("submodule", "desc")
            .addOption(createOption("someOption", '\0', "description", false, false))
            .addOption(createOption("someOtherOption", 's', "desc", true, false)
                .addTestCriteria(createNumberList("someOtherOption")
                    .add(1)
                    .add(2)
                    .add(50)))
            .addVerb(createVerb("some-verb", "some desc")))
        .addVerb(createVerb("verbname", "verbdesc"));
    
    argParser.parse(argc, argv);

    if (argParser.isPresent("cipher")) {
        printf("cipher: %s\n", argParser.getString("cipher").c_str());
    } if (argParser.isPresent("mode")) {
        printf("  mode: %s\n", argParser.getString("mode"  ).c_str());
    } if (argParser.isPresent("number")) {
        printf("number: %s\n", argParser.getString("number").c_str());
        printf("%d\n", argParser.get<int>("number"));
    }


    return 0;
}
