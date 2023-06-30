#include <stdio.h>
#include <stdlib.h>

int main()
{
   FILE *in_file = fopen("source.txt","r");;
   char *contents = (char *)calloc(2048, sizeof(char));
    fgets(contents, 2048, in_file);
   fclose(in_file);

   FILE *out_file = out_file = fopen("c_out.txt","w");
   fprintf(out_file,"%s",contents);
   fclose(out_file);

   return 0;
}