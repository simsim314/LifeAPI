#include "LifeAPI.h"

int GetCPU(int args, const char * argv[])
{
	if(args >= 2)
		return atoi(argv[1]);
	else
		return 0;
}

int main(int args, const char * argv[])
{
	int cpu = GetCPU(args, argv);
	getch();
	
	New();
	
	LifeState* pat =  NewState("18bobo18bobo$19b2o18b2o$19bo20bo2$38bo$36b2o$37b2o3$4bo37bo$5bo35bo$3b3o35b3o4$20b2ob2o$20b2ob2o3$21b2ob2o$21bo3bo$22bobo$21b2ob2o3$23bo$22bobo$23bo$4b3o33b3o$6bo33bo$5bo35bo2$2o43b2o$b2o41b2o$o45bo$9bo27bo$9b2o25b2o$8bobo25bobo2$30b3o$30bo$31bo2$15b3o$17bo$16bo!", -20, -20);
	
	LifeState* gld1 =  NewState("3o$2bo$bo!", -10, -5);
	LifeState* gld2 =  NewState("bo$2bo$3o!", -7, -21);
	LifeState* gld3 =  NewState("obo$b2o$bo!", -12, -19);
	LifeState* gld4 =  NewState("2bo$obo$b2o!!", -17, -21);
	
	LifeIterator *iter1 = NewIterator(gld1, -2, -2, 5, 5, 4);
	LifeIterator *iter2 = NewIterator(gld2, -2, -2, 5, 5, 4);
	LifeIterator *iter3 = NewIterator(gld3, -2, -2, 5, 5, 4);
	LifeIterator *iter4 = NewIterator(gld4, -2, -2, 5, 5, 4);
	
	printf("\nSearching...");
	do
	{
		New();
		
		PutState(pat);
		PutState(iter1);
		PutState(iter2);
		PutState(iter3);
		PutState(iter4);
		
		int collision = NO; 
		int pop = GetPop();
		
		for(int i = 0; i < 6; i++)
		{
			Run(1);
			
			if(GetPop() != pop)
			{
				collision = YES;
				break;
			}
		}
		
		if(collision == YES)
			continue; 
			
		Run(63);
		
		int pop40 = YES; 
		
		//Let's make sure it's dart 
		//it's population will drop to 34 and back to 40 - it's either some crazy p3 oscillator or dart. 
		for(int i = 0; i < 10; i++)
		{
			if(GetPop() != 40)
			{
				pop40 = NO;
				break;
			}
			
			Run(1);
			
			if(GetPop() != 34)
			{
				pop40 = NO;
				break;
			}
			
			Run(2);
		}
		
		if(pop40 == YES)
		{
			printf("\n     Found it!!    \n");
			
			Print();
			
			New();
		
			PutState(pat);
			PutState(iter1);
			PutState(iter2);
			PutState(iter3);
			PutState(iter4);
			
			printf("\n");
			PrintRLE();
			
			printf("\n\n");
			Print();
			
			printf("\n\n\n");
			
			getch();
			printf("\nsearching...");
		}
		
	}while(Next(iter1, iter2, iter3, iter4) == SUCCESS);
	
	printf("\nFinished");
	getch();
}