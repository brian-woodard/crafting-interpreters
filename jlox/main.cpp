#include <stdio.h>

void run_file(char* filename)
{
   printf("Run file %s\n", filename);
}

void run_prompt()
{
   printf("Run prompt\n");
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
