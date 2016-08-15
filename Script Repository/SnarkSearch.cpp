#include "../LifeAPI.h"

int main () 
{

printf("x = 0, y = 0, rule = B3/S23\n");
	
#pragma omp parallel
{
	New();
   
	LifeState* states[53];
	
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
	states[10] = NewState("bo$obo$o2bo$bobo$2bo!");
	states[11] = NewState("b2o$o2bo$bo2bo$2b2o!");
	states[12] = NewState("2bo$bobo$o2bo$obo$bo!");
	states[13] = NewState("2b2o$bo2bo$o2bo$b2o!");
	states[14] = NewState("2bo$bobo$obo$bo!");
	states[15] = NewState("bo$obo$bobo$2bo!");
	states[16] = NewState("2b2o$bobo$obo$bo!");
	states[17] = NewState("2bo$bobo$obo$2o!");
	states[18] = NewState("bo$obo$bobo$2b2o!");
	states[19] = NewState("2o$obo$bobo$2bo!");
	states[20] = NewState("3bo$2bobo$bobo$obo$bo!");
	states[21] = NewState("bo$obo$bobo$2bobo$3bo!");
	states[22] = NewState("bo$obo$bo!");
	states[23] = NewState("2b2o$3bo$3o$o!");
	states[24] = NewState("2b2o$2bo$obo$2o!");
	states[25] = NewState("o$3o$3bo$2b2o!");
	states[26] = NewState("2o$obo$2bo$2b2o!");
	states[27] = NewState("2o$o$b3o$3bo!");
	states[28] = NewState("2o$bo$bobo$2b2o!");
	states[29] = NewState("3bo$b3o$o$2o!");
	states[30] = NewState("2b2o$bobo$bo$2o!");
	states[31] = NewState("4b2o$3bobo$3b2o$b2o$obo$2o!");
	states[32] = NewState("2o$obo$b2o$3b2o$3bobo$4b2o!");
	states[33] = NewState("b2o$obo$2o!");
	states[34] = NewState("2o$obo$b2o!");
	states[35] = NewState("b2o$bo$2bo$obo$2o!");
	states[36] = NewState("3b2o$2o2bo$ob2o!");
	states[37] = NewState("2o$obo$2bo$bo$b2o!");
	states[38] = NewState("ob2o$2o2bo$3b2o!");
	states[39] = NewState("2o$bo$o$obo$b2o!");
	states[40] = NewState("2o$o2b2o$b2obo!");
	states[41] = NewState("b2o$obo$o$bo$2o!");
	states[42] = NewState("b2obo$o2b2o$2o!");
	states[43] = NewState("5bo$4bobo$3bo2bo$2bob2o$bobo$o2bo$b2o!");
	states[44] = NewState("b2o$o2bo$bobo$2bob2o$3bo2bo$4bobo$5bo!");
	states[45] = NewState("bo$obo$o2bo$b2obo$3bobo$3bo2bo$4b2o!");
	states[46] = NewState("4b2o$3bo2bo$3bobo$b2obo$o2bo$obo$bo!");
	states[47] = NewState("b2o$o2bo$o2bo$b2o!");
	states[48] = NewState("2o$2o!");
	states[49] = NewState("2o$obo$bobo$2b2o!");
	states[50] = NewState("2b2o$bobo$obo$2o!");
	states[51] = NewState("o$o$o!");
	states[52] = NewState("3o!");

	LifeState* init = NewState("2o$2o17b2o$13bo6bo$13bo6bobo$13bo7b2o!", -0, -20);
	
	LifeIterator *iter1 = NewIterator(states, -12, -15, 25, 25, 53);
	LifeIterator *iter2 = NewIterator(states, -12, -15, 25, 25, 53);
	LifeIterator *gld_iter = NewIterator("3o$o$bo!", 15, -13, 1, 42);
	
	LifeTarget* snark = NewTarget("8bo$4b2obobo$3bobobobo$o2bobobobobo$4ob2o3b2o$4bo$2bobo$2b2o!", 11, -20);
	
	do{
    #pragma omp single nowait
	{
		if(Validate(iter1, iter2) != FAIL)
		{
			
			New();
			PutState(init);
			PutState(iter1);
			PutState(iter2);
			PutState(gld_iter);

			int found = NO; 
			for(int i = 0; i < 400; i++)
			{
				Run(1);
				
				if(Contains(snark))
				{
					New();
					PutState(init);
					PutState(iter1);
					PutState(iter2);
					PutState(gld_iter);

					#pragma omp critical
					{
						found = YES; 
						printf(GetRLE(GlobalState));
						printf("100$");
					}
				}
					
				if(found == YES)
					break;
			}
		}
	}
   }while(Next(gld_iter, iter1, iter2, NO));

}
	printf("!");
	printf("\n\nFINISH");
	getchar();
}
