   #include <stdio.h>
   #include <string.h>

   int main(int argc, char *argv[]) {
    //    gcc your_program.c -o your_program
       if (argc == 1) { // number of args
           printf("No arguments provided.\n");
           return 0;
       }

       printf("Program name: %s\n", argv[0]);
       for (int i = 1; i < argc; i++) {
           printf("Argument %d: %s\n", i, argv[i]);
       }
       return 0;
   }