#include "../LifeAPI.h"

int main () 
{

printf("x = 0, y = 0, rule = B3/S23\n");
	
#pragma omp parallel
{
	New();
   
	LifeState* states[37];
	
   	states[0] = NewState("b2o$obo$bo!");
	states[1] = NewState("bo$obo$2o!");
	states[2] = NewState("bo$obo$b2o!");
	states[3] = NewState("2o$obo$bo!");
	states[4] = NewState("bo$obo$obo$bo!");
	states[5] = NewState("b2o$o2bo$b2o!");
	states[6] = NewState("b2o$o2bo$bobo$2bo!");
	states[7] = NewState("bo$obo$o2bo$b2o!");
	states[8] = NewState("2bo$bobo$o2bo$b2o!");
	states[9] = NewState("b2o$o2bo$obo$bo!");
	states[10] = NewState("2bo$bobo$o2bo$obo$bo!");
	states[11] = NewState("2b2o$bo2bo$o2bo$b2o!");
	states[12] = NewState("bo$obo$o2bo$bobo$2bo!");
	states[13] = NewState("b2o$o2bo$bo2bo$2b2o!");
	states[14] = NewState("2o$obo$bobo$2bo!");
	states[15] = NewState("2bo$bobo$obo$2o!");
	states[16] = NewState("2b2o$bobo$obo$bo!");
	states[17] = NewState("bo$obo$bobo$2b2o!");
	states[18] = NewState("bo$obo$bobo$2bobo$3bo!");
	states[19] = NewState("3bo$2bobo$bobo$obo$bo!");
	states[20] = NewState("bo$obo$bobo$2bo!");
	states[21] = NewState("2bo$bobo$obo$bo!");
	states[22] = NewState("bo$obo$bo!");
	states[23] = NewState("4b2o$3bobo$3b2o$b2o$obo$2o!");
	states[24] = NewState("2o$obo$b2o$3b2o$3bobo$4b2o!");
	states[25] = NewState("b2o$obo$2o!");
	states[26] = NewState("2o$obo$b2o!");
	states[27] = NewState("5bo$4bobo$3bo2bo$2bob2o$bobo$o2bo$b2o!");
	states[28] = NewState("b2o$o2bo$bobo$2bob2o$3bo2bo$4bobo$5bo!");
	states[29] = NewState("bo$obo$o2bo$b2obo$3bobo$3bo2bo$4b2o!");
	states[30] = NewState("4b2o$3bo2bo$3bobo$b2obo$o2bo$obo$bo!");
	states[31] = NewState("b2o$o2bo$o2bo$b2o!");
	states[32] = NewState("2o$2o!");
	states[33] = NewState("2o$obo$bobo$2b2o!");
	states[34] = NewState("2b2o$bobo$obo$2o!");
	states[35] = NewState("o$o$o!");
	states[36] = NewState("3o!");

	
	LifeIterator *iter1 = NewIterator(states, -15, -15, 30, 30, 37);
	LifeIterator *iter2 = NewIterator(states, -15, -15, 30, 30, 37);
	LifeIterator *iter3 = NewIterator(states, 0, -6, 1, 10, 37);
	LifeState *gld = NewState("3o$o$bo!", 18, 18);
	TargetLocator* locator = NewTargetLocator("2b2o$bobo$bo$2o!");
	
	do{
    #pragma omp single nowait
	{
		if(Validate(iter1, iter2, iter3) != FAIL)
		{
			
			New();
			
			PutState(iter1);
			PutState(iter2);
			PutState(iter3);
			PutState(gld);
			int pop = GetPop();
			
			for(int i = 0; i < 4; i++)
			{
				Run(1);
				
				if(pop != GetPop())
				{
					pop = -1; 
					break;
				}
			}
			
			if(pop > 0)
			{
				int fail = NO; 
				
				for(int i = 0; i < 30; i++)
				{
					Capture(1);
					
					Run(10);
					
					if(AreEqual(1))
						break; 
					
					int pop = GetPop();
					
					if(pop > 50 || pop == 0)
					{
						fail = YES;
						break;
					}	
				}
					
				if(fail == NO && GetPop() == 7 && ContainsLocator(locator) == YES)
				{
					New();
						
					PutState(iter1);
					PutState(iter2);
					PutState(iter3);
					PutState(gld);
					
					#pragma omp critical
					{
						printf(GetRLE(GlobalState));
						printf("100$");
					}
				}
			}
		}
	}
   }while(Next(iter1, iter2, iter3, NO));

}
	printf("!");
	printf("\n\nFINISH");
	getchar();
}
