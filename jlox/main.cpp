#include <stdio.h>
#include <vector>
#include "Utility.h"

struct T_Token
{
   char*    Token;
   uint32_t Length;
};

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
      printf("%.*s", token.Length, token.Token);
   }
}

void run_file(char* filename)
{
   TBuffer buffer = ReadEntireFile(filename);

   if (buffer.Data && buffer.Count)
   {
      run((char*)buffer.Data, buffer.Count);
      delete [] buffer.Data;
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
