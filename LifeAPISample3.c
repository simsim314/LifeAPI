#include "LifeAPI.h"

void Continue()
{

	printf("Press to continue... \n");
	getchar();
}

int main()
{
	printf("\n\n================LifeAPISample with explanation: =============\n\n");
	printf("\n\n           Target with two gliders search example: \n\n");
	
	//Always start with New();
	New();
	
	//Initial pattern
	LifeState* pat =  NewState("obo$b2o$bo9$4bo$4b2o$3bobo$7b3o$7bo$8bo$14bo$13b2o$13bobo!", -20, -20);
	
	//target and inverse target
	LifeState* target =  NewState("$b2ob2o$bo3bo$2bobo$b2ob2o3$3bo$2bobo$3bo!", -18, -10);
	LifeState* inverse = NewState("7o$o2bo2bo$ob3obo$2obob2o$o2bo2bo$7o$7o$3ob3o$2obob2o$3ob3o$7o!", -18, -10);
	
	//Life target object contains the on and the off cells
	LifeTarget * fulltarget = NewTarget(target, inverse);
	
	//glider with (+1, -1) direction at (0,0)
	LifeState* gld =  NewState("b2o$obo$2bo!", 0, 0);
	
	New();
	PutState(pat);
	Print();
	printf("\n\n We have durty snake pair synthesis \n\n");
	Continue();
	Run(21);
	Print();
	Continue();
	printf("\n\n It destoryed after 2 generations \n\n");
	Run(2);
	Print();
	Continue();
	
	printf("\n\n We want to place 2 gliders to reach this configuration \n\n");
	
	New();
	PutState(target);
	Print();
	
	Continue();

	New();
	PutState(inverse);
	Print();
	
	printf("\n\n While all these cells should be off \n\n");
	Continue();

	printf("\n\nSearching... \n\n");
	
	//Gldier iterators always have 4 states
	LifeIterator *iter1 = NewIterator(gld, -27, 2, 15, 15, 4);
	LifeIterator *iter2 = NewIterator(gld, -27, 2, 15, 15, 4);
	
	do
	{
		if(Validate(iter1, iter2) == FAIL)
			continue; 
			
		New();
		
		PutState(pat);
		PutState(iter1);
		PutState(iter2);
		
		//100 should be enough
		Run(100);
		
		//Contains checks both "on" and "off" cells
		if(Contains(fulltarget) == YES)
		{
			printf("\nFound!\n\n");
			
			New();
			
			PutState(pat);
			PutState(iter1);
			PutState(iter2);

			PrintRLE();
			printf("\n\n");
			
			Print();
			
			Run(100);
			
			Print();
			
			Continue();
			printf("\Searching...\n\n");
		}
	}while(Next(iter1, iter2, NO) == SUCCESS);
	
	printf("\nFinished");
	getchar();
}