#include "LifeAPI.h"

void Continue()
{
	printf("Press to continue... \n");
	getch();
}

int main()
{
	//Always start with new
	New();
	
	printf("\n\n================LifeAPISample with explanation: =============");
	printf("\n\n----- Blockic seeds for glider reflection example: ---\n\n\n\n");
	Continue();
	//create block and glider states
	LifeState* blck =  NewState("2o$2o!");
	LifeState* gld =  NewState("2o$obo$o!");

	//create iterators for glider and block (the second block is at (0,0))
	//The iterators replace 3 loops, on x, y, and s - you just use (Next), they also work together. 
	//start at (-10, -19) - iterate x in width of 20, y in width of 10
	LifeIterator *blckiter1= NewIterator(blck, -10, -10, 20, 10);
	
	//start at (-16, 6) - iterate x in width of 35, y remains. 
	LifeIterator *glditer= NewIterator(gld, -16, 5, 35, 1);

	//	Let's measure performance (remove all Continue(); statements to see the real time). 
	clock_t t = clock();

	do
	{
		//clear previous junk from GlobalState 
		New();
		
		//PutState works with LifeState and LifeIterator
		
		//Place block at (0,0)
		PutState(blck);
		
		//place block and glider, by their iterators data. 
		PutState(glditer);
		PutState(blckiter1);
		
		//Get population (to avoid blocks placed on same spot)
		int pop = GetPop();
		
		//block + block + glider
		if(pop != 5 + 4 + 4)
			continue;
		
		Run(1);
		
		//Check if blocks are interfering. 
		if(pop != GetPop())
			continue;
		
		//Just iterate - the glider will run all over tha place - we're on torus anyway 
		Run(180);
		

		//potential glider 
		if(GetPop() == 5)
		{	
			int min = GlobalState->min;
			int max = GlobalState->max;
			
			//evolve 
			Run(16);
			if(GetPop() == 5 && !(min == GlobalState->min && max == GlobalState->max))
			{
				//Success! let's report it here.
				
				//We do the same as we did previously to get here: 
				New();
						
				PutState(glditer);
				PutState(blckiter1);
				PutState(blck);
				
				printf("\n\n       SUCCESS!!    \n\n");
				printf("Here is the blockic seed:    \n\n");				
				//The iterators state is change only on Next. 
				
				Print();
				
				PrintRLE();
				
				printf("\n\nYou can copy-paste the rle into golly (remember to remove \"new line\" symbols)!!    \n\n");
				Continue();
				
			}
		
		}
	}
	while(Next(blckiter1, glditer) == SUCCESS);
	
	t = clock() - t;
	
	printf ("Total time: %d miliseconds\n",t);
	printf("\n\n\n\n\n                 THE END !!!\n\n\n\n\n\n");
	getch();
	return 0;
}
