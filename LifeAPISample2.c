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
	
	//Initialize all needed life object from rle
	LifeState* pat =  NewState("5bo$6bo8bo3bo12bo$4b3o2bo3bobo4b2o8b2o$10b2o2b2o3b2o10b2o$9b2o2$28bobo$11b2o15b2o$10bobo16bo$12bo2$5bo27bo$3bobo10b2ob2o12bobo$4b2o10b2ob2o12b2o3$17b2ob2o$17bo3bo$18bobo$17b2ob2o$5b2o25b2o$4bobo25bobo$6bo12bo12bo$18bobo$bo17bo17bo$b2o33b2o$obo33bobo2$9b2o17b2o$10b2o15b2o$9bo19bo$22b2o$22bobo$22bo2$16b2o$15bobo$17bo!", -20, -10);
	LifeState* gld =  NewState("2bo$2o$b2o!",-2, -17);
	
	
	printf("\n\n================LifeAPISample with explanation: =============\n\n");
	printf("\n\n           Clean up glider search example: \n\n");
	
	printf("\n\nWe have the following syntheses: \n\n");
	PutState(pat);
	
	Print();
	Continue();
	Run(28);
	Print();
	printf("After 28 generations it's almost working but needs a clean up glider: \n");
	Continue();
	
	//the glider is place at some reasonable place
	//It's just one glider, we don't think hard
	//Start to iterate the glider at (-20, -20) - go 50 on x and 50 on y. Use 4 states its evolution.
	LifeIterator *gldIter= NewIterator(gld, -20, -20, 50, 50, 4);

	New();
	
	PutState(pat);
	PutState(gldIter);
	
	//we check everything looks nice
	Print();
	
	printf("We add glider\n");
	Continue();
	
	printf("\n Searching... \n");
	
	do
	{
		New();
		
		PutState(pat);
		PutState(gldIter);
		
		int pop40 = YES; 
		
		Run(45);
		
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
			Continue();
			
			New();
			
			PutState(pat);
			PutState(gldIter);
			
			printf("\n");
			PrintRLE();
			
			printf("You can copy-paste the rle into golly (remember to remove \"new line\" symbols)!!    \n\n");
			Continue();
			
			Print();
			printf("\n\nThis was the initial pattern");
			
			printf("\n\n\n");
			Continue();
			
		}
		
	}while(Next(gldIter) == SUCCESS);
	
	printf("\n\nFINISH");
	Continue();
}