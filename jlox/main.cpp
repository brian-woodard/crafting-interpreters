#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <ctype.h>
#include <unordered_map>
#include <string>
#include "Utility.h"

enum TokenType
{
   // Single-character tokens
   LEFT_PAREN,
   RIGHT_PAREN,
   LEFT_BRACE,
   RIGHT_BRACE,
   COMMA,
   DOT,
   MINUS,
   PLUS,
   SEMICOLON,
   SLASH,
   STAR,

   // One or two character tokens
   BANG,
   BANG_EQUAL,
   EQUAL,
   EQUAL_EQUAL,
   GREATER,
   GREATER_EQUAL,
   LESS,
   LESS_EQUAL,

   // Literals
   IDENTIFIER,
   STRING,
   NUMBER,

   // Keywords
   AND,
   CLASS,
   ELSE,
   FALSE,
   FUN,
   FOR,
   IF,
   NIL,
   OR,
   PRINT,
   RETURN,
   SUPER,
   THIS,
   TRUE,
   VAR,
   WHILE,

   END_OF_FILE,
};

const char* TokenTypeStr[END_OF_FILE + 1] =
{
   "LEFT_PAREN",
   "RIGHT_PAREN",
   "LEFT_BRACE",
   "RIGHT_BRACE",
   "COMMA",
   "DOT",
   "MINUS",
   "PLUS",
   "SEMICOLON",
   "SLASH",
   "STAR",
   "BANG",
   "BANG_EQUAL",
   "EQUAL",
   "EQUAL_EQUAL",
   "GREATER",
   "GREATER_EQUAL",
   "LESS",
   "LESS_EQUAL",
   "IDENTIFIER",
   "STRING",
   "NUMBER",
   "AND",
   "CLASS",
   "ELSE",
   "FALSE",
   "FUN",
   "FOR",
   "IF",
   "NIL",
   "OR",
   "PRINT",
   "RETURN",
   "SUPER",
   "THIS",
   "TRUE",
   "VAR",
   "WHILE",
   "END_OF_FILE",
};

struct T_Token
{
   TokenType Type;
   char*     Lexeme;
   uint32_t  Length;
   uint32_t  Line;
};

enum class ExprTypes
{
   Binary,
   Grouping,
   Literal,
   Unary,
   Error,
};

const char* ExprTypeStr[(int)ExprTypes::Error + 1] =
{
   "Binary",
   "Grouping",
   "Literal",
   "Unary",
   "Error",
};

struct T_Expr
{
   ExprTypes Type;
   void*     Expr;
   //union
   //{
   //   T_BinaryExpr   Binary;
   //   T_GroupingExpr Grouping;
   //   T_LiteralExpr  Literal;
   //   T_UnaryExpr    Unary;
   //};
};

struct T_BinaryExpr
{
   T_Expr  Left;
   T_Expr  Right;
   T_Token Operator;
};

struct T_GroupingExpr
{
   T_Expr Expression;
};

struct T_LiteralExpr
{
   T_Token Value;
};

struct T_UnaryExpr
{
   T_Expr  Right;
   T_Token Operator;
};

struct T_ErrorExpr
{
   T_Token Error;
};

bool had_error = false;
std::unordered_map<std::string, TokenType> keywords;

void print_error(const char* Message, int Line)
{
   printf("Error: %s at Line %d\n", Message, Line);
   had_error = true;
}

void scan_tokens(char* String, uint32_t Size, std::vector<T_Token>& Tokens)
{
   std::vector<T_Token> tokens;
   uint32_t             start = 0;
   uint32_t             current = 0;
   uint32_t             line = 1;

   while (current < Size)
   {
      switch (String[current])
      {
         case '(':
            Tokens.push_back({ TokenType::LEFT_PAREN, &String[current], 1, line });
            break;
         case ')':
            Tokens.push_back({ TokenType::RIGHT_PAREN, &String[current], 1, line });
            break;
         case '{':
            Tokens.push_back({ TokenType::LEFT_BRACE, &String[current], 1, line });
            break;
         case '}':
            Tokens.push_back({ TokenType::RIGHT_BRACE, &String[current], 1, line });
            break;
         case ',':
            Tokens.push_back({ TokenType::COMMA, &String[current], 1, line });
            break;
         case '.':
            Tokens.push_back({ TokenType::DOT, &String[current], 1, line });
            break;
         case '-':
            Tokens.push_back({ TokenType::MINUS, &String[current], 1, line });
            break;
         case '+':
            Tokens.push_back({ TokenType::PLUS, &String[current], 1, line });
            break;
         case ';':
            Tokens.push_back({ TokenType::SEMICOLON, &String[current], 1, line });
            break;
         case '*':
            Tokens.push_back({ TokenType::STAR, &String[current], 1, line });
            break;
         case '!':
            if (String[current+1] == '=')
            {
               Tokens.push_back({ TokenType::BANG_EQUAL, &String[current], 2, line });
               current++;
            }
            else
            {
               Tokens.push_back({ TokenType::BANG, &String[current], 1, line });
            }
            break;
         case '=':
            if (String[current+1] == '=')
            {
               Tokens.push_back({ TokenType::EQUAL_EQUAL, &String[current], 2, line });
               current++;
            }
            else
            {
               Tokens.push_back({ TokenType::EQUAL, &String[current], 1, line });
            }
            break;
         case '<':
            if (String[current+1] == '=')
            {
               Tokens.push_back({ TokenType::LESS_EQUAL, &String[current], 2, line });
               current++;
            }
            else
            {
               Tokens.push_back({ TokenType::LESS, &String[current], 1, line });
            }
            break;
         case '>':
            if (String[current+1] == '=')
            {
               Tokens.push_back({ TokenType::GREATER_EQUAL, &String[current], 2, line });
               current++;
            }
            else
            {
               Tokens.push_back({ TokenType::GREATER, &String[current], 1, line });
            }
            break;
         case '/':
            if (String[current+1] == '/')
            {
               while (current != '\n' && current < Size)
                  current++;
            }
            else
            {
               Tokens.push_back({ TokenType::SLASH, &String[current], 1, line });
            }
            break;
         case '"':
            current++;
            start = current;
            while (String[current] != '"' && current < Size)
            {
               if (current == '\n')
                  line++;

               current++;
            }

            if (current == Size)
               print_error("Unterminated string", line);
            else
            {
               Tokens.push_back({ TokenType::STRING, &String[start], current - start, line });
            }
            break;
         case ' ':
         case '\t':
         case '\r':
            // ignore whitespace
            break;
         case '\n':
            line++;
            break;
         default:
            if (isdigit(String[current]))
            {
               start = current++;
               while ((isdigit(String[current]) || (String[current] == '.' && isdigit(String[current+1]))) && current < Size)
               {
                  current++;
               }

               Tokens.push_back({ TokenType::NUMBER, &String[start], current - start, line });
               current--;
            }
            else if (isalpha(String[current]))
            {
               start = current++;
               while (isalnum(String[current]) && current < Size)
               {
                  current++;
               }

               std::string token(&String[start], current - start);
               TokenType type = TokenType::IDENTIFIER;

               if (keywords.find(token) != keywords.end())
                  type = keywords[token];

               Tokens.push_back({ type, &String[start], current - start, line });
               current--;
            }
            else
            {
               print_error("Unexpected character", line);
            }
      }

      current++;
   }

   Tokens.push_back({ TokenType::END_OF_FILE, nullptr, 0, line });
}

///////////////////////////////////////////////////////////////////////////////
// Parsing functions

T_Expr parse_primary(const std::vector<T_Token>& Tokens, int& Current)
{
   T_Expr expr = {};

   if (Tokens[Current].Type == FALSE ||
       Tokens[Current].Type == TRUE ||
       Tokens[Current].Type == NIL ||
       Tokens[Current].Type == NUMBER ||
       Tokens[Current].Type == STRING)
   {
      T_LiteralExpr* literal = new T_LiteralExpr;

      literal->Value = Tokens[Current++];

      expr.Type = ExprTypes::Literal;
      expr.Expr = literal;
   }

   return expr;
}

T_Expr parse_unary(const std::vector<T_Token>& Tokens, int& Current)
{
   // if (match(BANG, MINUS)) {
   //   Token operator = previous();
   //   Expr right = unary();
   //   return new Expr.Unary(operator, right);
   // }

   return parse_primary(Tokens, Current);
}

T_Expr parse_factor(const std::vector<T_Token>& Tokens, int& Current)
{
   T_Expr expr = {};

   expr = parse_unary(Tokens, Current);

   // while (match(SLASH, STAR)) {
   //   Token operator = previous();
   //   Expr right = unary();
   //   expr = new Expr.Binary(expr, operator, right);
   // }

   return expr;
}

T_Expr parse_term(const std::vector<T_Token>& Tokens, int& Current)
{
   T_Expr expr = {};

   expr = parse_factor(Tokens, Current);

   // while (match(MINUS, PLUS)) {
   //   Token operator = previous();
   //   Expr right = factor();
   //   expr = new Expr.Binary(expr, operator, right);
   // }

   return expr;
}

T_Expr parse_comparison(const std::vector<T_Token>& Tokens, int& Current)
{
   T_Expr expr = {};

   expr = parse_term(Tokens, Current);

   // while (match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
   //   Token operator = previous();
   //   Expr right = term();
   //   expr = new Expr.Binary(expr, operator, right);
   // }

   return expr;
}

T_Expr parse_equality(const std::vector<T_Token>& Tokens, int& Current)
{
   T_Expr expr = {};

   expr = parse_comparison(Tokens, Current);

   // while (match(BANG_EQUAL, EQUAL_EQUAL)) {
   //   Token operator = previous();
   //   Expr right = comparison();
   //   expr = new Expr.Binary(expr, operator, right);
   // }

   return expr;
}

T_Expr parse_expression(const std::vector<T_Token>& Tokens, int& Current)
{
   return parse_equality(Tokens, Current);
}

T_Expr parse_tokens(const std::vector<T_Token>& Tokens)
{
   int current = 0;
   return parse_expression(Tokens, current);
}

// Parsing functions
///////////////////////////////////////////////////////////////////////////////

void print_ast(T_Expr Expr)
{
   printf("Expr Type %s (%d), Expr %p\n", ExprTypeStr[(int)Expr.Type], Expr.Type, Expr.Expr);
}

void run(char* String, uint32_t Size)
{
   std::vector<T_Token> tokens;

   printf("Scanning\n");
   scan_tokens(String, Size, tokens);

   printf("Tokens %ld\n", tokens.size());
   for (const auto& token : tokens)
   {
      printf("Type %s (%d): %.*s\n", TokenTypeStr[token.Type], token.Type, token.Length, token.Lexeme);
   }

   printf("\nParsing\n");
   T_Expr expr = parse_tokens(tokens);

   if (had_error)
      return;

   // print AST tree
   print_ast(expr);
}

void run_file(const char* Filename)
{
   TBuffer buffer = ReadEntireFile(Filename);

   if (buffer.Data && buffer.Count)
   {
      run((char*)buffer.Data, buffer.Count);
      delete [] buffer.Data;

      if (had_error) exit(65);
   }
}

void run_prompt()
{
   TBuffer buffer;

   buffer.Data = new uint8_t[4096];
   buffer.Count = 0;

   while (1)
   {
      printf("> ");

      buffer.Data = (uint8_t*) fgets((char*)buffer.Data, 4096, stdin);

      if (buffer.Data && strncasecmp((char*)buffer.Data, "quit", strlen("quit")) != 0)
      {
         buffer.Count = strlen((char*)buffer.Data);
         run((char*)buffer.Data, buffer.Count);
         had_error = false;
      }
      else
      {
         break;
      }
   }
}

int main(int argc, char* argv[])
{
   keywords["and"] = AND;
   keywords["class"] = CLASS;
   keywords["else"] = ELSE;
   keywords["false"] = FALSE;
   keywords["for"] = FOR;
   keywords["fun"] = FUN;
   keywords["if"] = IF;
   keywords["nil"] = NIL;
   keywords["or"] = OR;
   keywords["print"] = PRINT;
   keywords["return"] = RETURN;
   keywords["super"] = SUPER;
   keywords["this"] = THIS;
   keywords["true"] = TRUE;
   keywords["var"] = VAR;
   keywords["while"] = WHILE;

   if (argc > 2)
   {
      printf("Usage: jlox [script]\n");
      return 1;
   }
   else if (argc == 2)
   {
      run_file(argv[1]);
   }
   else
   {
      run_prompt();
   }

   return 1;
}
