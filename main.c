// Trabajo practico: sincronizacion 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>

#define n_naves_destructoras 33
#define n_ciudades n_naves_destructoras

int main(){
    printf("Cantidad de naves destructoras %d\n", n_naves_destructoras);
    printf("Cantidad de ciudades %d\n", n_ciudades);
    return 0;
}
