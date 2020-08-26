#include "arg_parser/arg_parser.hpp"
#include <iostream>

int main(int argc, const char **argv) {
    arg_parser::ArgParser argParser;
    argParser.setProgramName(argv[0]);

    argParser.addVerb("encrypt", "");
    argParser.addVerb("decrypt", "");
    argParser.addVerb("solve",   "");
    argParser.addVerb("console", "");
    
    argParser.addOption("cipher", 'c', "The cipher to use", true,  true, true )
        .addTestCriteria((new arg_parser::OneOfStringTestCriteria("cipher", false))
            ->add("caesar")
            ->add("affine")
            ->add("atbash"))
        .addTestCriteria((new arg_parser::TypeTestCriteria("cipher", arg_parser::Test_string)));
    argParser.addOption("mode",   'm', "The mode to use", true,  true, false)
        .addTestCriteria((new arg_parser::OneOfStringTestCriteria("mode", false))
            ->add("encrypt")
            ->add("decrypt")
            ->add("solve")
            ->add("analyse"));
    argParser.addOption("nopunc", 'n', "Don't store the punctuation",         false, false, false);
    argParser.addOption("cli",    'C', "Opens the crypto cli",                false, false, false);
    argParser.addOption("key",    'k', "Pass the key argument to the cipher",  true, false, false);
    argParser.addOption("verbose",'v', "Show everything",                     false, false, false);
    argParser.addOption("number", 'n', "A test to pass a number",              true, false, false)
        .addTestCriteria((new arg_parser::NumberRangeTestCriteria("number"))
            ->addRange(10, 20)
            ->add(7));

    argParser.parse(argc, argv);

    printf("cipher: %s\n", argParser.get("cipher").c_str());
    printf("  mode: %s\n", argParser.get("mode"  ).c_str());
    printf("number: %s\n", argParser.get("number").c_str());

    return 0;
}
