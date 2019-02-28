#include "..\LifeAPI.h"

int main()
{
	New();
	
	LifeState* snakePair = NewState("2ob2o$o3bo$bobo$2ob2o!", -15, -7);
	LifeTarget* target = NewTarget(snakePair);
	
	//For lower part
	LifeState* synth = NewState("obo$b2o$bo13$4bo$4b2o$3bobo$11b3o$11bo$12bo$18bo$17b2o$17bobo!", -20, -20);
	LifeState* gld = NewState("bo$2o$obo!", -6, 13, 1, 0, 0, 1);
	LifeIterator* iter = NewIterator(gld, -30, -30, 60, 60, 4);
	LifeTarget* targetEmpty = NewTarget(Captures[1], NewState("5o2bo2b5o$5ob3ob5o$6obob6o$5o2bo2b5o$15o$15o$15o$15o$15o$15o$15o$15o$15o$15o$15o$15o!", -20, -7));

	//For upper part
	//LifeState* synth = NewState("obo$b2o$bo13$4bo$4b2o$3bobo$11b3o$11bo$12bo$18bo$17b2o$17bobo5$11bo$10b2o$10bobo!", -20, -20);
	//LifeTarget* targetEmpty = NewTarget(Captures[1], NewState("15o$15o$15o$15o$15o$15o$15o$15o$15o$15o$15o$5o2bo2b5o$5ob3ob5o$6obob6o$5o2bo2b5o!", -20, -18));
	//LifeState* gld = NewState("2bo$2o$b2o!", -2, -23, 1, 0, 0, -1);
	//LifeIterator* iter = NewIterator(gld, -30, -10, 60, 60, 4);
	
	do
	{
		New();
		
		PutState(synth);
		PutState(iter);
		
		
		Run(210);
		Capture(0);
		Run(2);
		
		//if(ContainsTarget(targetEmpty) == NO && ContainsTarget(target) == YES && GetPop() != 12 + 5)
		if(ContainsTarget(targetEmpty) == NO && ContainsTarget(target) == YES)// && !AreEqual(0))
		{
			Print();
			
			New();
			
			PutState(synth);
			PutState(iter);
			
			PrintRLE();
			//Print();
			
			printf("\nSUCCESS\n");
			getchar();
			printf("\nSearching\n");
			
		}
	}
	while(Next(iter) == SUCCESS);
	
	printf("\nFinish\n");
	
	getchar();
}