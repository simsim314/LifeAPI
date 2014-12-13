#include "LifeAPI.h"

int main()
{
	New();
	
	LifeState* pat =  NewState("obo$b2o$bo9$4bo$4b2o$3bobo$7b3o$7bo$8bo$14bo$13b2o$13bobo!", 0, -20);

	LifeState* target =  NewState("$b2ob2o$bo3bo$2bobo$b2ob2o3$3bo$2bobo$3bo!", 2, -10);
	LifeState* inverse = NewState("7o$o2bo2bo$ob3obo$2obob2o$o2bo2bo$7o$7o$3ob3o$2obob2o$3ob3o$7o!", 2, -10);
	LifeTarget * fulltarget = NewTarget(target, inverse);
	
	LifeState* gld =  NewState("b2o$obo$2bo!", 0, 0);

	LifeIterator *iter1 = NewIterator(gld, -21, -5, 30, 30);
	LifeIterator *iter2 = NewIterator(gld, -21, -5, 30, 30);
	
	do
	{
		New();
		
		PutState(pat);
		PutState(iter1);
		PutState(iter2);
		
		Print();
		getch();
		Run(100);
		
		if(ContainsTarget(fulltarget) == YES)
		{
			New();
			PutState(pat);
			PutState(iter1);
			PutState(iter2);

			PrintRLE();
			Print();
			
			Run(100);
			
			Print();
			
			getch();
			printf("\Searching");
		}
	}while(Next(iter1, iter2) == SUCCESS);
	
	printf("\nFinished");
	getch();
}