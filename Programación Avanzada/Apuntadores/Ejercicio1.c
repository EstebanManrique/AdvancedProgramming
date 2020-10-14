#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void invertirPalabra(char * palabra)
{   
    for(int i = strlen(palabra); i>0; i--)
    {
        printf("%c", *(palabra+(i -2)));
    }
    return;
}

int main()
{
    char* palabraAVoltear = (char*)malloc(100 * sizeof(char));
    printf("Introduce la palabra a ser volteada.\n");
    fgets(palabraAVoltear, 100, stdin);
    palabraAVoltear = realloc(palabraAVoltear, strlen(palabraAVoltear) * sizeof(char));
    invertirPalabra(palabraAVoltear); 
    free(palabraAVoltear);
    return 0;
}