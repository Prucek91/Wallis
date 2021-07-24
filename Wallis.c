#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>


double liczba_pi = 1;

pthread_mutex_t mutex =  PTHREAD_MUTEX_INITIALIZER;  //pthread_mutex_initializer;

struct thread_data
{
  int n;
  int begin;
  int end;
}   td[100];


pthread_t threads[100]; // tablica watkow


void * obliczenia(void * dane);



double wallis(int val)
{
	double tmp;
	double dval = (double)val;
	tmp = 2 * dval * 2 * dval;
	double m1 = (2 * dval) - 1;
	double m2 = (2 * dval) + 1;
	double m3 = m1 * m2;

	double x = tmp / m3;

//	printf(" %f ", x);
	if (x <=0) x = 1;
	return x;
}



int main(int ac, char**av)
{
    int n;
    int w;

    struct thread_data d;


    if(ac!=3)
    {
        puts("Niepoprawna ilosc argumentow.\n");
        return -1;
    }



    n = atoi(av[1]);
    if ( n < 0 || n > 1000000000 )
    {
        printf("Niepoprawna wartosc n: %d\n", n);
        return -1;
    }

    w = atoi(av[2]);
    if (w < 0 || w > 100)
    {
        printf("Niepoprawna wartosc w: %d\n", w);
        return -1;
    }


    int liczby_na_watek = n / w;
    int pozostale = n - ( liczby_na_watek * w );


// do pomiaru czasu
    clock_t start, stop;
    double czas;

    // odpalamy watki 

	
        start = clock();

	for (int ii = 0; ii < w; ii++)
	{
	    td[ii].n = liczby_na_watek;
            td[ii].begin = ii * liczby_na_watek;
            td[ii].end = (ii+1) * liczby_na_watek - 1;

	    if ( ii == w - 1 )
	    {
	        td[ii].n = liczby_na_watek + pozostale;
		td[ii].begin = ii * liczby_na_watek;
		td[ii].end = n-1;
	    }
//            pthread_create( &thread_0, NULL,obliczenia, &d0 );
	      pthread_create( &threads[ii], NULL, obliczenia , &td[ii] );
//            pthread_join( thread_0, NULL );
	}

	for (int ii = 0; ii < w; ii++)
	{
	    pthread_join( threads[ii], NULL);
	}

	stop = clock();

    czas = ((double)(stop - start)) / CLOCKS_PER_SEC;
    printf("Czas obliczen: %f s.\n", czas);
    printf("Liczba pi: %f\n", liczba_pi*2);


	double pi_bez_watkow = 1;
	start = clock();
	for (int i = 0; i < n; i++)
	{
		pi_bez_watkow *= wallis(i);
	}

	stop = clock();
        czas = ((double)(stop - start)) / CLOCKS_PER_SEC;
	printf("Bez uzycia watkow: \n");
        printf("Czas obliczen: %f s.\n", czas);
        printf("Liczba pi: %f\n", pi_bez_watkow*2);

    return 0;
}








void * obliczenia(void * thread_data)
{
//    int koniec_ = (( struct thread_data *) thread_data)->koniec;

	int n = (( struct thread_data *) thread_data)->n;
	int begin = (( struct thread_data *) thread_data)->begin;
	int end = (( struct thread_data * ) thread_data)->end;

//	wypisywanie danych: numer watku, n
	int id = (int) syscall(__NR_gettid);
//	printf("ID watku: %d\tn: %d\tPierwszy indeks: %d\n",id, n, begin );

	double tmp = 1;
	for ( int i = begin; i < end; i++)
	{
		tmp *= wallis( i );
	}

	pthread_mutex_lock( &mutex  );

	liczba_pi *= tmp;

	pthread_mutex_unlock ( &mutex );
	//mutexy, aktualizacja zmiennej globalnej

}

