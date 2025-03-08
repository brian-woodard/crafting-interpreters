#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <ctype.h>
#include <unordered_map>
#include <string>
#include "Utility.h"

enum eTokenType
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

struct T_Token
{
   eTokenType Type;
   char*      Lexeme;
   uint32_t   Length;
   uint32_t   Line;
};

bool had_error = false;
std::unordered_map<std::string, eTokenType> keywords;

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
            Tokens.push_back({ LEFT_PAREN, &String[current], 1, line });
            break;
         case ')':
            Tokens.push_back({ RIGHT_PAREN, &String[current], 1, line });
            break;
         case '{':
            Tokens.push_back({ LEFT_BRACE, &String[current], 1, line });
            break;
         case '}':
            Tokens.push_back({ RIGHT_BRACE, &String[current], 1, line });
            break;
         case ',':
            Tokens.push_back({ COMMA, &String[current], 1, line });
            break;
         case '.':
            Tokens.push_back({ DOT, &String[current], 1, line });
            break;
         case '-':
            Tokens.push_back({ MINUS, &String[current], 1, line });
            break;
         case '+':
            Tokens.push_back({ PLUS, &String[current], 1, line });
            break;
         case ';':
            Tokens.push_back({ SEMICOLON, &String[current], 1, line });
            break;
         case '*':
            Tokens.push_back({ STAR, &String[current], 1, line });
            break;
         case '!':
            if (String[current+1] == '=')
            {
               Tokens.push_back({ BANG_EQUAL, &String[current], 2, line });
               current++;
            }
            else
            {
               Tokens.push_back({ BANG, &String[current], 1, line });
            }
            break;
         case '=':
            if (String[current+1] == '=')
            {
               Tokens.push_back({ EQUAL_EQUAL, &String[current], 2, line });
               current++;
            }
            else
            {
               Tokens.push_back({ EQUAL, &String[current], 1, line });
            }
            break;
         case '<':
            if (String[current+1] == '=')
            {
               Tokens.push_back({ LESS_EQUAL, &String[current], 2, line });
               current++;
            }
            else
            {
               Tokens.push_back({ LESS, &String[current], 1, line });
            }
            break;
         case '>':
            if (String[current+1] == '=')
            {
               Tokens.push_back({ GREATER_EQUAL, &String[current], 2, line });
               current++;
            }
            else
            {
               Tokens.push_back({ GREATER, &String[current], 1, line });
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
               Tokens.push_back({ SLASH, &String[current], 1, line });
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
               Tokens.push_back({ STRING, &String[start], current - start, line });
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

               Tokens.push_back({ NUMBER, &String[start], current - start, line });
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
               eTokenType type = IDENTIFIER;

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

   Tokens.push_back({ END_OF_FILE, nullptr, 0, line });
}

void run(char* String, uint32_t Size)
{
   std::vector<T_Token> tokens;

   scan_tokens(String, Size, tokens);

   printf("Tokens %ld\n", tokens.size());
   for (const auto& token : tokens)
   {
      printf("Type %d: %.*s\n", token.Type, token.Length, token.Lexeme);
   }
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
