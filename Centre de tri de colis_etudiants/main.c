/*
 * TP Centre de tri de colis
 * 2/5/2019 - YA
 */
 
// includes Kernel
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// includes Standard 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// includes Simulateur (redirection de la sortie printf)
#include "simulateur.h"

// Définition du nombre d'éléments pour les files
#define x 10 // nombre d'éléments pour la File_tapis_arrivee 
#define y 10 // nombre d'éléments pour les File_depart_national et File_depart_international
#define z 10 // nombre d'éléments pour la File_relecture 

// Définition des timeout pour les files en ms
#define TIMEOUT_FILE_TAPIS_ARRIVEE 200
#define delayRelecture 1000


// Création des files
xQueueHandle File_tapis_arrivee, File_tapis_relecture, File_depart_national, File_depart_international;

// Création du Semaphore
xSemaphoreHandle handler;




//fonction affichage d'un message 

void affiche_message(char *text, unsigned int colis){
		
	int b0;
	int b1;
	int b2;
	int compteur; 
	
	compteur = colis >> 3;
	
			if (xSemaphoreTake(handler, portMAX_DELAY) == pdTRUE){
					printf("%s : ",text);
					printf("compteur = %d ",compteur);

					b0 = colis& (1<<0);
					b1 = (colis& (1<<1))>>1;
					b2 = (colis& (1<<2))>>2;

					printf("B0 = %d, ",b0);
					printf("B1 = %d, ",b1);
					printf("B2 = %d\n",b2);
				
					xSemaphoreGive(handler);
			}

}

//********************************************************
//* Tâche arrivée 
//*
//* Répète de façon cyclique une liste de colis à deposer
//*
//* B2=1 -> le colis est passé par « tache-relecture », sinon B2=0
//* B1=1 -> étiquette non lisible sinon B1=0
//* B0=1 -> marché international / B0=0 -> marché international
//* 
//********************************************************
void Tache_arrivee( void *pvParameters )
{	
	unsigned int liste_colis[]={  1,   3,   1,   2,   3,   0}; // Etat des bits B2, B2 et B0 -> 0 à 3 en décimal car B2=0
	unsigned int liste_delai[]={  5, 100,   0,   0, 400,  50}; // Temps d'attente en ms pour le colis suivant
	unsigned int num_colis = 0; // Numéro de colis
	unsigned int colis; // colis (étiquette)
while(1)
	{ 
		colis = (num_colis<<3) + liste_colis[num_colis % (sizeof(liste_colis)/sizeof(unsigned int))];
		xQueueSendToBack(File_tapis_arrivee, &colis, TIMEOUT_FILE_TAPIS_ARRIVEE); // Pour simplifier, on considère que la transmissions s'est bien effectuée
		vTaskDelay(liste_delai[num_colis % (sizeof(liste_colis)/sizeof(unsigned int))]/portTICK_RATE_MS); // Attente entre deux colis en ms
		if (xSemaphoreTake(handler, portMAX_DELAY) == pdTRUE){
					printf("Le colis No %d est depose sur le tapis roulant et il porte l'etiquette %d\n",num_colis, colis);	
					xSemaphoreGive(handler);
		}
		affiche_message("Tache arrivee",colis);
		num_colis++;
  }
	vTaskDelete( NULL );
}


//********************************************************
//* Tâche lecture rapide 
//*
//* Récupère un par un les msg de la file_tapis_arrivée
//*
//* B1=1 -> étiquette non lisible sinon B1=0
//* B0=1 -> marché international / B0=0 -> marché international
//* 
//********************************************************
void Tache_lecture_rapide(){
	while(1){
	unsigned int colis;
	int b0;
	int b1;
	
	if (xQueueReceive(File_tapis_arrivee, &colis, TIMEOUT_FILE_TAPIS_ARRIVEE)) {
		
			b0 = colis& (1<<0);
			b1 = (colis& (1<<1))>>1;
			
			if(b1 == 1){ //non lisible ? 
					xQueueSendToBack(File_tapis_relecture, &colis, TIMEOUT_FILE_TAPIS_ARRIVEE);				
			}
			else if(b0 == 0){ //national
					xQueueSendToBack(File_depart_national, &colis, TIMEOUT_FILE_TAPIS_ARRIVEE);				
			}
			else if(b0 == 1){//international
					xQueueSendToBack(File_depart_international, &colis, TIMEOUT_FILE_TAPIS_ARRIVEE);				
			}
			affiche_message("Lecture Rapide", colis);
	}
	}
}


//********************************************************
//* Tâche relecture 
//*
//* Récupère un par un les msg de la file_tapis_arrivée
//* Il doit fixé les bit B2 a 1 et B1 a 0 
//* Et finalement elle renvoie un msg dans le lecture rapide()
//* 
//********************************************************
void Tache_relecture(void *pvParameters){
	unsigned int colis; 
	while(1){
			if (xQueueReceive(File_tapis_arrivee, &colis, TIMEOUT_FILE_TAPIS_ARRIVEE)) {
				affiche_message("Avant Relecture", colis);
				colis &= ~(1<<1);
				colis |= (1<<2);
				xQueueSendToFront(File_tapis_arrivee, &colis, TIMEOUT_FILE_TAPIS_ARRIVEE);
				affiche_message("Apres Relecture", colis);
			}
			vTaskDelay(delayRelecture);
	}
}

//********************************************************
//* Tâche depart NATIONAL & INTERNATIONAL
//*
//* Récupèrent un par un les msg de leurs files et affiche les msg  
//********************************************************
void Tache_depart_national(void *pvParameters){
	unsigned int colis;
		while(1){
			if (xQueueReceive(File_depart_national, &colis, TIMEOUT_FILE_TAPIS_ARRIVEE)) {
				affiche_message("depart national",colis);
			}
		}
}

void Tache_depart_international(void *pvParameters){
	unsigned int colis;
		while(1){
			if (xQueueReceive(File_depart_international, &colis, TIMEOUT_FILE_TAPIS_ARRIVEE)) {
				affiche_message("depart international",colis);
			}
		}
}

// Main()
int main(void)
{
	// ulTaskNumber[0] = Tache_arrivee
	// ulTaskNumber[1] = idle task

	//semaphore init 
	vSemaphoreCreateBinary(handler);
	
	//init files 
	File_tapis_arrivee = xQueueCreate(x, sizeof(unsigned int));
	File_tapis_relecture = xQueueCreate(z, sizeof(unsigned int));
	File_depart_national = xQueueCreate(y, sizeof(unsigned int));
	File_depart_international = xQueueCreate(y, sizeof(unsigned int));
	
	
	// Création des tâches
	xTaskCreate( 	Tache_arrivee, 						// Pointeur vers la fonction
								"Tache arrivee",					// Nom de la tâche, facilite le debug
							  240, 											// Taille de pile (mots)
								NULL, 										// Pas de paramètres pour la tâche
								1, 												// Niveau de priorité 1 pour la tâche (0 étant la plus faible) 
								NULL ); 									// Pas d'utilisation du task handle
	
	xTaskCreate( 	Tache_lecture_rapide, "Tache lecture rapide",	240, NULL, 1, NULL );
	
	xTaskCreate( 	Tache_relecture, "Tache relecture",240,	NULL,1, NULL ); 							
								
	xTaskCreate( 	Tache_depart_national, "Tache depart national",240, NULL, 1, 	NULL ); 									
								
	xTaskCreate( 	Tache_depart_international, "Tache depart international", 240, NULL, 1, NULL );
								
								
	// Lance le scheduler et les taches associées 
	vTaskStartScheduler();
	// On n'arrive jamais ici sauf en cas de problèmes graves: pb allocation mémoire, débordement de pile par ex.
	while(1);
}

