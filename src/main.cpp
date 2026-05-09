#include "Lexer.h"
#include "Parser.h"
#include "Compiler.h"
#include "VM.h"
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {
    // 1. Check if the user provided a file name
    if (argc != 2) {
        std::cerr << "Usage: ./cvm <script.cvm>\n";
        return 1;
    }

    // 2. Open the file
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << argv[1] << "'\n";
        return 1;
    }

    // 3. Read the entire file into a std::string
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string code = buffer.str();
    
    std::cout << "--- CVM++ Compiler Engine ---\n";
    std::cout << "Executing: " << argv[1] << "\n\n";

    // 4. The Standard Pipeline
    Lexer lexer(code);
    std::vector<Token> tokens = lexer.scanTokens();
    
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> ast = parser.parse(); 
    
    Compiler compiler;
    std::unordered_map<std::string, Function> compiledFunctions = compiler.compile(ast);
    
    VM vm;
    vm.interpret(compiledFunctions);
    
    return 0;
}