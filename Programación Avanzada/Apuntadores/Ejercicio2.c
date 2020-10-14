#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int longitudPalabra(char* palabra)
{
    int longitud = 0;
    while(*(palabra + longitud) != NULL)   //Esto se puede poner (*palabra++ != '\0')
    {
        longitud++;
    }
    return longitud;
}

int longitudAA(char* palabra)
{
    char *p = palabra;
    while (*p++ != '\0'){};
    return(p - palabra - 1);
    
}

int main()
{
    char* palabraAAnalizar = (char*)malloc(100 * sizeof(char));
    printf("Introduce la palabra a la que se le quiere sacar la longitud.\n");
    scanf(" %[^\n]", palabraAAnalizar);
    palabraAAnalizar = realloc(palabraAAnalizar, strlen(palabraAAnalizar) * sizeof(char));
    printf("La longitud de la palabra %s es %d.\n", palabraAAnalizar, longitudPalabra(palabraAAnalizar));
    printf("La longitud de la palabra %s es %d.\n", palabraAAnalizar, longitudAA(palabraAAnalizar));
    free(palabraAAnalizar);
    return 0;
}