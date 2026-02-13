#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <pthread.h>

#include "pubsub.h"

#define MAX_TOPICS 2
#define MAX_SUBSCRIBERS 2

int code[MAX_TOPICS][MAX_SUBSCRIBERS];

void * gestisci_registrazioni(void * arg);
void * gestisci_messaggi(void * arg);


int main() {

    // inizializzazione lista di code
    for(int i=0; i<MAX_TOPICS; i++) {
        for(int j=0; j<MAX_SUBSCRIBERS; j++) {
            code[i][j] = 0;
        }
    }


    key_t chiave_coda_registrazioni = ftok(".", 'a');
    int id_coda_registrazioni = msgget(chiave_coda_registrazioni, IPC_CREAT | 0664);

    if(id_coda_registrazioni < 0){
        perror("errore msgget coda registrazioni");
        exit(1);
    }


    key_t chiave_coda_messaggi = ftok(".", 'b');
    int id_coda_messaggi = msgget(chiave_coda_messaggi, IPC_CREAT | 0664);

    if(id_coda_messaggi < 0){
        perror("errore msgget coda messaggi");
        exit(1);
    }



    /* TBD:
        Creare un thread che esegua la funzione "gestisci_registrazioni".
        Passare come parametro alla funzione gli id delle code.
     */

    pthread_t gestione_reg; 
    int err;

    err = pthread_create(&gestione_reg, NULL, gestisci_registrazioni, (void *)&id_coda_registrazioni);

    if(err != 0){
        perror("errore create thread gestione registrazione");
        exit(1);
    }



    /* TBD:
        Creare un thread che esegua la funzione "gestisci_messaggi".
        Passare come parametro alla funzione gli id delle code.
     */

    pthread_t gestione_mess; 

    err = pthread_create(&gestione_mess, NULL, gestisci_messaggi, (void *)&id_coda_messaggi);

    if(err != 0){
        perror("errore create thread gestione messaggi");
        exit(1);
    }


    /* TBD: Attendere la terminazione dei 2 thread */

    pthread_join(gestione_reg, NULL);
    pthread_join(gestione_mess, NULL);

    /* TBD: Deallocare le 2 code di messaggi */

    msgctl(id_coda_messaggi, IPC_RMID, NULL);
    msgctl(id_coda_registrazioni, IPC_RMID, NULL);

}


void * gestisci_registrazioni(void * arg) {


    /* TBD: Completare il passaggio di parametri */


    for(int i=0; i<TOTALE_SUBSCRIBER; i++) {

        messaggio_registrazione messaggio;

        /* TBD: Ricevere un messaggio di registrazione */



        long topic = messaggio.topic;
        int id_coda = messaggio.id_coda;

        printf("[BROKER] Ricevuto messaggio di registrazione: topic=%ld, id_coda=%d\n", topic, id_coda);


        // aggiungo il subscriber alla lista di code

        if(topic <= 0 || topic > MAX_TOPICS) {
            printf("[BROKER] Topic non valido\n");
            exit(1);
        }

        if(id_coda <= 0) {
            printf("[BROKER] Id coda non valido\n");
            exit(1);
        }

        int trovato = 0;
        for(int i=0; i<MAX_SUBSCRIBERS; i++) {
            if(code[topic-1][i] == 0) {
                code[topic-1][i] = id_coda;
                trovato = 1;
                break;
            }
        }

        if(trovato == 0) {
            printf("[BROKER] Numero massimo di subscriber raggiunto\n");
            exit(1);
        }

    }

    return NULL;
}



void * gestisci_messaggi(void * arg) {


    /* TBD: Completare il passaggio di parametri */

    int id_coda_messaggi = *((int *)arg);

    for(int i=0; i<TOTALE_MESSAGGI; i++) {

        messaggio_valore messaggio;

        /* TBD: Ricevere un messaggio dai publisher  */

        ssize_t bytes = msgrcv(id_coda_messaggi, &messaggio, sizeof(messaggio_valore)-sizeof(long), 0, 0);

        if(bytes < 0) {
            perror("[BROKER] Errore ricezione messaggio");
            continue; // O exit(1) in base alla severità desiderata
        }

        long topic = messaggio.topic;
        int valore = messaggio.valore;

        printf("[BROKER] Ricevuto messaggio: topic=%ld, valore=%d\n", topic, valore);



        // invio messaggio a tutti i subscriber registrati al topic

        if(topic <= 0 || topic > MAX_TOPICS) {
            printf("[BROKER] Topic non valido\n");
            exit(1);
        }

        for(int j=0; j<MAX_SUBSCRIBERS; j++) {

            int id_coda = code[topic-1][j];

            if(id_coda != 0) {

                /* TBD: Inviare il messaggio al subscriber in ascolto sulla coda "id_coda" */

                int err = msgsnd(id_coda, &messaggio, sizeof(messaggio_valore)-sizeof(long), 0);
                
                if(err < 0) {
                    perror("[BROKER] Errore inoltro messaggio");
                }
            }
        }

    }

    return NULL;
}
