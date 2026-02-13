#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/msg.h>

#include "pubsub.h"

void subscriber(int id_coda_registrazioni, long topic);

int main() {

    srand(getpid());
    
    key_t chiave_coda_registrazioni = ftok(".", 'a');
    int id_coda_registrazioni = msgget(chiave_coda_registrazioni, IPC_CREAT | 0664);

    if(id_coda_registrazioni < 0){
        perror("errore msgget coda registrazioni");
        exit(1);
    }

    for(int i=0; i<NUM_SUBSCRIBERS_PER_TOPIC; i++) {

        printf("[SUBSCRIBER] Avvio processo subscriber su topic 1\n");

        /* TBD: 
            Creare un processo figlio, che esegua la funzione "subscriber".
            Passare come parametri lo ID della coda per le registrazioni, e il valore TOPIC1. */

            pid_t pid_sub = fork();

            if(pid_sub == 0){
                subscriber(id_coda_registrazioni, TOPIC1);
                exit(0);
            }else if(pid_sub < 0){
                perror("errore fork subscriber");
                exit(1);
            }
    }


    for(int i=0; i<NUM_SUBSCRIBERS_PER_TOPIC; i++) {

        printf("[SUBSCRIBER] Avvio processo subscriber su topic 2\n");

        /* TBD: 
            Creare un processo figlio, che esegua la funzione "subscriber".
            Passare come parametri lo ID della coda per le registrazioni, e il valore TOPIC2.
        */

        pid_t pid_sub = fork();
            if(pid_sub == 0){
                subscriber(id_coda_registrazioni, TOPIC2);
                exit(0);
            }else if(pid_sub < 0){
                perror("errore fork subscriber");
                exit(1);
            }
    }


    printf("[SUBSCRIBER] Attesa terminazione processi figli\n");

    /* TBD: Attendere la terminazione dei processi figli */

    int status;
    for(int i=0; i<(NUM_SUBSCRIBERS_PER_TOPIC * 2); i++){
        wait(&status);
    }

    return 0;
}

void subscriber(int id_coda_registrazioni, long topic) {


    key_t chiave_coda_messaggi_subscriber = IPC_PRIVATE;

    int id_coda_messaggi_subscriber = msgget(chiave_coda_messaggi_subscriber, IPC_CREAT | 0664);

    if(id_coda_messaggi_subscriber < 0){
        perror("errore msgget coda messaggi subscriber");
        exit(1);
    }

    messaggio_registrazione registrazione;

    /* TBD: 
        Inviare un messaggio di registrazione al broker.
        Includere nel messaggio lo ID della coda di messaggi privata.
    */

    registrazione.mtype = 1;
    registrazione.topic = topic;
    registrazione.id_coda = id_coda_messaggi_subscriber;

    msgsnd(id_coda_registrazioni, &registrazione, sizeof(messaggio_registrazione)- sizeof(long), 0);

    for(int i=0; i<NUM_MESSAGGI*NUM_PUBLISHERS_PER_TOPIC; i++) {

        messaggio_valore messaggio;

        /* TBD: Ricevere un messaggio dal broker */

        msgrcv(id_coda_messaggi_subscriber, &messaggio, sizeof(messaggio_valore)- sizeof(long),0, 0);

        int valore = messaggio.valore;

        printf("[SUBSCRIBER] Ricevuto messaggio: topic=%ld, valore=%d\n", topic, valore);

    }

    /* TBD: Deallocare la coda di messaggi privata del subscriber */

    msgctl(id_coda_messaggi_subscriber, IPC_RMID, NULL);

}

