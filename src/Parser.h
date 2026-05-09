#pragma once
#include "Token.h"
#include "AST.h"
#include <vector>
#include <stdexcept>
#include <iostream>

class Parser {
private:
    std::vector<Token> tokens;
    int current = 0;

    // Helper tools
    Token peek() const;
    Token previous() const;
    bool isAtEnd() const;
    bool check(TokenType type) const;
    Token advance();
    bool match(std::initializer_list<TokenType> types);
    Token consume(TokenType type, const std::string& message);
    void error(const Token& token, const std::string& message);

    // --- NEW PHASE 6 STRUCTURES ---
    std::vector<std::unique_ptr<Stmt>> block();
    std::unique_ptr<Stmt> functionDeclaration();

    // Grammar rules
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> letDeclaration();
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> primary();

public:
    Parser(const std::vector<Token>& tokens);
    std::vector<std::unique_ptr<Stmt>> parse();
};