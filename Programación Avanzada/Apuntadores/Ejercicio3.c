#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void CopiarString(char* palabraOriginal, char* seCopiaAqui)
{
    for(int i = 0; i<strlen(palabraOriginal) - 1; i++)
    {
        seCopiaAqui[i] = &palabraOriginal[i];
        *(seCopiaAqui + (i)) = palabraOriginal[i];

        //while((*seCopiaAqui++ = *palabraOriginal++) != '\n'){};
    }
    printf("%s\n", seCopiaAqui);
}

int main()
{
    char* palabraOriginal = (char*)malloc(100 * sizeof(char));
    char* seCopiaAqui = (char*)malloc(100 * sizeof(char));
    printf("Introducir la palabra con la que se va a trabajar.\n");
    fgets(palabraOriginal, 100, stdin);
    palabraOriginal = realloc(palabraOriginal, strlen(palabraOriginal) * sizeof(char));
    seCopiaAqui = realloc(seCopiaAqui, strlen(palabraOriginal) * sizeof(char));
    CopiarString(palabraOriginal, seCopiaAqui);
    
    free(palabraOriginal);
    free(seCopiaAqui);
    return  0;
}