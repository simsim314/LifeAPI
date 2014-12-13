#include "LifeAPI.h"

int Assert(int counter, int expected, char* testName)
{
	if(counter != expected)
	{
		printf("\n %s Failed", testName);
		return FAIL;
	}
	else
	{
		printf("\n %s Succeeded", testName);
		return SUCCESS;
	}
}


int Test1()
{
	int counter = 0; 
	
	New();
	
	LifeState* blck =  NewState("2o$2o!");
	LifeState* gld =  NewState("2o$obo$o!");
	
	LifeIterator *blckiter1= NewIterator(blck, -10, -10, 20, 10);
	LifeIterator *glditer= NewIterator(gld, -16, 5, 35, 1);

	do
	{
		New();
		PutState(blck);
		PutState(glditer);
		PutState(blckiter1);
		
		int pop = GetPop();
		
		if(pop != 5 + 4 + 4)
			continue;
		
		Run(1);
		
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
				counter++;
		}
	}
	while(Next(blckiter1, glditer, "none") == SUCCESS);
	
	return Assert(counter, 4, "Test1");
}

int Test2()
{
	int counter = 0; 
	
	New();
	
	LifeState* pat =  NewState("5bo$6bo8bo3bo12bo$4b3o2bo3bobo4b2o8b2o$10b2o2b2o3b2o10b2o$9b2o2$28bobo$11b2o15b2o$10bobo16bo$12bo2$5bo27bo$3bobo10b2ob2o12bobo$4b2o10b2ob2o12b2o3$17b2ob2o$17bo3bo$18bobo$17b2ob2o$5b2o25b2o$4bobo25bobo$6bo12bo12bo$18bobo$bo17bo17bo$b2o33b2o$obo33bobo2$9b2o17b2o$10b2o15b2o$9bo19bo$22b2o$22bobo$22bo2$16b2o$15bobo$17bo!", -20, -10);
	LifeState* gld =  NewState("2bo$2o$b2o!",-2, -17);
	
	LifeIterator *gldIter= NewIterator(gld, -20, -20, 50, 50, 4);

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
			counter++;
		
	}while(Next(gldIter) == SUCCESS);
	
	return Assert(counter, 3, "Test2");
}

int Test3()
{
	int counter = 0; 
	
	New();
	
	LifeState* pat =  NewState("obo$b2o$bo9$4bo$4b2o$3bobo$7b3o$7bo$8bo$14bo$13b2o$13bobo!", -20, -20);
	LifeState* target =  NewState("$b2ob2o$bo3bo$2bobo$b2ob2o3$3bo$2bobo$3bo!", -18, -10);
	LifeState* inverse = NewState("7o$o2bo2bo$ob3obo$2obob2o$o2bo2bo$7o$7o$3ob3o$2obob2o$3ob3o$7o!", -18, -10);
	
	LifeTarget * fulltarget = NewTarget(target, inverse);
	LifeState* gld =  NewState("b2o$obo$2bo!", 0, 0);
	
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
		
		//120 should be enough
		Run(120);
		
		//ContainsTarget checks both "on" and "off" cells
		if(ContainsTarget(fulltarget) == YES)
			counter++;
			
	}while(Next(iter1, iter2, "none") == SUCCESS);
	
	return Assert(counter, 2, "Test3");
	
}

int RunTests()
{
	int result = SUCCESS;
	
	if(Test1() == FAIL)
		result = FAIL;
		
	if(Test2() == FAIL)
		result = FAIL;
		
	if(Test3() == FAIL)
		result = FAIL;
	
	return result;
}

int main()
{

	if(RunTests() == SUCCESS)
		printf("\nFinished all UnitTests succesfully");
	else	
		printf("\nFinished, some UnitTests failed");
		
	getch();
}