#include "LifeAPI.h"
#include <string.h>

int Assert(int counter, int expected, const char* testName)
{
	if(counter != expected)
	{
		printf("\n\n %s Failed :(\n", testName);
		return FAIL;
	}
	else
	{
		printf("\n\n %s Succeeded!!\n", testName);
		
		return SUCCESS;
	}
}

int Assert(char* counter, char* expected, const char* testName)
{
	if(strcmp(counter, expected) != 0)
	{
		printf("\n\n %s Failed :(\n", testName);
		return FAIL;
	}
	else
	{
		printf("\n\n %s Succeeded!!\n", testName);
		
		return SUCCESS;
	}
}



int Test1()
{
	int counter = 0; 
	
	printf("\n Tests Life iteration");
	
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
		
		//Just iterate - the glider will run all over the place - we're on torus anyway 
		Run(200);
		
		if(GetPop() == 0 && strlen(GlobalState->emittedGliders->value) > 0)
		{	
			counter++;
		}
	}
	while(Next(blckiter1, glditer, NO) == SUCCESS);
	
	return Assert(counter, 4, "Test1");
}

int Test2()
{
	int counter = 0; 
	
	printf("\n Tests Life iteration");
	
	
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
	printf("\n Test advanced target matching");
	
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
		
		//60 should be enough
		Run(60);
		
		//ContainsTarget checks both "on" and "off" cells
		if(Contains(fulltarget) == YES)
			counter++;
			
	}while(Next(iter1, iter2, NO) == SUCCESS);
	
	return Assert(counter, 2, "Test3");
	
}

int Test4()
{
	printf("\n Tests Boundary");
	
	New();
	
	LifeState* pat =  NewState("2o$2o2$2o$2o!", -20, -20);
	
	PutState(pat);
	GetBoundary(1);
	
	return Assert(GetPop(1), 20, "Test4");
	
}

int Test5()
{
	printf("\n RLE Parsing");
	
	New();
	
	LifeState* pat =  NewState("2o$2o2$2o$2o!", -32, -32);

	return Assert(strcmp(GetRLE(pat), "2o$2o2$2o$2o"), 0, "Test5");
	
}

int Test6()
{
	printf("\n Save-Load Results test");
	
	LifeState* pat = NewState("3o$o$bo!");
	LifeResults* results = NewResults();
	New();
	PutState(pat);
	
	for(int i = 0; i < 15; i++)
	{
		Add(results);
		Run(1);
	}

	SaveResults(results, "results.txt");
	LifeResults*  loaded = LoadResults("results.txt");

	return (Assert(results->size, 15, "Test6 Part 1") == SUCCESS) && (Assert(AreEqual(results->results[7], loaded->results[7]), YES, "Test6 Part 2") == SUCCESS);
}

int Test7()
{
	printf("\n Locators basic test");
	
	LifeState* pat = NewState("3o$o$bo!");
	LifeState* result = NewState();
	
	TargetLocator* locator = NewTargetLocator("3o$o$bo!");
	
	New();
	PutState(pat);
	PutState(pat, 10, 10);
	
	LocateTarget(locator, result);
	
	return (Assert(GetCell(result, 0, 0) * GetCell(result, 10, 10), 1, "Test7") == SUCCESS);
}

int Test8()
{
	printf("\n Gliders removal basic");
	
	LifeState* pat = NewState("3o$o$bo!");
	
	New();
	PutState(pat);
	PutState(pat, 15, 6, -1, 0, 0, 1);
	PutState(pat, 12, 17, -1, 0, 0, -1);
	PutState(pat, -13, 11, 1, 0, 0, -1);
	Run(150);
	
	return (Assert(GlobalState->emittedGliders->value, "(2,22,62,62)(1,60,74,1)(3,2,74,62)(0,0,126,1)", "Test8") == SUCCESS);
}


int RunTests()
{
	int result = SUCCESS;
		
	if(Test1() == FAIL)
		result = FAIL;
		
	if(Test2() == FAIL)
		result = FAIL;
		
	if(Test4() == FAIL)
		result = FAIL;
	
	if(Test5() == FAIL)
		result = FAIL;
		
	if(Test6() == FAIL)
		result = FAIL;
	
	if(Test7() == FAIL)
		result = FAIL;
	
	if(Test8() == FAIL)
		result = FAIL;
	
	//Slow but basic test, will run at the end
	if(Test3() == FAIL)
		result = FAIL;
	
	return result;
}

int main()
{

	if(RunTests() == SUCCESS)
		printf("\n\n ==========Finished all UnitTests succesfully!! Horray!! ===========");
	else	
		printf("\n\n ==========Finished, some UnitTests failed :( Please fix them ======");
		
	getchar();
}