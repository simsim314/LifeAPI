#include "../mpLifeAPI.h"

int main () 
{

printf("x = 0, y = 0, rule = B3/S23\n");
	
#pragma omp parallel
{
	New();
   
   LifeState* blck =  NewState("2o$o$b3o$3bo!");
   LifeState* gldL =  NewState("3o$2bo$bo!");
   LifeState* gldR =  NewState("bo$o$3o!");
   
   LifeIterator *iterL = NewIterator(gldL, -10, 5, 10, 1);
   
   LifeIterator *iterR1 = NewIterator(gldR, 0, -15, 10, 10, 4);
   LifeIterator *iterR2 = NewIterator(gldR, 0, -15, 10, 10, 4);
   
   int initPop = GetPop(blck);
   

   do{
    #pragma omp single nowait
	{
	
		if(Validate(iterR1, iterR2) != FAIL)
		{
			
			New();
			PutState(blck);
			PutState(iterL);
			PutState(iterR1);
			PutState(iterR2);

			int collide = NO;

			for(int i = 0; i < 4; i++)
			{
				if(GetPop() != 5 * 3 + initPop)
				{
				collide = YES;
				break;
				}
				
				Run(1);
			}

			if(collide != YES)
			{ 
				
				for(int i = 0; i < 300; i++)
				{
					Run(1);
					
					uint64_t gld = LocateAtX(GlobalState, _glidersTarget[0], 2);
					int found = NO;
					
					if(strlen(GlobalState->emittedGliders->value) != 0)
					break;
					
					int gen = GlobalState->gen;
					
					if(gld != 0 && GetPop() == 5)
					{
						found = YES;
						
						for(int j = 0; j < 4; j++)
						{
								 if(GlobalState->gen%4 == 0)
									break;
									
								 Run(1);
						}
						
						Capture(0);
						Move(Captures[0], (GlobalState->gen) / 4 + 4, (GlobalState->gen) / 4 + 4);
						Evolve(Captures[0], 2);
						
						New();
						PutState(blck);
						PutState(iterL);
						PutState(iterR1);
						PutState(iterR2);
						PutState(Captures[0]);
						
						Run(gen);
						
						uint64_t gld = LocateAtX(GlobalState, _glidersTarget[0], 2);
						
						if(gld != 0 && GetPop() == 10)
					{
							 New();
							 PutState(blck);
							 PutState(iterL);
							 PutState(iterR1);
							 PutState(iterR2);
							 PutState(Captures[0]);
							 
							#pragma omp critical
							{
								printf(GetRLE(GlobalState));
								printf("100$");
							}
						}
					}

					if(found == YES)
					break;

				}
			}
		}
	}
   }while(Next(iterL, iterR1, iterR2, NO));

}
	printf("!");
	printf("\n\nFINISH");
	getchar();
}
