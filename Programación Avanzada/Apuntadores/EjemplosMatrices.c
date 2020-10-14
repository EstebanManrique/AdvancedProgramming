#include <stdio.h>

int main()
{
    int vector[10];
    int matriz[3][5];
    int *p;

    p = &vector[0];
    printf("%d\n", *(p+2)); //Se imprime la posicion 2 del arreglo

    p = &matriz[0][0];
    printf("%d\n", *(p+2)); //Se imprime la posicion [0][2]
    printf("%d\n", *(p+5)); //Se imprime la posicion [1][0]
    printf("%d\n", *(p+12)); //Se imprime la posicion [2][2]

    //Si se tiene double vector[10], vector es EQUIVALENTE a vector[0]
    // *(p+1) = vector[1] = *(vector+1) = p[1]

    int m[5][3], **p, *q;
    // m = &m[0]
    // m[0] = &m[0][0]
    // m[1] = &m[1][0]
    
    //Si **p = m -> *p = m[0], *(p+1) = m[1], **p = m[0][0], **(p+1) = m[1][0] y *(*(p+1)+1) = m[1][1]

    //Si q = &matriz[0][0] -> *(matriz[i]+j) = (*(mat+i)[j]) = *(*(matriz+i) + j)

    return 0;
}