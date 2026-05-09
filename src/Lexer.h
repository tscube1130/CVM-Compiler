#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Token.h"

class Lexer {
private:
    std::string source;
    int start = 0;
    int current = 0;
    int line = 1;

    // A map to quickly look up if a word is a keyword
    std::unordered_map<std::string, TokenType> keywords;

    bool isAtEnd() const;
    char advance();
    char peek() const;
    void addToken(TokenType type, std::vector<Token>& tokens);

    // Helpers for multi-character tokens
    void number(std::vector<Token>& tokens);
    void identifier(std::vector<Token>& tokens);

public:
    Lexer(const std::string& sourceCode);
    std::vector<Token> scanTokens();
};