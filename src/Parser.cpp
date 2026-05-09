#include "Parser.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

// --- Helper Methods ---
Token Parser::peek() const { return tokens[current]; }
Token Parser::previous() const { return tokens[current - 1]; }
bool Parser::isAtEnd() const { return peek().type == TokenType::END_OF_FILE; }
bool Parser::check(TokenType type) const { return !isAtEnd() && peek().type == type; }

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

void Parser::error(const Token& token, const std::string& message) {
    std::cerr << "\n[Line " << token.line << "] Syntax Error";
    if (token.type == TokenType::END_OF_FILE) {
        std::cerr << " at end of file: " << message << "\n";
    } else {
        std::cerr << " at '" << token.lexeme << "': " << message << "\n";
    }
    exit(1); 
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    error(peek(), message);
    return peek(); 
}

// --- NEW PHASE 6: Block Parsing ---
std::vector<std::unique_ptr<Stmt>> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expected '}' after block.");
    return statements;
}

// --- NEW PHASE 6: Function Definition ---
std::unique_ptr<Stmt> Parser::functionDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expected function name.");
    consume(TokenType::LEFT_PAREN, "Expected '(' after function name.");
    
    std::vector<Token> parameters;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            parameters.push_back(consume(TokenType::IDENTIFIER, "Expected parameter name."));
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RIGHT_PAREN, "Expected ')' after parameters.");
    consume(TokenType::LEFT_BRACE, "Expected '{' before function body.");
    
    auto body = std::make_unique<BlockStmt>(block());
    return std::make_unique<FunctionStmt>(name, std::move(parameters), std::move(body));
}

// --- Grammar Rules (Recursive Descent) ---
std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

std::unique_ptr<Stmt> Parser::declaration() {
    // Check for our new Phase 6 rules first
    if (match({TokenType::DEF})) return functionDeclaration();
    if (match({TokenType::LET})) return letDeclaration();
    
    if (match({TokenType::LEFT_BRACE})) {
        return std::make_unique<BlockStmt>(block());
    }
    
    if (match({TokenType::PRINT})) {
        std::unique_ptr<Expr> value = expression();
        return std::make_unique<PrintStmt>(std::move(value));
    }
    
    if (match({TokenType::RETURN})) {
        std::unique_ptr<Expr> value = nullptr;
        if (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            value = expression();
        }
        return std::make_unique<ReturnStmt>(std::move(value));
    }
    
    if (match({TokenType::IF})) {
        std::unique_ptr<Expr> condition = expression();
        std::unique_ptr<Stmt> thenBranch = declaration(); 
        return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch));
    }
    
    if (match({TokenType::WHILE})) {
        std::unique_ptr<Expr> condition = expression();
        std::unique_ptr<Stmt> body = declaration();
        return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
    }

    auto expr = expression();
    return nullptr; 
}

std::unique_ptr<Stmt> Parser::letDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expected variable name after 'let'.");
    consume(TokenType::EQUAL, "Expected '=' after variable name.");
    
    std::unique_ptr<Expr> initializer = expression();
    return std::make_unique<LetStmt>(name, std::move(initializer));
}

std::unique_ptr<Expr> Parser::expression() {
    auto expr = term(); 
    while (match({TokenType::LESS, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        auto right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    auto expr = factor();
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        Token op = previous();
        auto right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    auto expr = primary();
    while (match({TokenType::STAR, TokenType::SLASH})) {
        Token op = previous();
        auto right = primary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({TokenType::NUMBER, TokenType::BOOLEAN})) {
        return std::make_unique<LiteralExpr>(previous());
    }
    
    if (match({TokenType::IDENTIFIER})) {
        Token name = previous();
        
        // Check if it's a function call like multiply(10, 5)
        if (match({TokenType::LEFT_PAREN})) {
            std::vector<std::unique_ptr<Expr>> arguments;
            if (!check(TokenType::RIGHT_PAREN)) {
                do {
                    arguments.push_back(expression());
                } while (match({TokenType::COMMA}));
            }
            consume(TokenType::RIGHT_PAREN, "Expected ')' after arguments.");
            return std::make_unique<CallExpr>(std::make_unique<VariableExpr>(name), std::move(arguments));
        }
        return std::make_unique<VariableExpr>(name);
    }
    
    if (match({TokenType::INPUT})) {
        return std::make_unique<InputExpr>();
    }
    
    error(peek(), "Expected expression.");
    return nullptr; 
}