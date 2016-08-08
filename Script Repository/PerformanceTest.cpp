#include "../LifeAPI.h"
#include <ctime>
#include <iostream>
#include <omp.h>

int main()
{
	long long evolved_cells = 0; 
	clock_t begin;
	omp_set_num_threads(8);
	
	#pragma omp parallel
	{
		New();

		LifeState* snark = NewState("3o$o4bo$obobo$obo2bo$o$ob3o!", -3, -3);
		
		New();
		PutState(snark);

		int cycle = 0;

		for (int i = 0; i < 1243; i++)
		{
			Run(1);
			cycle += (64 * (2 + (GlobalState->max - GlobalState->min)));
		}

		#pragma omp single
		begin = clock();

		#pragma omp for 
		for (int iter_ix = 0; iter_ix < 30000; iter_ix++)
		{
			New();
			PutState(snark);
			Run(1243);
			
			#pragma omp critical
			evolved_cells += cycle; 
		}
	}
	
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

	std::cout << (evolved_cells / 1000000000) / elapsed_secs << " BCO/s \n";
	getchar();
}