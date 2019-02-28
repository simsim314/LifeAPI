#include "..\LifeAPI.h"

void PlaceIterator(LifeIterator* iter, LifeIterator* iter2)
{
	PutState(iter);
	PutState(iter2->States[iter->curs], -iter->curx, iter->cury);
}

int main()
{
	
	ResultManager *result = NewResults();
	
	New();
	int dx = -2; 
	int numGliders = 2;
	
	LifeState* pat = NewState("2ob2o$o3bo$bobo$2ob2o4$2bo$2bo$2bo!", dx, -20);
	LifeTarget* target = NewTarget("b2ob2o$bo3bo$2bobo$b2ob2o$o5bo$b2ob2o!", dx-1, -20);
	LifeTarget* initTarget = NewTarget("2ob2o$o3bo$bobo$2ob2o4$2bo$2bo$2bo!", dx, -20);
	LifeTarget* snakes = NewTarget("2ob2o$o3bo$bobo$2ob2o4!", dx, -20);
	
	LifeState* gld = NewState("bo$2o$obo!");
	LifeState* gld1 = NewState("bo$2o$obo!", 0, 0, -1, 0, 0, 1);
	
	LifeIterator* iter1 = NewIterator(gld, -10, 20, 40, 1, 4);
	LifeIterator* iter2 = NewIterator(gld1, -30, -10, 50, 40, 4);
	
	LifeIterator* iterMirrorX = NewIterator(gld1, 5, 0, 30, 30, 4);
	
	PutState(pat);
	int initPop = GetPop();

	do
	{
		//if(Validate(iter1, iter2) == FAIL)
		//	continue; 
			
		New();
		
		PutState(pat);
		PutState(iter1);
		PutState(iter2);
		
		int collide = NO;
		
		for(int i = 0; i < 6; i++)
		{
			Run(1);
			
			if(GetPop() != 5 * numGliders + initPop)
			{
				collide = YES;
				break;
			}
		}
		
		if(collide == YES)
			continue;
			
		Capture(3);
		int sym = NO; 
		
		for(int i = 0; i < 210; i++)
		{
			Capture(i % 2);
			Run(1);

			if(AreEqual((i + 1)%2))
				break;
			
			//if(i % 5 == 0)
			{	
				Capture(4);
				FlipX(4);
			
				if(GetPop() != 5 * numGliders + initPop && AreEqual(4) == YES)
				{
					sym = YES;
					break;
				}
			}
		}
		
		
		if(Contains(initTarget) && GetPop() == initPop)
			continue; 

		Run(1);
		
		if(Contains(initTarget) && GetPop() == initPop)
			continue; 
		
		
		if(sym == YES && Contains(snakes) && (GlobalState->state[0] == 0) && (GlobalState->state[N - 1] == 0))
		{
			int contains = NO;
			
			for(int i = 0; i < result->size; i++)
			{
				if(AreEqual(result->results[i]))
				{
					contains = YES;
					break;
				}
			}
			
			if(contains == NO)
			{
				Add(result);
				Print(iter2);
				Print(iter1);
				
				printf("\n\n");
			}
			
			
		}
		
		/*
		
		
			if(ContainsTarget(target))
			{
				Print();
				
				Print(3);
				
				PrintRLE(3);
				
				printf("\nSUCCESS!!\n");
				getchar();
			}
			
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
		*/
	}
	while(Next(iter2, iter1) == SUCCESS);
	
	for(int i = 0; i < result->size; i++)
	{
		New();
		PutState(result->results[i]);
		Print();
		getchar();
	}
	printf("\nFinish\n");
	
	getchar();
}