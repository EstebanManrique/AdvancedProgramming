#include <stdio.h>
#include <signal.h>

void gestor_ctrlc (int senial) 
{
    printf("***** Ha pulsado CTRL + C (señal numero %d) \n", senial);
}

void gestor_ctrlz (int senial) 
{
    printf("***** Ha pulsado CTRL + Z (señal numero %d) \n", senial);
}
    
int main(int argc, const char * argv[])
{
    sigset_t conjunto, pendientes;
    int i;
    
    sigemptyset(&conjunto);
    
    sigaddset(&conjunto, SIGINT);
    sigaddset(&conjunto, SIGTSTP);

    
    sigprocmask(SIG_BLOCK, &conjunto, NULL);
    sigpending(&pendientes);
    
    for(i= 0; i < 10; ++i)
    {
        printf("La señal SIGINT está bloqueada ... \n");
        sleep(1);
    }
    
    signal(SIGINT, gestor_ctrlc);
    signal(SIGTSTP, gestor_ctrlz);
    
    sigprocmask(SIG_UNBLOCK, &conjunto, NULL);
    printf("Ya se desbloqueó la señal");
    
    
    //while(1);
    
    return 0;
}