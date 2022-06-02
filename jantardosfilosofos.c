#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

#define N 5
#define pensando 2
#define faminto 1
#define comendo 0
#define esquerda (nfilo + 4) % N
#define direita (nfilo + 1) % N

int estado[N];
int filosofo[N] = { 0, 1, 2, 3, 4 };

sem_t mutex;
sem_t S[N];

void teste(int nfilo)
{
	if (estado[nfilo] == faminto
		&& estado[esquerda] != comendo
		&& estado[direita] != comendo) {
		// estado quando comendo
		estado[nfilo] = comendo;

		sleep(2);

		printf("Filosofo %d pegou garfo %d e %d\n",
					nfilo + 1, esquerda + 1, nfilo + 1);

		printf("Filosofo %d esta comendo\n", nfilo + 1);

		// sem_post(&S[nfilo]) não tem efeito
		// durante pega_garfo
		// usado para acordar o filosofo faminto
		// durante deixa_garfo
		sem_post(&S[nfilo]);
	}
}

// funçao para pegar o garfo
void pegarGarfo(int nfilo)
{

	sem_wait(&mutex);

	// quando esta faminto
	estado[nfilo] = faminto;

	printf("Filosofo %d esta faminto\n", nfilo + 1);

	// comer se o do lado não estiver comendo
	teste(nfilo);

	sem_post(&mutex);

	// caso nao dé pra comer, esperar a sua vez
	sem_wait(&S[nfilo]);

	sleep(1);
}

// funçao para deixar o garfo
void deixarGarfo(int nfilo)
{

	sem_wait(&mutex);

	// pensando
	estado[nfilo] = pensando;

	printf("Filosofo %d deixando o garfo %d e %d na mesa\n",
		nfilo + 1, esquerda + 1, nfilo + 1);
	printf("Filosofo %d esta pensando\n", nfilo + 1);

	teste(esquerda);
	teste(direita);

	sem_post(&mutex);
}

void* Filosofo(void* num)
{

	while (1) {

		int* i = num;

		sleep(1);

		pegarGarfo(*i);

		sleep(0);

		deixarGarfo(*i);
	}
}

int main()
{

	int i;
	pthread_t thread_id[N];

	// inicialização do semaforo
	sem_init(&mutex, 0, 1);

	for (i = 0; i < N; i++)

		sem_init(&S[i], 0, 0);

	for (i = 0; i < N; i++) {

		// processo de criação de threads
		pthread_create(&thread_id[i], NULL, Filosofo, &filosofo[i]);

		printf("Filosofo %d esta pensando\n", i + 1);
	}

	for (i = 0; i < N; i++)

		pthread_join(thread_id[i], NULL);
}
