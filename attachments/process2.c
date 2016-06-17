#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

#define SHMSIZE 27
#define SHMSIZES 32
#define FILEKEY "/bin/cat"
#define KEY_PROCESO 1300
#define MAXBUF_PROCESO 5
#define KEY_CONTADOR 1500
#define KEY_FUNCIONES 1600
#define SLEEP 1000000
/*gcc process3.c -w -o process3.x 
^[[Akarelia@localhost ~/Downloads $ ./process3.x
*/ 
int * turno=NULL;
int * interesado=NULL; /* shared buffer */
char *shm, *s;

int TRUE=1;
int FALSE=0;

void escribir(int proc)
{
  fprintf (stderr, "HOOLAS\n");
}
void entrar(int proc)
{
  int otro;
  otro=1-proc;
  interesado[proc]= 1;
  turno[0]=proc;
  while(turno[0]==proc && interesado[otro]==1);
  printf("escribiendo(2)");
}
void salir(int proc)
{
  interesado[proc]=0;
}
main()
{
 //turno///////////////////////////////////////////////////////////////////////
	
	
	int key_proceso = ftok(FILEKEY, KEY_PROCESO);
	if (key_proceso == -1) {
		fprintf (stderr, "Error con la key \n");
		return -1; 
	}    
	int id_zone1 = shmget (key_proceso, sizeof(turno)*MAXBUF_PROCESO, 0777 | IPC_CREAT);
	if (id_zone1 == -1) {
		fprintf (stderr, "Error con id_zone 1 \n");
		return -1; 
	} 
	turno = shmat(id_zone1,NULL , 0);	
	if (turno == NULL) {
		fprintf (stderr, "Error reservando memoria en turno\n");
		return -1; 
	}
	
//////////////////////////////////////////////////////////////////////////// 
 //interesado///////////////////////////////////////////////////////////////////////
	/* Key to shared memory */
	int key_contador = ftok(FILEKEY, KEY_CONTADOR);
	if (key_contador == -1) {
		fprintf (stderr, "Error con la key \n");
		return -1; 
	}    
	
	/*Creacion de memoria compartida */
	int id_zone3 = shmget (key_contador, sizeof(int)*2, 0777 | IPC_CREAT);
	if (id_zone3 == -1) {
		fprintf (stderr, "Error con id_zone \n");
		return -1; 
	} 
		
	/* we declared to zone to share */
	interesado = (int *)shmat (id_zone3, (char *)0, 0);
	if (interesado == NULL) {
		fprintf (stderr, "Error reservando memoria interesado\n");
		return -1; 
	}
////////////////////////////////////////////////////////////////////////////
	

	
 
 char c;
 int shmid;
 key_t key;
 
 
 /*Lo llama con es ID */
 key = 5678;

 /* Crea segmento */
 if ((shmid = shmget(key, SHMSIZE, IPC_CREAT | 0666)) < 0) {
 perror("shmget");//describe el error
 exit(1);
 }

 /*Añade segmento . */
 if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
 perror("shmat");
 exit(1);
 }

 printf ("proceso 2 \n");

 /*Escribimos*/
	  *shm++='2';
	  *shm++=' ';
	  *shm++='h';
	  *shm++='o';
	  *shm++='l';
	  *shm++='o';
	  *shm = NULL;
/*terminamos*/
 int i=0;
 for(i=0;i<2;i++){
	//while(1){
      	  entrar(0);
          /**/
	  salir(0);
	}
  if (id_zone1 != -1)
	{
		shmdt ((char *)turno);
	}
	if (id_zone3 != -1)
	{
		shmdt ((char *)interesado);
	}
	if(shmid != -1){
		shmdt ((char*)shm);
	}	
	
 exit(0);
} 