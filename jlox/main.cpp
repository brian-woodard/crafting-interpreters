#include <stdio.h>
#include <stdlib.h>
#include <vector>
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

void print_error(int Line, const char* Where, const char* Message)
{
   printf("%s:%d - Error: %s\n", Where, Line, Message);
}

std::vector<T_Token> scan_tokens(char* String, uint32_t Size)
{
   std::vector<T_Token> tokens;

   return tokens;
}

void run(char* String, uint32_t Size)
{
   std::vector<T_Token> tokens = scan_tokens(String, Size);
   printf("Tokens %ld\n", tokens.size());
   for (const auto& token : tokens)
   {
      printf("%.*s", token.Length, token.Lexeme);
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
