#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

// Include your header files
#include "Lexer.h"
#include "Parser.h"
#include "Compiler.h"
#include "VM.h"
#include <chrono> // Added for execution timing

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./cvm <path_to_script.cvm>\n";
        return 1;
    }

    // Read the source file
    std::ifstream file(argv[1]);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sourceCode = buffer.str();

    // PHASE 1: LEXER
    std::cout << "\n[1/4] 🔍 LEXER: Tokenizing source code..." << std::endl;
    Lexer lexer(sourceCode);
    std::vector<Token> tokens = lexer.scanTokens(); // FIXED: Changed to scanTokens()

    // PHASE 2: PARSER
    std::cout << "[2/4] 🌳 PARSER: Building Abstract Syntax Tree (AST)..." << std::endl;
    Parser parser(tokens);
    auto ast = parser.parse();

    // PHASE 3: COMPILER
    std::cout << "[3/4] ⚙️ COMPILER: Flattening AST into 8-bit Bytecode...\n" << std::endl;
    Compiler compiler;
    
    // FIXED: compile() returns a dictionary of functions!
    auto userFunctions = compiler.compile(ast);
    
    // Grab the main chunk and trigger YOUR built-in disassembler!
    userFunctions["main"].chunk.disassemble();

    // PHASE 4: VM
    std::cout << "\n[4/4] 🚀 VIRTUAL MACHINE: Booting CPU and executing bytecode...\n" << std::endl;
    std::cout << "================== OUTPUT ==================\n";
    
    VM vm;
    // --- START TIMING ---
    auto start_time = std::chrono::high_resolution_clock::now();

    // Execute the bytecode
    vm.interpret(userFunctions);

    // --- STOP TIMING ---
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end_time - start_time;

    std::cout << "\n============================================\n";
    std::cout << "✅VM Halted Successfully.\n";
    // std::cout << "Execution Time: " << elapsed.count() << " ms\n";

    return 0;
}
