#include <iostream>
#include <cstdlib> 
#include <cstdio>
#include <pthread.h>
#include <unistd.h>

#define N 5	/* number of philosophers */
#define LEFT (i+N-1)%N /* number of i's left neighbor */
#define RIGHT (i+1)%N /* number of i's right neighbor */
#define THINKING 0 /* philosopher is thinking */
#define HUNGRY 1 /* philosopher is trying to get forks */
#define EATING 2 /* philosopher is eating */

using namespace std;

void* philosopher(void* I);

class P_Monitor{
	public:
		void ini_Monitor()
		{
			pthread_mutex_init(&mutex,NULL);
			for(int i=0;i<N;i++)
				pthread_cond_init(&s[i],NULL);
		} ;
		void take_forks(int i)
		{
			pthread_mutex_lock(&mutex);
			state[i]=HUNGRY;
			test(i);
			pthread_mutex_unlock(&mutex);
		};
		void put_forks(int i)
		{
			pthread_mutex_lock(&mutex);
			state[i]=THINKING;
			pthread_cond_signal(&s[LEFT]);
			pthread_cond_signal(&s[RIGHT]);
			pthread_mutex_unlock(&mutex);
		};
		void test(int i)
		{
			while(1)
			{
				if(state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING)
				{
					state[i] = EATING;
					break;
				}
				else
					pthread_cond_wait(&s[i],&mutex);
			}
		};
		void think()
		{
			sleep(1);
		};
		void eat()
		{
			int eat_time=rand()%3+1; 
			sleep(eat_time);
		};
		void print_state()
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
		};
	private:
		int state[N];
		pthread_mutex_t mutex; 
		pthread_cond_t s[N]; 
};
P_Monitor P_m;
pthread_t P_thread[N];
int main()
{
	int P_num[N];
	P_m.ini_Monitor();
	printf("=====Start=====\n");
	for(int i=0;i<N;i++)
	{
		P_num[i]=i;
		pthread_create(&P_thread[i], NULL, philosopher,(void*)&P_num[i]);
	}
	while(1)
	{
		P_m.print_state();	
	}
	for(int i=0;i<N;i++)
		pthread_join( P_thread[i], NULL ); 
	return 0;
}
void* philosopher(void* I)
{
	int i=*(int*)I;
	while(1)
	{
		P_m.think();
		P_m.take_forks(i);
		P_m.eat();
		P_m.put_forks(i);
	}
}
