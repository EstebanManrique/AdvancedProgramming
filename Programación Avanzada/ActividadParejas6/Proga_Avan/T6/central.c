#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define TCP_PORT 8000

void enviador_int_aux(int,int);
void enviador_int(int*,int*);
void imprimirEstados(int*, int*);
void gestor_CtrlC(int);
void gestor_CtrlZ(int);
int leer_int(int);
void gestor(char*, void (* handler_t)(int));


int main(int argc, const char * argv[])
{     
    struct sockaddr_in direccion;
    char buffer[1000];
    int* pids;
    pids = (int*)malloc(sizeof(int)*3);
    int servidor;
    int *cliente;
    cliente = (int*)malloc(sizeof(int)*3);
    int* altos;
    int* intermitentes;
    int* estadosActuales;
    estadosActuales = (int*)malloc(sizeof(int)*3);

    ssize_t leidos, escritos;
    int continuar = 3;
    pid_t pid;
    
    //EN CASO DE NO RECIBIR IP CORRECTA
    if (argc != 2) 
    {
        printf("Use: %s IP_Servidor. Favor de introducir direccion Ip para uso de socket -> ./central 127.0.0.1 \n", argv[0]);
        exit(-1);
    }
    //CREACION DEL SOCKET
    servidor = socket(PF_INET, SOCK_STREAM, 0);
    //ENLACE CON SOCKET
    inet_aton(argv[1], &direccion.sin_addr);
    direccion.sin_port = htons(TCP_PORT);
    direccion.sin_family = AF_INET;
    //ENLACE ENTRE IP Y PUERTO
    bind(servidor, (struct sockaddr *) &direccion, sizeof(direccion));
    //LIMITA NUMERO DE CLIENTES
    listen(servidor, 3);
    //TAMANO ESTRUCTURA
    int tamano = sizeof(direccion);

    //escritos = sizeof(direccion);

    //ACEPTAR CONEXIONES
    int i = 0;
    int* auxClientes = cliente;
    int* auxPid = pids;
    int *turno =  (int*)malloc(sizeof(int)*3);
    int*auxTurno=turno;
    int *auxActuales = estadosActuales;

    while(i<continuar)
    {
        *(auxClientes + i)= accept(servidor, (struct sockaddr *) &direccion, &tamano);
        printf("Aceptando conexiones en %s:%d \n", inet_ntoa(direccion.sin_addr), ntohs(direccion.sin_port));
        *(pids + i) = leer_int(*(auxClientes + i));
        printf("PID: %d\n", *(pids + i) );
        *auxTurno=i;
        auxTurno++;
        *auxActuales = 0;
        auxActuales++;
        i++;
    }

    enviador_int(pids,cliente);
    enviador_int(turno,cliente);
    

    while(1)
    {
        imprimirEstados(estadosActuales, cliente);
        //sleep(1);
    }
    
    auxClientes = cliente;    
    for (int i = 0; i < 3; i++)
    {
        close(*(auxClientes + i));
    }
    close(servidor);
    free(cliente);
    free(pids);
    free(estadosActuales);
    return 0;
}

void enviador_int(int*arr_pid,int*arr_clientes)
{
    int *auxClientes=arr_clientes;
    int *auxPid;
    for(auxPid=arr_pid+1;auxClientes<arr_clientes+3;++auxPid,++auxClientes)
    {   
        if(auxPid==arr_pid+3)
        {
            enviador_int_aux(*arr_pid,*auxClientes);
        }
        else
        {
            enviador_int_aux(*auxPid,*auxClientes);
        }
    }
}

void enviador_int_aux(int arr_pid,int arr_clientes)
{
    char buffer[1000];
    sprintf(buffer, "%d", arr_pid);
    write(arr_clientes, &buffer, sizeof(buffer));
}

void gestor(char* buffer, void (* handler_t)(int))
{
    struct sigaction gestorr;
    gestorr.sa_handler = handler_t;
    //sigaction(, &gestorr, );
}

void imprimirEstados(int* actuales, int*arr_clientes)
{
    int* auxClientes = arr_clientes;
    int* auxActuales = actuales;
    char buffer[1000];
    int banderaCambio = 0;
    for(; auxClientes<arr_clientes+3; ++auxClientes)
    {
        int estado = leer_int(*auxClientes);
        int contador=0;
        for(int i=0;i<3;i++,++auxActuales)
        {
            if(auxClientes==(arr_clientes+i)) *(auxActuales) = estado;
            else  *(auxActuales) = 0;
        }

        for (int i = 0; i < 3; i++)
        {
            if(*(actuales+i)==0) contador++;
        }
        
        auxActuales = actuales;

        if(contador!=3)
        {          
            for(int i = 0; i<3; i++)
            {
                printf("Estado semaforo %d: %d\n",(i + 1) , *(auxActuales + i));
            }
            printf("\n");
        }

    }
}

void gestor_CtrlC(int sig)
{

}

void gestor_CtrlZ(int sig)
{

}

int leer_int(int cliente)
{
    char buffer[1000];
    int x;
    read(cliente, &buffer, sizeof(buffer));
    x=atoi(buffer);
    return x;
}