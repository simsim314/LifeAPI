#include "../LifeAPI.h"

int GetCPU(int args, const char * argv[])
{
	if(args >= 2)
		return atoi(argv[1]);
	else
		return 0;
}

int IsCollision()
{
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

	return collision;
}
	
int IsDart()
{
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

	return 	pop40;
}

int main(int args, const char * argv[])
{
	int cpu = GetCPU(args, argv);
	
	New();
	
	LifeState* pat =  NewState("14bo$15bo13bobo$13b3o13b2o$30bo$3bobo5bo$4b2o3bobo16bo$4bo5b2o14b2o$27b2o3$4bo4b2o4bo3bo12bo$5bo4b2o4bobo12bo$3b3o3bo5b2ob2o11b3o3$16b2ob2o$16bo3bo$17bobo$16b2ob2o$4b3o23b3o$6bo23bo$5bo12bo12bo$17bobo$2o16bo16b2o$b2o31b2o$o35bo$9bo17bo$9b2o15b2o$8bobo15bobo2$20b3o$20bo$21bo2$15b3o$17bo$16bo!", -10, -10);
	LifeState* gld =  NewState("obo$b2o$bo!", 8, -9);

	LifeIterator *iter = NewIterator(gld, -20, -20, 40, 40, 4);
	
	printf("\nSearching...");	

	do
	{
		New();
		
		PutState(pat);
		PutState(iter);
			
		Run(37);
		
		if(IsDart() == YES)
		{
			printf("\n     Found it!!    \n");
			
			Print();
			
			New();
		
			PutState(pat);
			PutState(iter);
		
			printf("\n");
			PrintRLE();
			
			printf("\n\n");
			Print();
			
			printf("\n\n\n");
			Print(iter);
			
			getch();
			printf("\nsearching...");
		}
		
	}while(Next(iter) == SUCCESS);
	
	printf("\nFinished");
	getch();
}