#pragma once
#include "AST.h"
#include "Chunk.h"
#include <unordered_map>

class Compiler {
private:
    // The main script is technically just a function called "main"!
    Function mainFunction;     
    
    // A pointer to the function we are currently compiling bytecode into
    Function* currentFunction; 
    
    // A dictionary to store all the custom functions the user writes
    std::unordered_map<std::string, Function> userFunctions;

    // Helper functions to walk the tree
    void compileStatement(const Stmt* stmt);
    void compileExpression(const Expr* expr);
    
    // Helper to get the current chunk we are writing to
    Chunk& currentChunk(); 

public:
    Compiler();
    
    // Notice how compile() now returns a dictionary of ALL functions, not just one chunk
    std::unordered_map<std::string, Function> compile(const std::vector<std::unique_ptr<Stmt>>& ast);
};