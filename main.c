#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define N_DESTRUCTORES 33

// Semáforos para fases
sem_t sem_HablarConPresidentess;
sem_t sem_ComenzarInvadirCiudades;
sem_t sem_NavesAtacan;
sem_t sem_CiudadSeDefiende;
sem_t sem_NodrizaDesciende;
sem_t sem_AbrirCompuertas;
sem_t sem_EvacuarPlaneta;

// Contadores y mutex
int PresidentesYaHablados = 0;
int naves_posicionadas = 0;
int naves_atacaron = 0;
int ciudades_defendidas = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// los sleeps son para que el programa se vaya ejecutando de a poco y no todo de golpe
void presentarseALideresMundiales() {
printf("El comandante extraterrestre habla al presidente\n");
sleep(1);   // simulo que hablan un rato
}

void hablarConComandanteExtraterrestre(int id) {
printf(" Presidente %d hablando con comandante extraterrestre\n", id);
presentarseALideresMundiales();
}
void tomarPosicionEnMegaCiudad(int id) {
printf(" Nave destructora %d tomando posición\n", id);
sleep(1);
}
void atacarMegaciudad(int id) {
printf(" Nave destructora %d ataca la megaciudad\n", id);
sleep(1);
}
void iniciarProtocoloDefensivo(int id) {
printf(" Ciudad %d activa protocolo defensivo\n", id);
sleep(1);
}
void contraatacarNaves(int id) {
printf(" Ciudad %d contraataca, pero es ineficaz\n", id);
sleep(1);
}
void perderBatalla(int id) {
printf(" Ciudad %d ha caído\n", id);
sleep(1);
}
void descenderATierra() {
printf(" La nave nodriza desciende a la Tierra\n");
sleep(1);
}
void abrirCompuertas() {
printf(" La nave nodriza abre compuertas, un extranio polvillo entra junto con el aire (polen) y empiezan a morir los extraterrestres\n");
sleep(1);
}
void abandonarTierra(int id) {
printf(" La nave %d evacua y abandona la Tierra, tras la orden dada por el comandante.\n", id);
sleep(1);
}

// funciones para los hilos

void* HablarConPresidentes(void* arg) {
int id = *(int*)arg;
sem_wait(&sem_HablarConPresidentess); // Solo un presidente a la vez
hablarConComandanteExtraterrestre(id);
PresidentesYaHablados++;
if (PresidentesYaHablados == 10) {
    for (int i = 0; i < N_DESTRUCTORES; i++)
        sem_post(&sem_ComenzarInvadirCiudades);  // una vez que todos los presidentes hablaron doy permiso a las naves
}
sem_post(&sem_HablarConPresidentess); // libero el semáforo para el siguiente presidente

return NULL;
}

void* naveDestructora(void* arg) {
int id = *(int*)arg;
sem_wait(&sem_ComenzarInvadirCiudades); // Espera a que todos los presidentes hayan hablado
tomarPosicionEnMegaCiudad(id);

pthread_mutex_lock(&mutex);     // inicio sección crítica para que dea  uno modifique el contador
naves_posicionadas++;
pthread_mutex_unlock(&mutex);
if (naves_posicionadas == N_DESTRUCTORES) {     // si todas las naves se posicionaron ataco
for (int i = 0; i < N_DESTRUCTORES; i++)
sem_post(&sem_NavesAtacan); // Permitir que las naves ataquen
}


sem_wait(&sem_NavesAtacan);
atacarMegaciudad(id);

pthread_mutex_lock(&mutex);             // inicio sección crítica para que dea  uno modifique el contador
naves_atacaron++;
pthread_mutex_unlock(&mutex);
if (naves_atacaron == N_DESTRUCTORES) { // si ya se ataco a todas las ciudades 
for (int i = 0; i < N_DESTRUCTORES; i++)
sem_post(&sem_CiudadSeDefiende); // Permitir que las ciudades se defiendan
}

sem_wait(&sem_EvacuarPlaneta);      //espera a semaforo de que la nave nodriza descendia y de el permiso  para evacuar el planeta 
//printf("Nave destructora %d ", id);
abandonarTierra(id);
return NULL;
}

void* ciudad(void* arg) {
int id = *(int*)arg;
sem_wait(&sem_CiudadSeDefiende);        //espera a que todas las naves ataquen para que las ciudades se defiendan

iniciarProtocoloDefensivo(id);
contraatacarNaves(id);
perderBatalla(id);

pthread_mutex_lock(&mutex);     // inicio sección crítica para que de a uno modifique el contador
ciudades_defendidas++;
pthread_mutex_unlock(&mutex);
if (ciudades_defendidas == N_DESTRUCTORES) {    // si todas las ciudades fueron defendidas
sem_post(&sem_NodrizaDesciende); // Permitir que la nave nodriza descienda
}
return NULL;
}


void* naveNodriza(void* arg) {
sem_wait(&sem_NodrizaDesciende);            //espera a semafoto de que las ciudades fueron defendidas para descender
descenderATierra();
abrirCompuertas();

//libera a todas las naves
for (int i = 0; i < N_DESTRUCTORES; i++){
sem_post(&sem_EvacuarPlaneta);          //liberar las 33 naves
}


return NULL;
}


// Programa
int main() {
pthread_t HablarConPresidentess[10], destructores[N_DESTRUCTORES], ciudades[N_DESTRUCTORES], nodriza;
int id_pres[10], id_des[N_DESTRUCTORES], id_ciu[N_DESTRUCTORES];

// Inicializar semaforos
sem_init(&sem_HablarConPresidentess, 0, 1);
sem_init(&sem_ComenzarInvadirCiudades, 0, 0);
sem_init(&sem_NavesAtacan, 0, 0);
sem_init(&sem_CiudadSeDefiende, 0, 0);
sem_init(&sem_NodrizaDesciende, 0, 0);
sem_init(&sem_EvacuarPlaneta, 0, 0);


// Crea hilo para hablar con presidentes
for(int i=0; i<10; i++) {
id_pres[i] = i+1;
pthread_create(&HablarConPresidentess[i], NULL, HablarConPresidentes, &id_pres[i]);
pthread_join(HablarConPresidentess[i],NULL);
}                   //asi habla de  un solo presidente por ves

// Crea hilos destructores y ciudades
for(int i=0; i <N_DESTRUCTORES;i++){
id_des[i] = i+1;
id_ciu[i] = i+1;
pthread_create(&destructores[i], NULL, naveDestructora, &id_des[i]);
pthread_create(&ciudades[i], NULL, ciudad, &id_ciu[i]);
}

// Crea hilo nodriza
pthread_create(&nodriza, NULL, naveNodriza, NULL);

// Esperar hilos
for(int i=0; i<N_DESTRUCTORES;i++) pthread_join(destructores[i],NULL);
for(int i=0; i<N_DESTRUCTORES;i++) pthread_join(ciudades[i],NULL);
pthread_join(nodriza, NULL);

//Destruir hilos y mutex 
pthread_mutex_destroy(&mutex);
sem_destroy(&sem_HablarConPresidentess);
sem_destroy(&sem_ComenzarInvadirCiudades);
sem_destroy(&sem_NavesAtacan);
sem_destroy(&sem_CiudadSeDefiende);
sem_destroy(&sem_NodrizaDesciende);
sem_destroy(&sem_EvacuarPlaneta);
return 0;
}
