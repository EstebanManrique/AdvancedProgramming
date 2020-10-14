#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void llenadoArreglo(int* arreglo, int filas, int columnas)
{
    for(int i = 0; i<filas; i++)
    {
        for(int j = 0; j<columnas; j++)
        {
            printf("Favor de introducir el valor de fila %d, columna %d\n", i + 1, j + 1);
            int temporal; 
            scanf("%d", &temporal);
            *(arreglo + i * columnas + j) = temporal;
        }
    }
    return;
}

void impresionArreglo(int* arreglo, int filas, int columnas)
{
    for(int i = 0; i<filas; i++)
    {
        for(int j = 0; j<columnas; j++)
        {
            printf("%d ", *(arreglo + i * columnas + j));
        }
        printf("\n");
    }
}

int main()
{
    int filas;
    int columnas;
    printf("Favor de introducir numero de filas de la matriz.\n");
    scanf("%d", &filas);
    printf("Favor de introducir numero de columnas de la matriz.\n");
    scanf("%d", &columnas);
    int* arreglo = (int*)malloc(filas * columnas * sizeof(int));
    llenadoArreglo(arreglo, filas, columnas);
    impresionArreglo(arreglo, filas, columnas);

    free(arreglo);
    return 0;
}