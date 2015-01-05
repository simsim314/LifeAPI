#include "../mpLifeAPI.h"

int main (int argc, char *argv[]) 
{
	
printf("x = 0, y = 0, rule = B3/S23\n");

#pragma omp parallel
{
	New();
   
   LifeState* blck =  NewState("2o$2o!");
   LifeState* pat =  NewState("b3o$bo$3o!");
   
   LifeIterator *iters[3];
   LifeTarget* target = NewTarget(blck);
   int activated[3];
   
   
	for(int i = 0; i < 3; i++)
		iters[i] = NewIterator(blck, -5, 0, 20, 20);
	
   do{
    #pragma omp single nowait
	{
		int valid = Validate(iters, 3); 
		
		if(valid == YES)
		{
			New();
			
			for(int i = 0; i < 3; i++)
			{
				PutState(iters[i]);
			}

			Run(1);

			int collide = NO;
			
			for(int i = 0; i < 3; i++)
			{
				if(Contains(GlobalState, target, iters[i]->curx, iters[i]->cury) == NO)
				{
					collide = YES;
					break;
				}
			}
			
			if(collide == NO)
			{
				PutState(pat);
				
				for(int i = 0; i < 3; i++)
				{
					if(Contains(GlobalState, target, iters[i]->curx, iters[i]->cury) == NO)
					{
						collide = YES;
						break;
					}
				}
			}			
			
			if(collide == NO)
			{ 
				for(int i = 0; i < 3; i++)
					activated[i] = NO;
				
				int surviveCount = -1;
				
				for(int i = 0; i < 250; i++)
				{
					Run(1);
					
					for(int j = 0; j < 3; j++)
					{
						if(Contains(GlobalState, target, iters[j]->curx, iters[j]->cury) == NO)
						{
							activated[j] = YES;
						}
					}		
					
					int isAllActivated = YES;
					
					for(int j = 0; j < 3; j++)
					{	
						if(activated[j] == NO)
						{
							isAllActivated = NO;
							break;
						}
					}
					
					if(isAllActivated == YES)
					{
						int survive = YES;
						
						for(int j = 0; j < 3; j++)
						{
							if(Contains(GlobalState, target, iters[j]->curx, iters[j]->cury) != YES)
							{
								survive = NO;
								surviveCount = -1;
								break;
							}
						}	
						
						if(survive == YES)
							surviveCount++;
							
						if(surviveCount > 15)
						{
							New();
								
							for(int j = 0; j < 3; j++)
							{
								PutState(iters[j]);
							}

							PutState(pat);
								
							#pragma omp critical
							{
								printf(GetRLE(GlobalState));
								printf("100$");
							}
							
							break;
						}
					}
				}
			}
		}
	}
   }while(Next(iters, 3, NO));

}
	printf("!");
	printf("\n\nFINISH");
	getchar();
}
