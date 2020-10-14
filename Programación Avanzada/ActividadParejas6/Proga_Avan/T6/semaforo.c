#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#define TCP_PORT 8000

void gestor_Usr(int);
int leer_int(int);

int color=0;
int cliente;

int main(int argc, const char * argv[])
{
    FILE *stream;

    struct sockaddr_in direccion;
    char buffer[1000];
    int pid_next;
    
    ssize_t leidos, escritos;
    
    if (argc != 2) 
    {
        printf("Use: %s IP_Servidor \n", argv[0]);
        exit(-1);
    }
    
    // Crear el socket
    cliente = socket(PF_INET, SOCK_STREAM, 0);
    
    // Establecer conexión
    inet_aton(argv[1], &direccion.sin_addr);
    direccion.sin_port = htons(TCP_PORT);
    direccion.sin_family = AF_INET;
    
    escritos = connect(cliente, (struct sockaddr *) &direccion, sizeof(direccion));

    if (escritos == 0) 
    {
        signal(SIGTSTP, SIG_IGN);
        //signal(SIGINT, SIG_IGN);

        printf("Conectado a %s:%d \n", inet_ntoa(direccion.sin_addr), ntohs(direccion.sin_port));
        sleep(1);
        printf("%d\n", getpid());
        sprintf(buffer, "%d", getpid());
        write(cliente, &buffer, sizeof(buffer));

        pid_next= leer_int(cliente);
        printf("PID: %d\n", pid_next);

        int inicio=leer_int(cliente);
        printf("Numero: %d\n", inicio);
        
        // Escribir datos en el socket
        signal(SIGUSR1, gestor_Usr);
        sleep(1);

        if(inicio == 0)
        {
            color = 1;
        }
        while(1)
        {
            if(color == 1)
            {
                color = 0;
                kill(pid_next, SIGUSR1);
                sleep(1);
            }

        }
    }
    
    //Cerrar sockets
    close(cliente);
    
    return 0;
}

int leer_int(int cliente)
{
    char buffer[1000];
    int x;
    read(cliente, &buffer, sizeof(buffer));
    x=atoi(buffer);
    return x;
}

void gestor_Usr(int sig)
{
    char buffer[1000];
    sprintf(buffer, "%d", 1);
    write(cliente, &buffer, sizeof(buffer));
    printf("Cambio mi estado a SIGA.\n");
    color = 1;
    sleep(5);
    printf("Cambio mi estado a ALTO.\n");
}