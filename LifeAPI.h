//LifeAPI provide comfortable functions (API) to manipulate, iterate, evolve, compare and report Life objects. This is mainly done
//in order to provide fast (using C) but still comfortable search utility. 
//Contributor Chris Cain. 
//Written by Michael Simkin 2014

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
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

enum CopyType { COPY, OR, XOR, AND };
enum EvolveType { EVOLVE, LEAVE };

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
static LifeState* Temp, *Temp1, *Temp2;


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

void RefitMinMax(LifeState* state)
{
	int min = state->min;
	int max = state->max;
	uint64_t * states = state->state;
	
	for(int i = min; i <= max; i++)
	{
		if(states[i] != 0)
		{
			state-> min = i;
			break;
		}
	}
	
	for(int i = max; i >= min; i--)
	{
		if(states[i] != 0)
		{
			state-> max  = i;
			break;
		}
	}
	
	ExpandMinMax(&(state-> min), &(state-> max));	
}

void RecalculateMinMax(LifeState* state)
{
	state-> min = N - 1;
	state-> max = 0;
	uint64_t * states = state->state;
	
	for(int i = 0; i < N; i++)
	{
		if(states[i] != 0)
		{
			state-> min = i;
			break;
		}
	}
	
	for(int i = N - 1; i >= 0; i--)
	{
		if(states[i] != 0)
		{
		
			state-> max  = i;
			break;
		}
	}
	
	ExpandMinMax(&(state-> min), &(state-> max));	
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

void Copy(LifeState* main, LifeState* delta, CopyType op)
{
	if(op == COPY)
	{	
		for(int i = 0; i < N; i++)
			main->state[i] = delta->state[i];
	}
	if(op == OR)
	{	
		for(int i = 0; i < N; i++)
			main->state[i] |= delta->state[i];
	}
	if(op == AND)
	{	
		for(int i = 0; i < N; i++)
			main->state[i] &= delta->state[i];
		
	}
	if(op == XOR)
	{	
		for(int i = 0; i < N; i++)
			main->state[i] ^= delta->state[i];
	}
	
	RecalculateMinMax(main);
}

void Copy(LifeState* main, LifeState* delta)
{
	Copy(main, delta, COPY);
}

int GetPop(LifeState* state)
{
	int pop = 0;
	int min = state->min;
	int max = state->max;
	uint64_t * mainState = state->state;
	
	for(int i = min; i <= max; i++)
	{
		pop += __builtin_popcountll(mainState[i]);
	}
	
	return pop;
}

int GetPop()
{
	return GetPop(GlobalState);
}

int GetPop(int captureIdx)
{
	return GetPop(Captures[captureIdx]);
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

int AreEqual(LifeState* pat1)
{
	return AreEqual(GlobalState, pat1);
}

int AreEqual(int idx)
{
	return AreEqual(GlobalState, Captures[idx]);
}

int AreDisjoint(LifeState* main, LifeState* pat)
{
	int min = pat->min;
	int max = pat->max;
	uint64_t * patState = pat->state;
	uint64_t * mainState = main->state;
	
	for(int i = min; i <= max; i++)
		if(((~mainState[i]) & patState[i]) != patState[i])
			return NO;
			
	return YES;
}

int Contains(LifeState* main, LifeState* spark)
{
	int min = spark->min;
	int max = spark->max;
	uint64_t * mainState = main->state;
	uint64_t * sparkState = spark->state;
	
	for(int i = min; i <= max; i++)
		if((mainState[i] & sparkState[i]) != (sparkState[i]))
			return NO;
			
	return YES;
}

int AllOn(LifeState* spark)
{
	return Contains(GlobalState, spark);
}

int AllOff(LifeState* spark)
{
	return AreDisjoint(GlobalState, spark);
}

LifeState* NewState()
{
	LifeState* result = (LifeState*)(malloc(sizeof(LifeState)));
	ClearData(result);
	
	return result;
}

void IterateState(LifeState  *lifstate)
{
	uint64_t* state = lifstate->state;
	int min = lifstate->min;
	int max = lifstate->max;
	
	uint64_t tempxor[N];
	uint64_t tempand[N];

	uint64_t tempState[N];

	uint64_t l, r, temp;
	uint64_t x0, r0, xU, aU, xB, aB;
	uint64_t a0,a1,a2,c, b0, b1, b2;

	int i, idxU, idxB;
	
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
	
	RefitMinMax(lifstate);
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
	for(int i = 0; i < N; i++)
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


void FlipX(LifeState* state)
{	
	Reverse(state->state, 0, N - 1);
	Move(state, 1, 0);
}


void FlipX()
{
	FlipX(GlobalState);
}

void FlipX(int idx)
{
	FlipX(Captures[idx]);
	
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
	RecalculateMinMax(state);
}

void Transform(LifeState* state, int dx, int dy)
{
	Move(state, dx, dy);
}

int Parse(LifeState* lifeState, const char* rle, int dx, int dy)
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

int Parse(LifeState* lifeState, const char* rle)
{
	return Parse(lifeState, rle,  0, 0);
}

int Parse(LifeState* lifeState, const char* rle, int dx, int dy, int dxx, int dxy, int dyx, int dyy)
{
	int result = Parse(lifeState, rle);
	
	if(result == SUCCESS)
		Transform(lifeState, dx, dy, dxx, dxy, dyx, dyy);
		
	return result;
}


LifeState* NewState(const char* rle, int dx, int dy, int dxx, int dxy, int dyx, int dyy)
{
	LifeState* result = NewState();
	Parse(result, rle);
	Transform(result, dx, dy, dxx, dxy, dyx, dyy);
	
	return result;
}

LifeState* NewState(const char* rle, int dx, int dy)
{
	LifeState* result = NewState();
	Parse(result, rle, dx, dy);
	
	return result;
}

LifeState* NewState(const char* rle)
{
	return NewState(rle, 0, 0);
}

char* Join(char* first, char* second)
{
	char * s = (char *)malloc(snprintf(NULL, 0, "%s%s", first, second) + 1);
	sprintf(s, "%s%s", first, second);
	return s;
}

char* GetRLE(LifeState  * lifeState)
{
	uint64_t * state = lifeState->state;
	char* result = ""; 
	
	
	int i, j;
	
	int currowVal,currowCount, numempty, isempty; 
	
	numempty = 0;
	isempty = 1;
	char str[15];
	

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
					{
						sprintf(str, "%d$", numempty);
						result = Join(result, str);
					}
					else
					{
						result = Join(result, "$");
					}
						
					numempty = 0;
				}
			}
			
			if(Get(i,j, state) != currowVal)
			{
				isempty = 0;
				
				if(numempty > 0)
				{
					if(numempty > 1)
					{
						sprintf(str, "%d$", numempty);
						result = Join(result, str);
					}
					else
						result = Join(result, "$");
						
					numempty = 0;
				}
				
				if(currowVal == 0)
				{
					if(currowCount == 1)
						result = Join(result, "b");
					else
					{
						sprintf(str, "%db", currowCount);
						result = Join(result, str);
					}
				}
				else
				{
					if(currowCount == 1)
						result = Join(result, "o");
					else
					{
						sprintf(str, "%do", currowCount);
						result = Join(result,str);
					}
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
			{	
				sprintf(str, "%do", currowCount);
				result = Join(result,str);
			}
			else
				result = Join(result, "o$");
		}
		else 
		{	
			numempty++;
		}
		
		isempty = 1;
		
	}
	
	return result;
}

void PrintRLE(LifeState  * lifeState)
{
	char * prefix ="x = 0, y = 0, rule = B3/S23\n";
	printf(Join(prefix, GetRLE(lifeState)));
	printf ("!\n\n");
}

void Print()
{
	Print(GlobalState);
}

void Print(int idx)
{
	Print(Captures[idx]);
}

void PrintRLE()
{
	PrintRLE(GlobalState);
}

void PrintRLE(int idx)
{
	PrintRLE(Captures[idx]);
}

void Evolve(LifeState* state, int numIters)
{
	int i; 
	for(i = 0; i < numIters; i++)
		IterateState(state);
}

void New()
{
	if(GlobalState == NULL)
	{
		GlobalState = NewState();
		Temp = NewState();
		Temp1 = NewState();
		Temp2 = NewState();
		
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
	Copy(main, delta , OR);
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

void PutState(int idx)
{
	PutState(Captures[idx]);
}

void PutState(LifeState* state, int dx, int dy, int dxx, int dxy, int dyx, int dyy)
{
	ClearData(Temp);
	Copy(Temp, state);
	Transform(Temp, dx, dy, dxx, dxy, dyx, dyy);
	
	PutState(Temp);
}

void PutState(LifeState* state, CopyType op)
{
	Copy(GlobalState, state, op);
}

int PutState(const char* rle)
{
	ClearData(Temp);
	int result = Parse(Temp, rle);
	
	if( result == SUCCESS)
		PutState(Temp);
		
	return result;
}

int PutState(const char* rle, int x, int y)
{
	ClearData(Temp);
	int result = Parse(Temp, rle, x, y);
	
	if( result == SUCCESS)
		PutState(Temp);
		
	return result;
}

int PutState(const char* rle, int dx, int dy, int dxx, int dxy, int dyx, int dyy)
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


LifeIterator* NewIterator(LifeState* state, int x, int y, int w, int h, int s, EvolveType op)
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
		
		if(op == EVOLVE)
			Evolve(Temp, 1);		
	}
	
	return result;
}


LifeIterator* NewIterator(LifeState* states[], int x, int y, int w, int h, int s)
{
	LifeIterator* result = NewIterator(states[0], x, y, w, h, s, LEAVE);
	
	for(int i = 0; i < s; i++)
	{
		result->States[i] = NewState();		
		Copy(result->States[i], states[i]);
	}
	
	return result;
}

LifeIterator* NewIterator(LifeState* state, int x, int y, int w, int h, int s)
{
	return NewIterator(state, x, y, w, h, s, EVOLVE);
}

LifeIterator* NewIterator(LifeState* state, int x, int y, int w, int h)
{
	return NewIterator(state, x, y, w, h, 1, LEAVE);
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
void Print(LifeIterator* iter, const char* name)
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

int Next(LifeIterator *iter1[], int numIters, int toPrint)
{
	for(int i = 0; i < numIters; i++)
	{
		if(i == numIters - 1)
		{
			if(toPrint == YES)
				Print(iter1[i]);
		}
		
		if(Next(iter1[i]) == SUCCESS)
			return SUCCESS;
	}
	
	return FAIL;
}

int Next(LifeIterator *iter1, LifeIterator *iter2, int toPrint)
{
	LifeIterator *iters[] = {iter1, iter2};
    return Next(iters, 2, toPrint);
}

int Next(LifeIterator *iter1, LifeIterator *iter2)
{
	return Next(iter1, iter2, YES);
}

int Next(LifeIterator *iter1, LifeIterator *iter2, LifeIterator *iter3, int toPrint)
{
	LifeIterator *iters[] = {iter1, iter2, iter3};
    return Next(iters, 3, toPrint);
}

int Next(LifeIterator *iter1, LifeIterator *iter2, LifeIterator *iter3)
{
	return Next(iter1, iter2, iter3, YES);
}

int Next(LifeIterator *iter1, LifeIterator *iter2, LifeIterator *iter3, LifeIterator *iter4, int toPrint)
{
	LifeIterator *iters[] = {iter1, iter2, iter3, iter4};
    return Next(iters, 4, toPrint);
}
int Next(LifeIterator *iter1, LifeIterator *iter2, LifeIterator *iter3, LifeIterator *iter4)
{
	return Next(iter1, iter2, iter3, iter4, YES);
}

int Next(LifeIterator *iter1, LifeIterator *iter2, LifeIterator *iter3, LifeIterator *iter4, LifeIterator *iter5, int toPrint)
{
	LifeIterator *iters[] = {iter1, iter2, iter3, iter4, iter5};
    return Next(iters, 5, toPrint);
}

int Next(LifeIterator *iter1, LifeIterator *iter2, LifeIterator *iter3, LifeIterator *iter4, LifeIterator *iter5)
{
	return Next(iter1, iter2, iter3, iter4, iter5, YES);
}

int Next(LifeIterator *iter1, LifeIterator *iter2, LifeIterator *iter3, LifeIterator *iter4, LifeIterator *iter5, LifeIterator *iter6, int toPrint)
{
	LifeIterator *iters[] = {iter1, iter2, iter3, iter4, iter5, iter6};
    return Next(iters, 6, toPrint);
}

int Next(LifeIterator *iter1, LifeIterator *iter2, LifeIterator *iter3, LifeIterator *iter4, LifeIterator *iter5, LifeIterator *iter6)
{
	return Next(iter1, iter2, iter3, iter4, iter5, iter6, YES);
}

int Next(LifeIterator *iter1[], int numIters)
{
	return Next(iter1, numIters, YES);
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
		if((iter1->curx + iter2->curx) % 2 == 0)
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
		if((iter1->cury + iter2->cury) % 2 == 0)
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
	
	if((iter1->curs + iter2->curs) % 2 == 0)
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

void GetBoundary(LifeState* state, LifeState* boundary)
{
    for(int i = 0; i < N; i++) {
        uint64_t col = state->state[i];
        Temp->state[i] = col | CirculateLeft(col) | CirculateRight(col);
    }

    boundary->state[0] = Temp->state[N-1] | Temp->state[0] | Temp->state[1];
    
    for(int i = 1; i < N-1; i++)
        boundary->state[i] = Temp->state[i-1] | Temp->state[i] | Temp->state[i+1];

    boundary->state[N-1] = Temp->state[N-2] | Temp->state[N-1] | Temp->state[0];
    
    for(int i = 0; i < N; i++)
        boundary->state[i] &= ~(state->state[i]);
}

void GetBoundary(LifeState* state, int captureIdx)
{
	GetBoundary(state, Captures[captureIdx]);
}

void GetBoundary(LifeState* boundary)
{
	GetBoundary(GlobalState, boundary);
}

void GetBoundary(int captureIdx)
{
	GetBoundary(GlobalState, Captures[captureIdx]);
}

typedef struct 
{
	LifeState* wanted;
	LifeState* unwanted;
	
} LifeTarget;

LifeTarget* NewTarget(LifeState* wanted, LifeState* unwanted)
{
	LifeTarget* result = (LifeTarget*)(malloc(sizeof(LifeTarget)));
	
	result->wanted = NewState();
	result->unwanted = NewState();

	Copy(result->wanted, wanted);	
	Copy(result->unwanted, unwanted);
	
	RecalculateMinMax(result->wanted);
	RecalculateMinMax(result->unwanted);
	
	return result;
}



LifeTarget* NewTarget(LifeState* wanted)
{
	GetBoundary(wanted, Temp1);
	return NewTarget(wanted, Temp1);
}


LifeTarget* NewTarget(const char* rle, int x, int y, int dxx, int dxy, int dyx, int dyy)
{
	int result = Parse(Temp2, rle, x, y, dxx, dxy, dyx, dyy);
	
	if(result == SUCCESS)
	{
		return NewTarget(Temp2);
	}
	
	return NULL;
}

LifeTarget* NewTarget(const char* rle, int x, int y)
{
	int result = Parse(Temp2, rle, x, y);
	
	if(result == SUCCESS)
	{
		return NewTarget(Temp2);
	}
	
	return NULL;
}

LifeTarget* NewTarget(const char* rle)
{
	return NewTarget(rle, 0, 0);
}

int Contains(LifeState* state, LifeTarget* target)
{
	if(Contains(state, target->wanted) == YES && AreDisjoint(state, target->unwanted) == YES)
		return YES;
	else
		return NO;
}

int Contains(LifeTarget* target)
{
	return Contains(GlobalState, target);
}

void FreeTarget(LifeTarget* iter)
{
	free(iter -> wanted);
	free(iter -> unwanted);
	
	free(iter);
}


typedef struct 
{
	LifeState** results;
	int size;
	int alocated;
	
} ResultManager;

ResultManager* NewResults()
{
	ResultManager* result = (ResultManager*)(malloc(sizeof(ResultManager*)));
	
	result->results = (LifeState**)(malloc(10 * sizeof(LifeState*)));
	
	for(int i = 0; i < 10; i++)
	{
		(result->results)[i] = NewState();
	}
	
	result->alocated = 10;
	result->size = 0;
	
	return result;
}

void Add(ResultManager* manager, LifeState* state)
{
	if(manager->size == manager->alocated)
	{
		manager->results = (LifeState**)(realloc(manager->results, manager->alocated * 2 * sizeof(LifeState*)));
		manager->alocated *= 2;
		
		for(int i = manager->size; i < 2 * (manager->size); i++)
		{
			(manager->results)[i] = NewState();
		}
	}
	
	Copy((manager->results)[manager->size], state);
	manager->size++;
}

void Add(ResultManager* manager)
{
	Add(manager, GlobalState);
}

void Save(ResultManager* manager, const char* fileName)
{
	
}
