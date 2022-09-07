#include "mm.h"
void test_myrealloc(int n);
void test_mymalloc(int n);
void test_mycalloc(int n);
void print_array(int *arr, int n);

int main()
{
	srand(time(NULL));
	printf("Array size: ");
	int n;
	scanf("%d",&n);
	test_mymalloc(n);
	test_mycalloc(n);
	test_myrealloc(n);
	return 0;
}
void print_array(int *arr, int n)
{
	int i;
	printf("Address : %p\n",arr);	
	for(i = 0 ; i < n ; i++)
		printf("%3d ",arr[i]);
	printf("\n");
}

void test_mymalloc(int n)
{
	int i;
	printf("---test mymalloc.---\n");
	printf("============================\n");
	int *test_MemoryArray = (int *)mymalloc(sizeof(int) * n);
	for(i = 0 ; i < n ; i++)
		test_MemoryArray[i] = rand() % 99;	
	print_array(test_MemoryArray, n);
	printf("============================\n\n");
	myfree(test_MemoryArray);
}

void test_mycalloc(int n)
{
	int i;
	printf("---test mycalloc.---\n");
	printf("============================\n");
	int *test_MemoryArray = (int *)mycalloc(n, sizeof(int));
	print_array(test_MemoryArray,n);
	for(i = 0 ; i < n ; i++)
			test_MemoryArray[i] = rand() % 99;
	print_array(test_MemoryArray,n);
	printf("============================\n\n");
	myfree(test_MemoryArray);
}

void test_myrealloc(int n)
{
	int i;
	printf("---test myrealloc.---\n");
	printf("============================\n");
	int *test_MemoryArray = (int *)mymalloc(sizeof(int) * n);
	for(i = 0 ; i < n ; i++)
		test_MemoryArray[i] = rand() % 99;	
	print_array(test_MemoryArray,n);
	printf("--realloc double space.--\n");
	test_MemoryArray = (int *)myrealloc(test_MemoryArray, sizeof(int) * (n*2));	
	print_array(test_MemoryArray,n*2);
	printf("============================\n");
	myfree(test_MemoryArray);
}
