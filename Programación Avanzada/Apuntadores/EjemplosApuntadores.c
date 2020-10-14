#include <stdio.h>

int main()
{
    int i, j, *p;
    p = &i; //p ahora contiene la direccion de i
    printf("%d \n", &i); //Imprime Direccion de i

    printf("%d \n", &p); //Imprime Direccion de p que apunta a i
    printf("%d \n", *p); //Imprime valor de i en ese momento en memoria (SIN DEFINIR AUN)

    *p = 10; //el valor de i ha sido cambiado a 10
    printf("%d \n", &p); //Imprime Direccion de p que apunta a i
    printf("%d \n", *p); //Imprime valor de i ya definido (10)

    p = &j; //p ahora contiene la direccion de j
    printf("%d \n", &j); //Imprime Direccion de j

    printf("%d \n", &p); //Imprime Direccion de p que apunta a j
    printf("%d \n", *p); //Imprime valor de j en ese momento en memoria (SIN DEFINIR AUN)

    *p = -2; //el valor de j ha sido cambiado a -2
    printf("%d \n", &p); //Imprime Direccion de p que apunta a j
    printf("%d \n", *p); //Imprime valor de j ya definido (-2)


    //ERRORES COMUNES: p = &34 (constantes no tienen direccion), p = &(i+1) (expresiones no tienen direccion), &i = p (direcciones no se pueden cambiar)

    int *a;
    float *b;
    void *c;

    //a = b; NO SE PUEDE HACER ya que son de distintos tipo de dato.
    a = (int*)b; //Se esta casteando un pointer flaot a uno int
    a = c = b; // el pointer void funciona como una especie de comodin


    //ARITMETICA APUNTADORES: Esta PERMITIDO sumar y restar (lo que se suma o resta es la direccion de memoria) PERO NO dividir y multiplicar.

    int *g;
    double *h;
    printf("\n\n%d\n", *g);
    printf("%d\n", *h);

    g++;
    h++;
    printf("%d\n", *g);
    printf("%d\n", *h);

    printf("%d\n", *g - *h);

    return 0;
}