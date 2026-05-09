#pragma once
#include "Token.h"
#include <memory>
#include <string>
#include <vector>
#include <iostream>

// Base class for all nodes in our tree
struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0; // For debugging
};

// --- Expressions (Things that produce a value, like 10 + 5) ---
struct Expr : public ASTNode {};

struct LiteralExpr : public Expr {
    Token value;
    LiteralExpr(Token val) : value(val) {}
    
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "- Literal: " << value.lexeme << "\n";
    }
};

struct BinaryExpr : public Expr {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
    
    BinaryExpr(std::unique_ptr<Expr> l, Token o, std::unique_ptr<Expr> r) 
        : left(std::move(l)), op(o), right(std::move(r)) {}
        
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "- Binary Op: '" << op.lexeme << "'\n";
        left->print(indent + 4);
        right->print(indent + 4);
    }
};
struct InputExpr : public Expr {
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "- [Input Fetch]\n";
    }
};

// --- Statements (Things that perform an action, like let x = ...) ---
struct Stmt : public ASTNode {};

struct LetStmt : public Stmt {
    Token name;
    std::unique_ptr<Expr> initializer;
    
    LetStmt(Token n, std::unique_ptr<Expr> init) 
        : name(n), initializer(std::move(init)) {}
        
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "[Statement] Let '" << name.lexeme << "' =\n";
        if (initializer) initializer->print(indent + 4);
    }
};

// Add a new Expression for variables
struct VariableExpr : public Expr {
    Token name;
    VariableExpr(Token n) : name(n) {}
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "- Variable: " << name.lexeme << "\n";
    }
};

// Add a new Statement for Print
struct PrintStmt : public Stmt {
    std::unique_ptr<Expr> expression;
    PrintStmt(std::unique_ptr<Expr> expr) : expression(std::move(expr)) {}
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "[Statement] Print:\n";
        if (expression) expression->print(indent + 4);
    }
};
// Add these under your Statements:
struct IfStmt : public Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    
    IfStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> thenB) 
        : condition(std::move(cond)), thenBranch(std::move(thenB)) {}
        
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "[If Statement]\n";
        condition->print(indent + 4);
        thenBranch->print(indent + 4);
    }
};

struct WhileStmt : public Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
    
    WhileStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> b) 
        : condition(std::move(cond)), body(std::move(b)) {}
        
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "[While Loop]\n";
        condition->print(indent + 4);
        body->print(indent + 4);
    }
};

struct CallExpr : public Expr {
    std::unique_ptr<Expr> callee; // The function being called
    std::vector<std::unique_ptr<Expr>> arguments; // The list of arguments
    
    CallExpr(std::unique_ptr<Expr> c, std::vector<std::unique_ptr<Expr>> args) 
        : callee(std::move(c)), arguments(std::move(args)) {}
        
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "- Function Call\n";
    }
};

// 1. The Block Statement (for { })
struct BlockStmt : public Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;
    
    BlockStmt(std::vector<std::unique_ptr<Stmt>> stmts) 
        : statements(std::move(stmts)) {}
        
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "[Block Start]\n";
        for (const auto& stmt : statements) {
            if (stmt) stmt->print(indent + 4);
        }
        std::cout << std::string(indent, ' ') << "[Block End]\n";
    }
};

// 2. The Function Definition Statement
struct FunctionStmt : public Stmt {
    Token name;
    std::vector<Token> params;
    std::unique_ptr<Stmt> body; 
    
    FunctionStmt(Token n, std::vector<Token> p, std::unique_ptr<Stmt> b) 
        : name(n), params(std::move(p)), body(std::move(b)) {}
        
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "[Function Definition] " << name.lexeme << "\n";
        body->print(indent + 4);
    }
};

// 3. The Return Statement
struct ReturnStmt : public Stmt {
    std::unique_ptr<Expr> value;
    
    ReturnStmt(std::unique_ptr<Expr> val) : value(std::move(val)) {}
        
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "[Return Statement]\n";
        if (value) value->print(indent + 4);
    }
};