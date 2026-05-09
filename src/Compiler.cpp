#include "Compiler.h"
#include <string>

Compiler::Compiler() {
    mainFunction.name = "main";
    currentFunction = &mainFunction;
}

Chunk& Compiler::currentChunk() {
    return currentFunction->chunk;
}

std::unordered_map<std::string, Function> Compiler::compile(const std::vector<std::unique_ptr<Stmt>>& ast) {
    for (const auto& stmt : ast) {
        if (stmt) {
            compileStatement(stmt.get());
        }
    }
    currentChunk().write(static_cast<uint8_t>(Opcode::HALT)); 
    
    // Package the main script into our dictionary and return the whole thing
    userFunctions["main"] = mainFunction;
    return userFunctions;
}

void Compiler::compileStatement(const Stmt* stmt) {
    if (const LetStmt* letStmt = dynamic_cast<const LetStmt*>(stmt)) {
        if (letStmt->initializer) compileExpression(letStmt->initializer.get());
        uint8_t nameIndex = currentChunk().addIdentifier(letStmt->name.lexeme);
        currentChunk().write(static_cast<uint8_t>(Opcode::DEFINE_VAR));
        currentChunk().write(nameIndex); 
    } 
    else if (const PrintStmt* printStmt = dynamic_cast<const PrintStmt*>(stmt)) {
        compileExpression(printStmt->expression.get());
        currentChunk().write(static_cast<uint8_t>(Opcode::PRINT));
    }
    else if (const IfStmt* ifStmt = dynamic_cast<const IfStmt*>(stmt)) {
        compileExpression(ifStmt->condition.get());
        currentChunk().write(static_cast<uint8_t>(Opcode::JUMP_IF_FALSE));
        int jumpOffsetIndex = currentChunk().code.size(); 
        currentChunk().write(0); 
        
        compileStatement(ifStmt->thenBranch.get()); 
        
        int distance = currentChunk().code.size() - jumpOffsetIndex - 1;
        currentChunk().code[jumpOffsetIndex] = static_cast<uint8_t>(distance);
    }
    else if (const WhileStmt* whileStmt = dynamic_cast<const WhileStmt*>(stmt)) {
        int loopStart = currentChunk().code.size(); 
        compileExpression(whileStmt->condition.get());
        
        currentChunk().write(static_cast<uint8_t>(Opcode::JUMP_IF_FALSE));
        int exitJumpIndex = currentChunk().code.size();
        currentChunk().write(0); 
        
        compileStatement(whileStmt->body.get());
        
        currentChunk().write(static_cast<uint8_t>(Opcode::LOOP));
        int loopDistance = currentChunk().code.size() - loopStart + 1;
        currentChunk().write(static_cast<uint8_t>(loopDistance));
        
        int exitDistance = currentChunk().code.size() - exitJumpIndex - 1;
        currentChunk().code[exitJumpIndex] = static_cast<uint8_t>(exitDistance);
    }
    
    // --- NEW PHASE 6: Block Statements ---
    else if (const BlockStmt* blockStmt = dynamic_cast<const BlockStmt*>(stmt)) {
        for (const auto& innerStmt : blockStmt->statements) {
            compileStatement(innerStmt.get());
        }
    }
    
    // --- NEW PHASE 6: Return Statements ---
    else if (const ReturnStmt* returnStmt = dynamic_cast<const ReturnStmt*>(stmt)) {
        if (returnStmt->value) {
            compileExpression(returnStmt->value.get());
        } else {
            // If they just type "return" with no math, safely return a 0
            currentChunk().write(static_cast<uint8_t>(Opcode::PUSH_CONST));
            uint8_t zeroIndex = currentChunk().addConstant(0.0);
            currentChunk().write(zeroIndex);
        }
        currentChunk().write(static_cast<uint8_t>(Opcode::RETURN));
    }
    
    // --- NEW PHASE 6: Function Definitions ---
    else if (const FunctionStmt* funcStmt = dynamic_cast<const FunctionStmt*>(stmt)) {
        Function newFunc;
        newFunc.name = funcStmt->name.lexeme;
        newFunc.arity = funcStmt->params.size();

        // 1. Save our place in the main script
        Function* previousFunction = currentFunction;

        // 2. Switch the compiler's brain over to the new function
        userFunctions[newFunc.name] = newFunc;
        currentFunction = &userFunctions[newFunc.name];

        // 3. Register the parameters so the VM knows they exist
        for (const Token& param : funcStmt->params) {
            currentChunk().addIdentifier(param.lexeme);
        }

        // 4. Compile the body { ... }
        compileStatement(funcStmt->body.get());

        // 5. Add a hidden "return 0" at the very end just in case the user forgot to write 'return'
        currentChunk().write(static_cast<uint8_t>(Opcode::PUSH_CONST));
        uint8_t zeroIndex = currentChunk().addConstant(0.0);
        currentChunk().write(zeroIndex);
        currentChunk().write(static_cast<uint8_t>(Opcode::RETURN));

        // 6. Switch the compiler back to the main script!
        currentFunction = previousFunction;
    }
}

void Compiler::compileExpression(const Expr* expr) {
    if (const LiteralExpr* literal = dynamic_cast<const LiteralExpr*>(expr)) {
        double val = std::stod(literal->value.lexeme); 
        uint8_t index = currentChunk().addConstant(val); 
        currentChunk().write(static_cast<uint8_t>(Opcode::PUSH_CONST));
        currentChunk().write(index);
    } 
    else if (const VariableExpr* varExpr = dynamic_cast<const VariableExpr*>(expr)) {
        uint8_t nameIndex = currentChunk().addIdentifier(varExpr->name.lexeme);
        currentChunk().write(static_cast<uint8_t>(Opcode::GET_VAR));
        currentChunk().write(nameIndex);
    }
    else if (dynamic_cast<const InputExpr*>(expr)) {
        currentChunk().write(static_cast<uint8_t>(Opcode::INPUT_OP));
    }
    
    // --- NEW PHASE 6: Function Calls ---
    else if (const CallExpr* callExpr = dynamic_cast<const CallExpr*>(expr)) {
        // First, compile all the arguments so they get pushed onto the stack
        for (const auto& arg : callExpr->arguments) {
            compileExpression(arg.get());
        }
        
        // Then, emit the CALL instruction with the function's name
        if (const VariableExpr* varExpr = dynamic_cast<const VariableExpr*>(callExpr->callee.get())) {
            uint8_t nameIndex = currentChunk().addIdentifier(varExpr->name.lexeme);
            currentChunk().write(static_cast<uint8_t>(Opcode::CALL));
            currentChunk().write(nameIndex);
        }
    }
    
    else if (const BinaryExpr* binary = dynamic_cast<const BinaryExpr*>(expr)) {
        compileExpression(binary->left.get());
        compileExpression(binary->right.get());
        
        switch (binary->op.type) {
            case TokenType::PLUS: currentChunk().write(static_cast<uint8_t>(Opcode::ADD)); break;
            case TokenType::MINUS: currentChunk().write(static_cast<uint8_t>(Opcode::SUBTRACT)); break;
            case TokenType::STAR: currentChunk().write(static_cast<uint8_t>(Opcode::MULTIPLY)); break;
            case TokenType::SLASH: currentChunk().write(static_cast<uint8_t>(Opcode::DIVIDE)); break;
            case TokenType::LESS: currentChunk().write(static_cast<uint8_t>(Opcode::LESS)); break;
            case TokenType::EQUAL_EQUAL: currentChunk().write(static_cast<uint8_t>(Opcode::EQUAL_COMPARE)); break;
            default: break;
        }
    }
}