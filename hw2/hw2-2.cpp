#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // for pthread series
#include <unistd.h>  // for fork()

#define N 5	/* number of philosophers */
#define LEFT (i+N-1)%N /* number of i's left neighbor */
#define RIGHT (i+1)%N /* number of i's right neighbor */
#define THINKING 0 /* philosopher is thinking */
#define HUNGRY 1 /* philosopher is trying to get forks */
#define EATING 2 /* philosopher is eating */

typedef pthread_mutex_t semaphore; /* semaphores are a special kind of int */
int state[N]; /* array to keep track of everyone's state */
semaphore mutex; /* mutual exclusion for critical regions */
semaphore s[N]; /* one semaphore per philosopher */
pthread_t P_thread[N];

void* philosopher(void* I);
void take_forks(int i);
void put_forks(int i);
void test(int i);
void think();
void eat(int i);
void print_state();

int main()
{
	int P_num[N];
	pthread_mutex_init( &mutex, NULL );
	printf("=====Start=====\n");
	for(int i=0;i<N;i++)
	{
		P_num[i]=i;
		pthread_mutex_init(&s[i], NULL );
		pthread_mutex_lock( &s[i] );
		pthread_create(&P_thread[i], NULL, philosopher,(void*)&P_num[i]);
	}
	/*while(1)
	{
		pthread_mutex_lock( &mutex );
		printf( "============\n" );
		for(int i=0;i<N;i++)
		{
			if( state[i] == THINKING )
				printf("%d : THINKING\n", i);
			else if( state[i] == HUNGRY )
				printf("%d : HUNGRY\n", i);
			else
				printf("%d : EATING\n", i);
		}
		printf( "============\n" );
		pthread_mutex_unlock( &mutex );
		sleep(1);
	}*/
	for(int i=0;i<N;i++)
		pthread_join( P_thread[i], NULL );
}
void print_state()
{  
    int i;
    printf("=======NOW STATE=======\n");
    for(i=0;i<N;i++)
    {
        if(state[i] == 0)
            printf("%d. THINKING...\n",i+1);
        else if(state[i] == 1)
            printf("%d. HUNGRY...\n",i+1);
        else if(state[i] == 2)
            printf("%d. EATING...\n",i+1);
    }
    printf("=======================\n");
    sleep(1);
}
void* philosopher(void* I)
{
	int i=*(int*)I;
	while(1)
	{
		think();
		take_forks(i);
		eat(i);
		put_forks(i);
	}
}
void think()
{
	sleep(1);
}
void take_forks(int i)
{
	pthread_mutex_lock(&mutex);
	state[i]=HUNGRY;
	print_state();
	test(i);
	pthread_mutex_unlock(&mutex);
	pthread_mutex_lock(&s[i]);
}
void put_forks(int i)
{
	pthread_mutex_lock(&mutex);
	state[i]=THINKING;
	print_state();
	test(LEFT);
	test(RIGHT);
	pthread_mutex_unlock(&mutex);
}
void test(int i)
{
	if(state[i]==HUNGRY&&state[LEFT]!=EATING&&state[RIGHT]!=EATING)
	{
		state[i]=EATING;
		print_state();
		pthread_mutex_unlock(&s[i]);
	}
}
void eat(int i)
{
	int eat_time=rand()%3+1; 
	sleep(eat_time);
}
