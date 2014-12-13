//LifeAPI provide comfortable functions (API) to manipulate, iterate, evolve, compare and report Life objects. This is mainly done
//in order to provide fast (using C) but still comfortable search utility. 
//Written by Michael Simkin 2014

#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define N 64
#define CAPTURE_COUNT 10 
#define MAX_ITERATIONS 200

#define SUCCESS 1
#define FAIL 0

#define YES 1
#define NO 0


#ifdef __MSC_VER
#  include <intrin.h>
#  define __builtin_popcount __popcnt64
#endif

typedef struct 
{
	int min;
	int max; 
	uint64_t  state[N];
	
} LifeState;

inline uint64_t CirculateLeft(uint64_t x);
inline uint64_t CirculateRight(uint64_t x);
static LifeState* GlobalState;
static LifeState* Captures[CAPTURE_COUNT];
static LifeState* Temp;


inline uint64_t CirculateLeft(uint64_t x)
{
	return (x << 1) | (x >> (63));
}

inline uint64_t CirculateLeft(uint64_t x, int k)
{
	return (x << k) | (x >> (64 - k));
}

inline uint64_t CirculateRight(uint64_t x, int k)
{
	return (x >> k) | (x << (64 - k));
}

inline uint64_t CirculateRight(uint64_t x)
{
	return (x >> 1) | (x << (63));
}

void Set(int x, int y, uint64_t  *state)
{
	state[x] |= (1ULL << (y));
}

void Erase(int x, int y, uint64_t  *state)
{
	state[x] &= ~(1ULL << (y));
}

int Get(int x, int y, uint64_t  *state)
{
	return (state[x] & (1ULL << y)) >> y;
}

void SetCell(LifeState* state, int x, int y, int val)
{
	
	if(val == 1)
	{
		Set((x + 32) % N, (y + 32) % 64, state->state);
	}
	if(val == 0)
		Erase((x + 32) % 64, (y + 32) % 64,  state->state);
}

int GetCell(LifeState* state, int x, int y)
{
	return Get((x + 32) % 64, (y + 32) % 64, state->state);
}

int GetCell(int x, int y, int val)
{
	return GetCell(GlobalState, x, y);
}

void SetCell(int x, int y, int val)
{
	SetCell(GlobalState, x, y, val);
}


void ExpandMinMax(int* min, int* max)
{
	*min = *min - 2;
	*max = *max + 2;
	
	if(*min < 0 || *max >= N)
	{
		(*min) = 0;
		(*max) = N - 1;
		
	}
	
}

void Print(LifeState *state)
{
	int i, j;
	
	for(i = 0; i < N; i++)
	{
		for(j = 0; j < 64; j++)
		{
			if(GetCell(state, j - 32, i - 32) == 0)
			{
				int hor = 0;
				int ver = 0;
				
				if((i - 32) % 10 == 0)
					hor = 1;
				
				if((j - 32) % 10 == 0)
					ver = 1;
								
				if(hor == 1 && ver == 1)
					printf ("+");
				else if(hor == 1)
					printf ("-");
				else if(ver == 1)
					printf ("|");
				else
					printf (".");
			}
			else
				printf ("O");
		}
		printf("\n");
	}
	
	printf("\n\n\n\n\n\n");
	
}


void Copy(LifeState* main, LifeState* delta, char* op)
{
	if(strcmp(op,"copy") == 0)
	{	
		for(int i = 0; i < N; i++)
			main->state[i] = delta->state[i];
	}
	if(strcmp(op,"or") == 0)
	{	
		for(int i = 0; i < N; i++)
			main->state[i] |= delta->state[i];
	}
	if(strcmp(op,"and") == 0)
	{	
		for(int i = 0; i < N; i++)
			main->state[i] &= delta->state[i];
		
	}
	if(strcmp(op,"xor") == 0)
	{	
		for(int i = 0; i < N; i++)
			main->state[i] ^= delta->state[i];
	}
	
	main->min = 0;
	main->max = N - 1;
}

void Copy(LifeState* main, LifeState* delta)
{
	Copy(main, delta, "copy");
}

int GetPop(LifeState* state)
{
	int pop = 0;
	
	for(int i =0; i <= N - 1; i++)
	{
		
		if(state->state[i] != 0)
			pop += __builtin_popcountll(state->state[i]);
	}
	
	return pop;
}

void FitMinMax(LifeState* state)
{
	state-> min = N - 1;
	state-> max = 0;
	
	for(int i = 0; i < N; i++)
	{
		if(state->state[i] != 0)
		{
			state-> min = i;
			break;
		}
	}
	
	for(int i = N - 1; i >= 0; i--)
	{
		if(state->state[i] != 0)
		{
		
			state-> max  = i;
			break;
		}
	}
	
	ExpandMinMax(&(state-> min), &(state-> max));	
}

int GetPop()
{
	return GetPop(GlobalState);
}

void Inverse(LifeState* state)
{	
	for(int i = 0; i < N; i++)
	{
		state->state[i] = ~(state->state[i]);
	}
}

void ClearData(LifeState* state)
{
	int i; 
	
	for(i = 0; i < N; i++)
		state->state[i] = 0;
	
	state -> min = 0;
	state -> max = N - 1;	
}

int AreEqual(LifeState* pat1, LifeState* pat2)
{
	for(int i = 0; i < N; i++)
		if(pat1->state[i] != pat2->state[i])
			return NO;
			
	return YES;
}


int ContainsInverse(LifeState* main, LifeState* inverseSpark)
{
	ClearData(Temp);
	Copy(Temp, main);
	Inverse(Temp);
	Copy(Temp, inverseSpark, "and");
	return AreEqual(Temp, inverseSpark);
}


int Contains(LifeState* main, LifeState* spark)
{
	ClearData(Temp);
	Copy(Temp, main);
	Copy(Temp, spark, "and");
	
	return AreEqual(Temp, spark);
}

int Contains(LifeState* spark)
{
	return Contains(GlobalState, spark);
}

int ContainsInverse(LifeState* inverseSpark)
{
	return ContainsInverse(GlobalState, inverseSpark);
}
LifeState* NewState()
{
	LifeState* result = (LifeState*)(malloc(sizeof(LifeState)));
	ClearData(result);
	
	return result;
}

void IterateState(uint64_t  *state, int min, int max)
{
	uint64_t tempxor[N];
	uint64_t tempand[N];

	uint64_t tempState[N];

	uint64_t l, r, temp;
	uint64_t x0, r0, xU, aU, xB, aB;
	uint64_t a0,a1,a2,c, b0, b1, b2;

	int i, j, idxU, idxB, minNot0, maxNot0;
	
	for(i = min; i <= max; i++)
	{
		temp = state[i];
		l = CirculateLeft(temp);
		r = CirculateRight(temp);
		tempxor[i] = l ^ r ^ temp;
		tempand[i] = ((l | r) & temp) | (l & r);
	}

	for(i = min; i <= max; i++)
	{
		if(i == 0)
			idxU = N - 1;
		else
			idxU = i - 1;

		if(i == N - 1)
			idxB = 0;
		else
			idxB = i + 1;

		temp = state[i];

		x0 = tempxor[i];
		r0 = tempand[i];

		xU = tempxor[idxU];
		aU = tempand[idxU];

		xB = tempxor[idxB];
		aB = tempand[idxB];

		a0 = x0^xU;
		c = (x0&xU);
		a1 = c^aU^r0;
		a2 = (aU&r0)|((aU|r0)&c);

		b0 = xB^a0;
		c = (xB&a0);
		b1 = c^aB^a1;
		b2 = (aB&a1)|((aB|a1)&c);

		tempState[i] = (b0&b1&(~b2)&(~a2))|((temp)&(a2^b2)&(~b0)&(~b1));
		
	}
	
	int s = min + 1;
	int e =  max - 1;
	
	if(s == 1)
		s = 0;
	
	if(e == N - 2)
		e = N - 1;
	
	for(i = s; i <= e; i++)
	{
		state[i] = tempState[i];
	}	
}

void Reverse(uint64_t  *state, int idxS, int idxE)
{
	for(int i = 0; idxS + i <  idxE - i; i++)
	{
		int l = idxS + i; 
		int r = idxE - i;
		
		uint64_t temp = state[l];
		state[l] = state[r];
		state[r] = temp;
	}
}

void CirculateUp(uint64_t  *state, int anyk)
{
	int k = (anyk + 64 * 10) % 64;
	
	Reverse(state, 0, N - 1);
	Reverse(state, 0, k - 1);
	Reverse(state, k, N - 1);
}


void Move(LifeState* state, int x, int y)
{
	int i, j;
	
	uint64_t temp[N];
	
	for(i = 0; i < N; i++)
	{
		if(y < 0)
			state->state[i] = CirculateRight(state->state[i], -y);
		else
			state->state[i] = CirculateRight(state->state[i], 64 - y);
		
	}

	if(x < 0)
		CirculateUp(state->state, 64 + x);
	else
		CirculateUp(state->state, x);
		
	state->min = 0;
	state->max = N - 1;
}


void Transform(LifeState* state, int dx, int dy, int dxx, int dxy, int dyx, int dyy)
{
	ClearData(Temp);
	
	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < 64; j++)
		{
			int x = i - 32; 
			int y = j - 32;
			
			int x1 = x * dxx + y * dxy;
			int y1 = x * dyx + y * dyy;
			
			int val = GetCell(state, x1, y1);
			
			SetCell(Temp, x, y, val);
			
			if(val == 1)
			{
				//print(Temp);
				//getch();
				//printf("%d, %d, %d, %d, %d\n", x, y, x1, y1, GetPop(Temp));
			}
		}
	}
	
	//print(Temp);
	
	Copy(state, Temp);
	Move(state, dx, dy);
	FitMinMax(state);
}

void Transform(LifeState* state, int dx, int dy)
{
	Move(state, dx, dy);
}

int Parse(LifeState* lifeState, char* rle, int dx, int dy)
{
	char ch;
	int cnt, i, j; 
	int x, y;
	x = 0;
	y = 0;
	cnt = 0;
	
	for(j = 0; j < N; j++)
		lifeState->state[j] = 0;
	
	i = 0;
	
	while((ch = rle[i]) != '\0')
	{

		if(ch >= '0' && ch <= '9')
		{
			cnt *= 10;
			cnt += (ch - '0');
		}
		else if(ch == 'o')
		{
			
			if(cnt == 0)
				cnt = 1;
				
			for(j = 0; j < cnt; j++)
			{
				SetCell(lifeState, x, y, 1);
				x++;
			}
			
			cnt = 0;
		}
		else if(ch == 'b')
		{
			if(cnt == 0)
				cnt = 1;
			
			x += cnt;
			cnt = 0;
			
		}
		else if(ch == '$')
		{
			if(cnt == 0)
				cnt = 1;
			
			y += cnt;
			x = 0;
			cnt = 0;
		}
		else if(ch == '!')
		{
			break;
		}
		else
		{
			return FAIL;
		}
		
		i++;
	}
	
	lifeState->min = 0;
	lifeState->max = N - 1;
	
	Move(lifeState, dx, dy);
	
	return SUCCESS;
}

int Parse(LifeState* lifeState, char* rle)
{
	return Parse(lifeState, rle,  0, 0);
}

int Parse(LifeState* lifeState, char* rle, int dx, int dy, int dxx, int dxy, int dyx, int dyy)
{
	int result = Parse(lifeState, rle);
	
	if(result == SUCCESS)
		Transform(lifeState, dx, dy, dxx, dxy, dyx, dyy);
		
	return result;
}


LifeState* NewState(char* rle, int dx, int dy, int dxx, int dxy, int dyx, int dyy)
{
	LifeState* result = NewState();
	Parse(result, rle);
	Transform(result, dx, dy, dxx, dxy, dyx, dyy);
	
	return result;
}

LifeState* NewState(char* rle, int dx, int dy)
{
	LifeState* result = NewState();
	Parse(result, rle, dx, dy);
	
	return result;
}

LifeState* NewState(char* rle)
{
	return NewState(rle, 0, 0);
}


void PrintRLE(LifeState  * lifeState)
{
	uint64_t * state = lifeState->state;
	
	printf("x = 0, y = 0, rule = B3/S23\n");
	int i, j;
	
	int currowVal,currowCount, numempty, isempty; 
	
	numempty = 0;
	isempty = 1;
	
	for(j = 0; j < N; j++)
	{
		currowCount = 1;
		currowVal = Get(0,j, state);
			
		for(i = 1; i < 64; i++)
		{
			if(currowVal == 1)
			{
				isempty = 0;
				
				if(numempty > 0)
				{
					if(numempty > 1)
						printf ("%d$",numempty);
					else
						printf ("$");
						
					numempty = 0;
				}
			}
			
			if(Get(i,j, state) != currowVal)
			{
				isempty = 0;
				
				if(numempty > 0)
				{
					if(numempty > 1)
						printf ("%d$",numempty);
					else
						printf ("$");
						
					numempty = 0;
				}
				
				if(currowVal == 0)
				{
					if(currowCount == 1)
						printf ("b");
					else
						printf ("%db",currowCount);
				}
				else
				{
					if(currowCount == 1)
						printf ("o");
					else
						printf ("%do",currowCount);
				}
				
				currowCount = 1;
				currowVal = Get(i,j, state);
			
			}
			else
			{
				currowCount++;
			}
			
		}
		
		if(currowVal == 1)
		{
			if(currowCount > 1)
				printf ("%do$",currowCount);
			else
				printf ("o$");
		}
		else 
		{	
			numempty++;
		}
		
		isempty = 1;
		
	}
	
	printf ("!\n\n");
}

void Print()
{
	Print(GlobalState);
}

void PrintRLE()
{
	PrintRLE(GlobalState);
}

void Evolve(LifeState* state, int numIters)
{
	int i; 
	for(i = 0; i < numIters; i++)
	{
		FitMinMax(state);
		ExpandMinMax(&(state-> min), &(state-> max));		
		IterateState(state->state, (state->min), (state->max));
	}
}

void New()
{
	if(GlobalState == NULL)
	{
		GlobalState = NewState();
		Temp = NewState();
		
		for(int i = 0; i < CAPTURE_COUNT; i++)
		{
			Captures[i] =  NewState();
		}
	}
	else
	{
		ClearData(GlobalState);
	}
}

void Capture(LifeState* cap, int idx)
{
	Copy(Captures[idx], cap);
}

void Capture(int idx)
{
	Copy(Captures[idx], GlobalState);
}


void Run(int numIter)
{
	Evolve(GlobalState, numIter);
}

void Join(LifeState* main, LifeState* delta)
{	
	Copy(main, delta , "or");
}

void Join(LifeState* main, LifeState* delta, int dx, int dy)
{	
	for(int i = delta->min; i <= delta->max; i++)
	{	
		int idx = (i + dx + N) % N;
		
		if(dy < 0)
			main->state[idx] |= CirculateRight(delta->state[i], -dy);
		else
			main->state[idx] |= CirculateRight(delta->state[i], 64 -dy);
	}

	main->min  = 0;
	main->max  = N - 1;
}


void PutState(LifeState* state)
{
	Join(GlobalState, state);
}

void PutState(LifeState* state, int dx, int dy)
{
	Join(GlobalState, state, dx, dy);
}

void PutState(LifeState* state, int dx, int dy, int dxx, int dxy, int dyx, int dyy)
{
	ClearData(Temp);
	Copy(Temp, state);
	Transform(Temp, dx, dy, dxx, dxy, dyx, dyy);
	
	PutState(Temp);
}

void PutState(LifeState* state, char* op)
{
	Copy(GlobalState, state, op);
}

int PutState(char* rle)
{
	ClearData(Temp);
	int result = Parse(Temp, rle);
	
	if( result == SUCCESS)
		PutState(Temp);
		
	return result;
}

int PutState(char* rle, int x, int y)
{
	ClearData(Temp);
	int result = Parse(Temp, rle, x, y);
	
	if( result == SUCCESS)
		PutState(Temp);
		
	return result;
}

int PutState(char* rle, int dx, int dy, int dxx, int dxy, int dyx, int dyy)
{
	ClearData(Temp);
	int result = Parse(Temp, rle);
	
	if( result == SUCCESS)
	{
		Transform(Temp, dx, dy, dxx, dxy, dyx, dyy);
		PutState(Temp);
	}
	
	return result;
}

typedef struct 
{
	int x;
	int y; 
	int w; 
	int h;
	int s;
	
	LifeState* States[MAX_ITERATIONS];
	
	int curx;
	int cury;
	int curs; 
	
} LifeIterator;


LifeIterator* NewIterator(LifeState* state, int x, int y, int w, int h, int s, char* op)
{
	LifeIterator* result = (LifeIterator*)(malloc(sizeof(LifeIterator)));

	result -> x = x;
	result -> y = y;
	result -> w = w;
	result -> h = h;
	result -> s = s;
	
	result -> curx = x;
	result -> cury = y;
	result -> curs = 0;
	
	state -> min = 0;
	state -> max = N - 1;
	
	ClearData(Temp);
	Copy(Temp, state);
	
	for(int i = 0; i < s; i++)
	{
		result->States[i] = NewState();		
		Copy(result->States[i], Temp);
		
		if(strcmp("evolve", op) == 0)
			Evolve(Temp, 1);		
	}
	
	return result;
}


LifeIterator* NewIterator(LifeState* states[], int x, int y, int w, int h, int s)
{
	LifeIterator* result = NewIterator(states[0], x, y, w, h, s, "leave");
	
	for(int i = 0; i < s; i++)
	{
		result->States[i] = NewState();		
		Copy(result->States[i], states[i]);
	}
	
	return result;
}

LifeIterator* NewIterator(LifeState* state, int x, int y, int w, int h, int s)
{
	return NewIterator(state, x, y, w, h, s, "evolve");
}

LifeIterator* NewIterator(LifeState* state, int x, int y, int w, int h)
{
	return NewIterator(state, x, y, w, h, 1, "leave");
}

LifeIterator* NewIterator(int x, int y, int w, int h)
{
	ClearData(Temp);
	return  NewIterator(Temp, x, y, w, h, 1);
}

void Print(LifeIterator* iter)
{
	printf("\n(%d, %d, %d)", iter->curx, iter->cury, iter->curs);
}
void Print(LifeIterator* iter, char* name)
{
	printf("\nSetCurrent(%s, %d, %d, %d);", name, iter->curx, iter->cury, iter->curs);
}

void Reset(LifeIterator* iter)
{
	iter -> curx = iter -> x;
	iter -> cury = iter -> y;
	iter -> curs = 0;
}

int Next(LifeIterator* iter)
{
	(iter -> curs)++;
	
	if((iter -> curs) < (iter->s))
		return SUCCESS;
	
	(iter -> curs) = 0;
	(iter -> curx)++;
	
	if((iter -> curx) < (iter->x) + (iter->w))
		return SUCCESS;
		
	iter -> curx = iter->x;
	(iter -> cury)++;
	
	if((iter -> cury) < (iter->y) + (iter->h))
		return SUCCESS;
	
	Reset(iter);
	
	return FAIL;
}

int Next(LifeIterator *iter1[], int numIters)
{
	for(int i = 0; i < numIters; i++)
	{
		if(i == numIters - 1)
			Print(iter1[i]);
		
		if(Next(iter1[i]) == SUCCESS)
			return SUCCESS;
	}
			
	return FAIL;
}

int Next(LifeIterator *iter1, LifeIterator *iter2)
{
	LifeIterator *iters[] = {iter1, iter2};
    return Next(iters, 2);
}


int Next(LifeIterator *iter1, LifeIterator *iter2, LifeIterator *iter3)
{
	LifeIterator *iters[] = {iter1, iter2, iter3};
    return Next(iters, 3);
}


int Next(LifeIterator *iter1, LifeIterator *iter2, LifeIterator *iter3, LifeIterator *iter4)
{
	LifeIterator *iters[] = {iter1, iter2, iter3, iter4};
    return Next(iters, 4);
}

int Next(LifeIterator *iter1, LifeIterator *iter2, LifeIterator *iter3, LifeIterator *iter4, LifeIterator *iter5)
{
	LifeIterator *iters[] = {iter1, iter2, iter3, iter4, iter5};
    return Next(iters, 5);
}


int Next(LifeIterator *iter1, LifeIterator *iter2, LifeIterator *iter3, LifeIterator *iter4, LifeIterator *iter5, LifeIterator *iter6)
{
	LifeIterator *iters[] = {iter1, iter2, iter3, iter4, iter5, iter6};
    return Next(iters, 6);
}

void FreeIterator(LifeIterator* iter)
{
	for(int i = 0; i < iter->s; i++)
		free(iter->States[i]);
		
	free(iter);
}

void PutState(LifeState* state, LifeIterator* iter)
{
	Join(state, iter->States[iter -> s], iter->curx, iter->cury);
}

void PutState(LifeIterator* iter)
{
	Join(GlobalState, iter->States[iter -> curs], iter->curx, iter->cury);
}

void SetCurrent(LifeIterator* iter, int curx, int cury, int curs)
{
	iter -> curx = curx;
	iter -> cury = cury;
	iter -> curs = curs;
}

int Validate(LifeIterator *iter1, LifeIterator *iter2)
{
	if(!(iter1->curx >= iter2->x && iter1->curx < iter2->x + iter2->w))
		return SUCCESS;
	
	if(!(iter1->cury >= iter2->y && iter1->cury < iter2->y + iter2->h))
		return SUCCESS;
		
	if(!(iter2->curx >= iter1->x && iter2->curx < iter1->x + iter1->w))
		return SUCCESS;
	
	if(!(iter2->cury >= iter1->y && iter2->cury < iter1->y + iter1->h))
		return SUCCESS;
		
	if(iter1->curx != iter2->curx)
	{
		//Those two lines are the same
		//if((iter1->curx + iter2->curx) % 2 == 0)
		if((iter1->curx + iter2->curx) & 1 == 0)
		{
			if(iter1->curx > iter2->curx)
				return SUCCESS;
			else
				return FAIL;
		}
		else
		{
			if(iter1->curx > iter2->curx)
				return FAIL;
			else
				return SUCCESS;
		}
	}
	
	if(iter1->cury != iter2->cury)
	{
		if((iter1->cury + iter2->cury) & 1 == 0)
		{
			if(iter1->cury > iter2->cury)
				return SUCCESS;
			else
				return FAIL;
		}
		else
		{
			if(iter1->cury > iter2->cury)
				return FAIL;
			else
				return SUCCESS;
		}
	}
	
	if((iter1->curs + iter2->curs) & 1 == 0)
	{
		if(iter1->curs > iter2->curs)
			return SUCCESS;
		else
			return FAIL;
	}
	else
	{
		if(iter1->curs > iter2->curs)
			return FAIL;
		else
			return SUCCESS;
	}
	
	return SUCCESS;
}

typedef struct 
{
	LifeState* target;
	LifeState* inverse;
	
} LifeTarget;

LifeTarget* NewTarget(LifeState* target, LifeState* inverse)
{
	LifeTarget* result = (LifeTarget*)(malloc(sizeof(LifeTarget)));
	
	result->target = NewState();
	result->inverse = NewState();

	Copy(result->target, target);	
	Copy(result->inverse, inverse);
	
	return result;
}

int ContainsTarget(LifeState* state, LifeTarget* target)
{
	if(Contains(state, target->target) == YES && ContainsInverse(state, target->inverse) == YES)
		return YES;
	else
		return NO;
}

int ContainsTarget(LifeTarget* target)
{
	return ContainsTarget(GlobalState, target);
}

void FreeTarget(LifeTarget* iter)
{
	free(iter -> inverse);
	free(iter -> target);
	
	free(iter);
}
