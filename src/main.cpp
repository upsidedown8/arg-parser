#include "cpp-arg-parser/cpp-arg-parser.hpp"
#include "../test.hpp"
#include <iostream>

using namespace cpp_arg_parser;
using namespace std;

int main(int argc, char **argv) {
    ArgumentParser argParser("crypto", "[VERB] [OPTIONS]", "--help");

    // argParser
    //     .addVerb("encrypt")
    //     .addVerb("decrypt")
    //     .addVerb("solve")
    //     .addVerb("console");

    argParser.addOption<bool>("ciphers", '\0', "Show all cipher types", false, false);
    argParser.addOption<std::string>("mode", 'm', "The cipher mode to use", true, "encrypt");

    // argParser
    //     .addOption( true,  true, "The cipher to use",           "-c")
    //     .addOption( true,  true, "The mode to use",             "-m")
    //     .addOption(false, false, "Don't store the punctuation", "-nopunc")
    //     .addOption(false, false, "Open the crypto CLI",         "--cli")
    //     .addOption(false, false, "Show the help text",          "--help")
    //     .addOption(false, false, "Show all cipher types",       "--ciphers");
    
    // argParser.display();
    // auto parsedArgs = argParser.parse(argc, argv);
    // argParser.print(parsedArgs);

    return 0;
}
