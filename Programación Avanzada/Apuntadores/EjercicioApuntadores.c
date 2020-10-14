#include <stdio.h>

int main()
{
    int a, b, c;
    int *p1, *p2;
    void *p;

    p1 = &a;
    *p1 = 1;
    printf("%d\n", &p1); //Direccion de p1
    p2 = &b;
    *p2 = 2;
    printf("%d\n", &p2); //Direccion de p2
    p1 = p2;
    printf("%d\n", &p1); //Direccion de p1 NO TIENE CAMBIOS
    printf("%d\n", *p1); //Valor de p2 se vuelve la de p1
    *p1 = 0;
    p2 = &c;
    *p2 = 3;
    printf("%d\n", &p1); //Direccion de p1 SIN CAMBIOS
    printf("%d\n", *p1); //Valor de p1 CON CAMBIOS
    printf("%d\n", &p2); //Direccion de p2 con NUEVA ASIGNACION
    printf("%d\n", *p2); //Valor de p2 con NUEVA ASIGNACION
    
    p = &p1;
    *(int*)p = p2; //HAY QUE CASTEAR EL VOID A INT
    printf("%d\n", &p);
    printf("%d\n", *(int*)p);
    *p1 = 1;

    return 0;
}