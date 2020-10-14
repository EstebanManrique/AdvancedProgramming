#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

struct Libro //Estructura Libro planteada en problema
{
    char* titulo;
    int paginas;
};

typedef void(*t_opcion_menu)(); //Usado para Menu Principal
typedef void*(*fun_iterador)(void*); //Usado para manjera end, next y prev de Libros y numeros enteros
typedef void(*impresion)(void*); //Usado para manejo de modos de impresion de acuerdo a tipo de dato
typedef void(*iterador)(void*, void*, impresion); //Usado para modelar iteradores a partir de funcion recorrigoGenerico
fun_iterador END, NEXT, PREV;

//Funciones de generacion de elementos
void generacionNumerosAleatoriosInt(int);
void generacionLibros(int);

//Funcion para obtener elemento inicial de arreglos
void* begin(void*);

//Funciones especificas para numeros enteros. Se planteo similar a codigo de ordenamiento generico con las funciones ascInt, descInt y swapInt
void* prev(void*);
void* end(void*);
void* next(void*);

//Funciones especificas para Libros. Se planteo similar a codigo de ordenamiento generico con las funciones ascInt, descInt y swapInt
void* prevLibro(void*);
void* endLibro(void*);
void* nextLibro(void*);

//Funciones para impresion de acuerdo a tipo de dato siendo recorrido
void impresionInt(void*);
void impresionLibro(void*);

//Recorrido GENERICO y funciones de los diferentes iteradores
void recorridogenerico(void*, iterador); 
void forward(void*, void*, impresion); 
void reverse(void*, void*, impresion); 
void bidirectional(void*, void*, impresion);

//Funciones para manejo de menu y submenus de usuario
void menu(); //LISTA
void generaMenu(); //LISTA
void recorridoEImpresionMenu();
void recorridoEImpresionMenu2(void*);

//Liberaicon de memoria en cada estructura Libro (se libera el nombre del Libro)
void liberacionLibros(struct Libro*, int);

struct Libro* arregloLibros;
int* arregloNumeros;

int main()
{  
    srand(time(0)); //Necesario para numeros aleatorios
    menu();
    return 0;
}
void generacionNumerosAleatoriosInt(int size) //Recibe la longitud deseada del arreglo de Enteros, previamente introducida por el usuario
{
    arregloNumeros = (int*)malloc((size + 1) * sizeof(int)); //Alocacion de memoria
    int* aux = begin(arregloNumeros); //Inicio del arreglo
    printf("El arreglo generado es:\n");
    for(aux; aux<(arregloNumeros + size); aux++) //Llenado de arreglo con aritmetica de apuntadores
    {
        *aux = (rand() % 10000000);
        printf("%d, ", *aux); 
    }
    printf("\n");
    aux = NULL;
    free(aux); //Liberacion de memoria
    return;
}
void generacionLibros(int size) //Recibe la longitud deseada del arreglo de Enteros, previamente introducida por el usuario
{
    arregloLibros = (struct Libro*)malloc((size + 1) * sizeof(struct Libro)); //Alocacion de memoria
    struct Libro* aux = begin(arregloLibros); //Inicio del arreglo
    for(aux;aux<(arregloLibros + size); aux++) //Llenado de arreglo con aritmetica de apuntadores
    {
        printf("Introduzca el nombre del libro.\n");
        aux->titulo = (char*)malloc(80 * sizeof(char));
        scanf(" %[^\n]", aux->titulo);
        do
        {
            printf("Introduzca el numero de paginas del libro.\n");
            scanf("%d", &(aux->paginas));
        } while (aux->paginas<1);
        printf("El libro %s, con %d paginas ha sido introducido en el arreglo.\n", aux->titulo, aux->paginas);
    }
    printf("\n");
    aux = NULL;
    free(aux); //Liberacion de memoria
    return;
}
void* begin(void* arreglo) //Recibe arreglo del que se quiere regresar su posicion inicial
{
    return (void*)arreglo;
}
void* end(void* arreglo) //Recibe arreglo del que se quiere regresar posicion de ultimo elemento del arreglo de enteros
{
    int* aux =begin(arreglo);
    while(*((int*)next(aux))!=0) //Elemento debe ser distinto a 0 (equivalente a NULL en enteros)
    {
        aux++;
    }
    ++aux;
    return (void*)aux;
}
void* endLibro(void* arreglo) //Recibe arreglo del que se quiere regresar posicion de ultimo elemento del arreglo de Libros
{
    struct Libro* aux = begin(arreglo);
    while(((struct Libro*)nextLibro(aux))->paginas!=0) //Libro no inicializado cuenta con numero de paginas 0
    {
        aux++;
    }
    ++aux;
    return (void*)aux;
}
void* next(void* arreglo) //Recibe arreglo del que se quiere regresar 0 (NULL) si se ha llegado a final de arreglo de numeros o simplemente la siguiente posicion del mismo
{
    int* posOriginal = arreglo;
    if(*(posOriginal) == 0) //EQUIVALENTE A ESPACIO DE MEMORIA NO INICIALIZADO = FINAL DE ARREGLO
    {
        return 0;
    }
    else
    {
        ++posOriginal;
        return (void*)posOriginal;
    } 
}
void* nextLibro(void* arreglo) //Recibe arreglo del que se quiere regresar NULL si se ha llegado a final de arreglo de Libros o simplemente la siguiente posicion del mismo
{
    struct Libro* libroOriginal = arreglo;
    if((libroOriginal)==NULL)
    {
        return NULL;
    }
    else
    {
        ++libroOriginal;
        return (void*)libroOriginal;
    }
}
void* prev(void* arreglo) //Recibe arreglo del que se quiere regresar 0 (NULL) si se ha llegado a inicio de arreglo de numeros o simplemente la posicion anterior del mismo
{
    int* posOriginal = arreglo;
    if(posOriginal == begin(arregloNumeros))
    {
        return 0;
    }
    else
    {
        --posOriginal;
        return (void*)posOriginal;
    }
}
void* prevLibro(void* arreglo) //Recibe arreglo del que se quiere regresar NULL si se ha llegado a inicio de arreglo de Libros o simplemente la posicion anterior del mismo
{
    struct Libro* posOriginal = arreglo;
    if(posOriginal == begin(arregloLibros))
    {
        return NULL;
    }
    else
    {
        --posOriginal;
        return (void*)posOriginal;
    }
}
void impresionInt(void* elementoAImprimir) //Recibe elemento de arreglo de numeros enteros a imprimir
{
    printf("%d, ", (*(int*)elementoAImprimir));
}
void impresionLibro(void* elementoAImprimir) //Recibe elemento de arreglo de Libros enteros a imprimir
{
    struct Libro* libroOriginal = elementoAImprimir;
    printf("Libro %s, con %d paginas.\n", libroOriginal->titulo, libroOriginal->paginas);
}
void recorridogenerico(void* arreglo, iterador iter) //Recibe arreglo y tipo de iterador a ser utilizado
{
    if(arreglo == arregloNumeros)
    {
        int* aux = begin(arreglo);
        (*iter)(arreglo, aux, impresionInt); //Planteamiento para numeros enteros
        printf("\n");
        aux = NULL;
        free(aux);
        return;
    }
    struct Libro* aux = begin(arreglo);
    (*iter)(arreglo, aux, impresionLibro); //Planteamiento para Libros
    aux = NULL;
    free(aux);
    return;
}
void forward(void* arreglo, void* aux, impresion modo) //Recibe arreglo, variable auxiliar para inicializar begin y tipo de impresion a usar
{
    aux = begin(arreglo); //Memoria de aux se libera en funcion recorridogenerico
    while (aux!=END(arreglo))
    {
        modo(aux);
        aux = NEXT(aux);
    }
    aux = NULL;
    free(aux);
} 
void reverse(void* arreglo, void* aux, impresion modo) //Recibe arreglo, variable auxiliar para inicializar begin y tipo de impresion a usar
{
    aux = PREV(END(arreglo));  //Memoria de aux se libera en funcion recorridogenerico
    while (aux!= PREV(begin(arreglo)))
    {
        modo(aux);
        aux = PREV(aux);
    }
    aux = NULL;
    free(aux);
}
void bidirectional(void* arreglo, void* aux, impresion modo) //Recibe arreglo, variable auxiliar para inicializar begin y tipo de impresion a usar
{
    printf("%s\n", "Inicia impresion de inicio a fin");
    forward(arreglo,aux,modo); //Memoria de aux se libera en funcion recorridogenerico
    printf("\n");
    printf("%s\n", "Inicia impresion de fin a inicio");
    reverse(arreglo,aux,modo); //Memoria de aux se libera en funcion recorridogenerico
    printf("\n");
}
void menu() //Menu general de la aplicacion
{
    t_opcion_menu* menu = (t_opcion_menu*)malloc(3 * sizeof(t_opcion_menu));
    *menu = generaMenu; 
    *(menu + 1) = recorridoEImpresionMenu;
    
    int sizeEnteros = 0;
    int sizeLibros = 0;
    int sizeFlotantes = 0;

    int opcion = 0;

    while(opcion != 3)
    {
        printf("Selecciona la opcion a ejecutar.\n");
        printf("1.- Genereacion/Creacion de arreglo.\n");
        printf("2.- Recorrido e Impresion del arreglo.\n");
        printf("3.- Finalizacion del programa.\n");
        scanf("%d", &opcion);
        if(opcion!=3 && opcion>=0 && opcion<=2)
        {
            (*(menu+opcion-1))();
        }
        else if(opcion!=3)
        {
            printf("La opcion introducida no es valida. \n\n");
            continue;
        }
        else
        {
            break;
        }
    }
    free(arregloNumeros);
    liberacionLibros(arregloLibros,sizeLibros);
    free(arregloLibros);
    free(menu);
    return;
}
void generaMenu() //Submenu para generacion de arreglos, usuario define tipo de dato y longitud del arreglo
{
    int opcion;
    do
    {
        printf("Introduzca el numero de elementos que contendra el arreglo.\n");
        int size;
        scanf("%d", &size);
        printf("Selecciona el tipo de arreglo a generar.\n");
        printf("1.- Generar arreglo de ENTEROS aleatorios.\n");
        printf("2.- Generar arreglo de Libros.\n");
        scanf("%d", &opcion);
        switch(opcion)
        {
            case 1:
                generacionNumerosAleatoriosInt(size);
                break;
            case 2:
                generacionLibros(size);
                break;
        }
    } while(opcion< 0 >3);
    return;
}
void recorridoEImpresionMenu() //Submenu para definir sobre que arreglo se va a recorrer e imprimir
{
    int opcionn;
    do
    {
        printf("Selecciona el tipo de arreglo a generar.\n");
        printf("1.- Recorrer e Imprimir arreglo de ENTEROS.\n");
        printf("2.- Recorrer e Imprimir arreglo de Libros.\n");
        scanf("%d", &opcionn);
        switch(opcionn)
        {
            // Definicion de funciones a utilizar
            case 1:
                END = *end;
                PREV = *prev;
                NEXT = *next;
                recorridoEImpresionMenu2(arregloNumeros);
                break;
            case 2:
                END = *endLibro;
                PREV = *prevLibro;
                NEXT = *nextLibro;
                recorridoEImpresionMenu2(arregloLibros);
                break;
        }
    } while (opcionn<0>4);   
}
void recorridoEImpresionMenu2(void* arreglo) //Submenu para escoger tipo de iterador
{
    int opcionR;
    if(begin(arreglo)!=NULL)
    {
        do
        {
            printf("Selecciona el tipo de iterador a utilizar.\n");
            printf("1.- Forward.\n");
            printf("2.- Reverse.\n");
            printf("3.- Bidirectional.\n");
            scanf("%d", &opcionR);
            switch(opcionR)
            {
                case 1:
                    recorridogenerico(arreglo, forward);
                    break;
                case 2:
                    recorridogenerico(arreglo, reverse);
                    break;
                case 3:
                    recorridogenerico(arreglo, bidirectional);
                    break;
            }
        } while (opcionR<0>4);
    }
    else
    {
        printf("%s\n", "No se puede trabajar con un arreglo vacio y sin definir.");
    }    
}
void liberacionLibros(struct Libro* libros, int size) //Recibe arreglo de libros y su longitud; liberacion de memoria de elemento titulo del libro
{
    struct Libro* aux = libros;
    for(aux; aux<(libros + size); aux++)
    {
        free(aux->titulo);
    }
    free(aux);
    return;
}