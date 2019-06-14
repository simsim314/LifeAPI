//LifeAPI provide comfortable functions (API) to manipulate, iterate, evolve, compare and report Life objects. This is mainly done
//in order to provide fast (using C) but still comfortable search utility. 
//Contributors Chris Cain, Dongook Lee. 
//Written by Michael Simkin 2014
#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define N 64
#define MAX_EMITTED 64

#define PrimeN 71
#define CAPTURE_COUNT 10 
#define MAX_ITERATIONS 200

#define SUCCESS 1
#define FAIL 0

#define YES 1
#define NO 0

#ifdef _MSC_VER
	#include <intrin.h>
	#define __builtin_popcountll __popcnt64
#endif

enum CopyType { COPY, OR, XOR, AND };
enum EvolveType { EVOLVE, LEAVE };

typedef struct 
{
	char* value;
	int size;
	int allocated;
	
} LifeString;

void FreeString(LifeString* string)
{
	free(string->value);
	free(string);
}

LifeString* NewString()
{
	LifeString* result = (LifeString*)(malloc(sizeof(LifeString)));
	
	result->value = (char*)(malloc(2 * sizeof(char)));
	result->value[0] = '\0';
	result->size = 1;
	result->allocated = 1;
	
	return result;
}

void Clear(LifeString* s)
{
	s->value[0] = '\0';
	s->size = 1;

}

void Realloc(LifeString* string)
{
	int empty = NO;
	
	if(string->value[0] == '\0')
		empty = YES;
	
	if(empty == NO)
	{
		string->value = (char*)(realloc(string->value, string->allocated * 2 * sizeof(char)));
	}
	else
	{
		string->value = (char*)(malloc(string->allocated * 2 * sizeof(char)));
		string->value[0] = '\0';
	}
	
	string->allocated *= 2;
}

void Realloc(LifeString* string, int size)
{
	while(string->allocated <= string->size + size + 1)
		Realloc(string);
}

void Append(LifeString* string, const char* val)
{
	Realloc(string, strlen(val));
	strcat(string->value, val);
	string->size = strlen(string->value);
}

void Append(LifeString* string, int val)
{
	char str[10];
	sprintf(str, "%d", val);
	Append(string, str);
}

LifeString* NewString(const char* val)
{
	LifeString* result = NewString();
	Append(result, val);
	return result;
}

typedef struct 
{
	int x; 
	int y; 
	int gen; 
	int dx; 
	int dy; 
	
} EmitedGlider;

typedef struct 
{
	int min;
	int max; 
	int gen; 
	uint64_t  state[N];
	
	EmitedGlider emittedGliders[MAX_EMITTED];  
	int num_emitted; 
	
} LifeState;

static LifeState* GlobalState;
#pragma omp threadprivate(GlobalState)

static LifeState* Captures[CAPTURE_COUNT];
#pragma omp threadprivate(Captures)

static LifeState* Temp, *Temp1, *Temp2;
#pragma omp threadprivate(Temp, Temp1, Temp2)

static EmitedGlider _gliders[4];
#pragma omp threadprivate(_gliders)

inline uint64_t CirculateLeft(uint64_t x){
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

int GetCell(int x, int y)
{
	return GetCell(GlobalState, x, y);
}

void SetCell(int x, int y, int val)
{
	SetCell(GlobalState, x, y, val);
}

uint64_t  GetHash(LifeState* state)
{
	uint64_t  result = 0; 
	
	for(int i = state->min; i <= state->max; i++)
	{
		result +=  CirculateRight(state->state[i], i);
		result += CirculateLeft(state->state[i], (i  + 8) % 47);
	}
	
	return result;
}

uint64_t GetHash()
{
	return GetHash(GlobalState);
}

void ExpandMinMax(int* min, int* max)
{
	*min -= 2;
	*max += 2;
	
	if(*min < 0)
		(*min) = 0;
	
	if(*max > N - 1)
		(*max) = N - 1;
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
			
		main->gen = delta->gen;
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

int IsSame(LifeState* s1, LifeState* s2)
{
	for(int i = 0; i < N; i++)
		if(s1->state[i] != s2->state[i])
			return NO;
		
	return YES; 
}

int IsSame(LifeState* s1)
{
	return IsSame(GlobalState, s1);
}

int IsSame(int capture_idx)
{
	return IsSame(GlobalState, Captures[capture_idx]);
}

int IsEmpty(LifeState* s)
{
	int min = s->min;
	int max = s->max;
	uint64_t * mainState = s->state;
	
	for(int i = min; i <= max; i++)
	{
		if(mainState[i] != 0LL)
			return NO;
	}
	
	return YES; 
}

int IsEmpty()
{
	return IsEmpty(GlobalState);
}

int IsEmpty(int idx)
{
	return IsEmpty(Captures[idx]);
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
	state->gen = 0;
	state->num_emitted = 0; 
}

LifeState* NewState()
{
	LifeState* result = (LifeState*)(malloc(sizeof(LifeState)));
	ClearData(result);
	
	return result;
}

void FreeState(LifeState* state)
{
	free(state);
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

int AreDisjoint(LifeState* main, LifeState* pat, int targetDx, int targetDy)
{
	int min = pat->min;
	int max = pat->max;
	uint64_t * patState = pat->state;
	uint64_t * mainState = main->state;
	int dy = (targetDy + 64) % 64;
	
	for(int i = min; i <= max; i++)
	{
		int curX = (N + i + targetDx) % N; 
		
		if(((~CirculateRight(mainState[curX], dy)) & patState[i]) != patState[i])
			return NO;
	}

	return YES;
}

int Contains(LifeState* main, LifeState* spark, int targetDx, int targetDy)
{
	int min = spark->min;
	int max = spark->max;
	
	uint64_t * mainState = main->state;
	uint64_t * sparkState = spark->state;
	int dy = (targetDy + 64) % 64;
	
	for(int i = min; i <= max; i++)
	{
		int curX = (N + i + targetDx) % N; 
		
		if((CirculateRight(mainState[curX], dy) & sparkState[i]) != (sparkState[i]))
			return NO;
	}		
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
	ClearData(Temp2);
	ClearData(Temp1);
	Copy(Temp1, state);

	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < 64; j++)
		{
			int x = i - 32; 
			int y = j - 32;
			
			int x1 = x * dxx + y * dxy;
			int y1 = x * dyx + y * dyy;
			
			int val = GetCell(Temp1, x1, y1);
			
			SetCell(Temp2, x, y, val);
		}
	}
	Move(Temp2, dx, dy);
	
	Copy(state, Temp2);
	RecalculateMinMax(state);
}

void Transform(LifeState* state, int dx, int dy)
{
	Move(state, dx, dy);
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


int Parse(LifeState* state, const char* rle, int starti)
{
	char ch;
	int cnt, i, j; 
	int x, y;
	x = 0;
	y = 0;
	cnt = 0;
	
	ClearData(state);
	
	i = starti;
	
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
				SetCell(state, x, y, 1);
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
			
			if(cnt == 129)
				return i + 1;
				
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
			return -2;
		}
		
		i++;
	}
	
	state->min = 0;
	state->max = N - 1;
	
	return -1;
}

int Parse(LifeState* state, const char* rle, int dx, int dy)
{
	if(Parse(state, rle, 0) == -1)
	{
		Move(state, dx, dy);
		return SUCCESS;
	}
	else 
	{
		return FAIL;
	}
}


int Parse(LifeState* state, const char* rle)
{
	return Parse(state, rle, 0, 0);
}

int Parse(LifeState* state, const char* rle, int dx, int dy, int dxx, int dxy, int dyx, int dyy)
{
	int result = Parse(state, rle);
	
	if(result == SUCCESS)
		Transform(state, dx, dy, dxx, dxy, dyx, dyy);
		
	return result;
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

int Contains(LifeState* state, LifeTarget* target, int dx, int dy)
{
	if(Contains(state, target->wanted, dx, dy) == YES && AreDisjoint(state, target->unwanted, dx, dy) == YES)
		return YES;
	else
		return NO;
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
	FreeState(iter -> wanted);
	FreeState(iter -> unwanted);
	
	free(iter);
}


typedef struct 
{
  int* xList;
  int* yList;
  int len;
  int allocated;
} Locator;

Locator* NewLocator()
{
	Locator* result = (Locator*)(malloc(sizeof(Locator)));
	
	result->xList = (int*)(malloc(sizeof(int)));
	result->yList = (int*)(malloc(sizeof(int)));
	result->len = 0;
	result->allocated = 1;
	
	return result;
}

Locator* Realloc(Locator* locator)
{
	if(locator->allocated <= locator->len)
	{				
		locator->allocated *= 2;
		locator->xList = (int*)(realloc(locator->xList, locator->allocated * sizeof(int)));
		locator->yList = (int*)(realloc(locator->yList, locator->allocated * sizeof(int)));	
	}

	return locator;
}

void Add(Locator* locator, int x, int y)
{
	Realloc(locator);
	
	locator->xList[locator->len] = x;
	locator->yList[locator->len] = y;
	locator->len++;
}

Locator *State2Locator(LifeState* state)
{
	Locator* result = NewLocator();
	
	for(int j = 0; j < PrimeN; j++)
	{
        for(int i = 0; i < PrimeN; i++)
		{
			int x = (j * 35 + 17) % PrimeN;
			int y = (i * 11 + 29) % PrimeN;
			
			if(x >= N || y >= N)
				continue;
			
            int val = Get(x, y, state->state);
			
			if(val == 1)
				Add(result, x, y);
		}
	}
	
	return result;
}

void ClearAtX(LifeState* state, Locator* locator, int x, uint64_t val)
{
	if(val == 0ULL)
		return;
	
	int len = locator->len;
	int* xList =  locator->xList;
	int* yList =  locator->yList;

	for(int i = 0; i < len; i++)
	{
		int idx = (x + xList[i] + N) % N;
		int circulate = (yList[i] + 64) % 64;
		
		state->state[idx] &= ~CirculateLeft(val, circulate);
	}
}

uint64_t LocateAtX(LifeState* state, Locator* locator, int x, int negate)
{
	uint64_t result = ~0ULL;
	int len = locator->len;
	int* xList =  locator->xList;
	int* yList =  locator->yList;

	for(int i = 0; i < len; i++)
	{
		int idx = (x + xList[i] + N) % N;
		int circulate = (yList[i] + 64) % 64;
		
		if(negate == NO)
			result &= CirculateRight(state->state[idx], circulate);
		else
			result &= ~CirculateRight(state->state[idx],circulate);
		
		if(result == 0ULL)
			break;
	}

	
	return result;
}

uint64_t LocateAtX(LifeState* state, Locator* onLocator, Locator* offLocator, int x)
{
	uint64_t onLocate = LocateAtX(state, onLocator, x, NO);
	
	if(onLocate == 0)
		return 0;
		
	return  onLocate & LocateAtX(state, offLocator, x, YES);
}

void LocateInRange(LifeState* state, Locator* locator, LifeState* result, int minx, int maxx, int negate)
{
	for(int i = minx; i<= maxx; i++)
	{
		result->state[i] = LocateAtX(state, locator, i, negate);
	}
}

void LocateInRange(LifeState* state, Locator* onLocator, Locator* offLocator, LifeState* result, int minx, int maxx)
{
	for(int i = minx; i<= maxx; i++)
	{
		result->state[i] = LocateAtX(state, onLocator, offLocator, i);
	}
}

void Locate(LifeState* state, Locator* locator, LifeState* result)
{
	LocateInRange(state, locator, result, state->min, state->max, NO);
}

int ContainsLocator(LifeState* state, Locator* onLocator, Locator* offLocator, int minx, int maxx)
{
	for(int i = minx; i<= maxx; i++)
	{
		if(LocateAtX(state, onLocator, offLocator, i) != 0)
			return YES;
	}
	
	return NO;
}


typedef struct 
{
  Locator* onLocator;
  Locator* offLocator;
} TargetLocator;


TargetLocator* NewTargetLocator()
{
	TargetLocator* result = (TargetLocator*)(malloc(sizeof(TargetLocator)));
	
	result->onLocator = NewLocator();
	result->offLocator = NewLocator();
	
	return result;
}


TargetLocator* Target2Locator(LifeTarget* target)
{
	TargetLocator* result = (TargetLocator*)(malloc(sizeof(TargetLocator)));
	result->onLocator = State2Locator(target->wanted);
	result->offLocator = State2Locator(target->unwanted);
	
	return result;
}

TargetLocator* NewTargetLocator(LifeState* state)
{
	TargetLocator* result = Target2Locator(NewTarget(state));
	return result;
}

TargetLocator* NewTargetLocator(const char* rle)
{
	TargetLocator* result = Target2Locator(NewTarget(rle, -32, -32));
	return result;
}


TargetLocator* NewTargetLocator(const char* rle, int x, int y)
{
	TargetLocator* result = Target2Locator(NewTarget(rle, -32 + x, -32 + y));
	return result;
}

uint64_t LocateAtX(LifeState* state, TargetLocator* targetLocator, int x)
{
	return LocateAtX(state, targetLocator->onLocator, targetLocator->offLocator,  x);
}

void LocateInRange(LifeState* state, TargetLocator* targetLocator, LifeState* result, int minx, int maxx)
{
	return LocateInRange(state, targetLocator->onLocator, targetLocator->offLocator, result, minx, maxx);
}

void LocateTarget(LifeState* state, TargetLocator* targetLocator, LifeState* result)
{
	LocateInRange(state, targetLocator, result, state->min, state->max);
}

void LocateTarget(TargetLocator* targetLocator, LifeState* result)
{
	LocateTarget(GlobalState, targetLocator, result);
}

int ContainsLocator(LifeState* state, TargetLocator* targetLocator)
{
	return ContainsLocator(state, targetLocator->onLocator, targetLocator->offLocator, state->min, state->max);
}

int ContainsLocator(TargetLocator* targetLocator)
{
	return ContainsLocator(GlobalState, targetLocator->onLocator, targetLocator->offLocator, GlobalState->min, GlobalState->max);
}

int ContainsLocator(TargetLocator* locs[], int n)
{
	for(int i = 0; i < n; i++)
		if(ContainsLocator(locs[i]) == YES)
			return YES;
	
	return NO; 
}

static TargetLocator* _glidersTarget[4];
#pragma omp threadprivate(_glidersTarget)

int RemoveAtX(LifeState *state, int x, int startGiderIdx)
{
	int removed = NO;
	
	for(int i = startGiderIdx; i < startGiderIdx + 2; i++)
	{
		uint64_t gld = LocateAtX(state, _glidersTarget[i], x);
		
		if(gld != 0)
		{
			removed = YES;
			ClearAtX(state, _glidersTarget[i]->onLocator, x, gld);
			
			for(int j = 0; j < 64; j++)
			{
				if(gld % 2 == 1)
				{
					state->emittedGliders[state->num_emitted].x = x; 
					state->emittedGliders[state->num_emitted].y = j;
					state->emittedGliders[state->num_emitted].gen = state->gen;
					state->emittedGliders[state->num_emitted].dx = _gliders[i].dx;
					state->emittedGliders[state->num_emitted].dy = _gliders[i].dy;
					state->num_emitted++; 
				}
				
				gld = gld >> 1;
				
				if(gld == 0)
					break;
			}
		}
	}
	
	return removed;
}

void RemoveGliders(LifeState *state)
{
	int removed = NO;
	
	if(state->min <= 1)
		if(RemoveAtX(state, 1, 0) == YES)
			removed = YES;
	
	if(state->max >= N - 2)
		if(RemoveAtX(state, N - 2, 2) == YES)
			removed = YES;
			
	if(removed == YES)
		RecalculateMinMax(state);
}

void inline Add(uint64_t& b1, uint64_t &b0, const uint64_t& val)
{
	b1 |= b0 & val;
	b0 ^= val;
}

void inline Add_Init(uint64_t& b1, uint64_t& b0, const uint64_t& val)
{
	b1 = b0 & val;
	b0 ^= val;
}

void inline Add(uint64_t& b2, uint64_t& b1, uint64_t &b0, const uint64_t& val)
{
	uint64_t t_b2 = b0 & val;

	b2 |= t_b2 & b1;
	b1 ^= t_b2;
	b0 ^= val;
}

void inline Add_Init(uint64_t& b2, uint64_t& b1, uint64_t &b0, uint64_t& val)
{
	uint64_t t_b2 = b0 & val;

	b2 = t_b2&b1;
	b1 ^= t_b2;
	b0 ^= val;
}

uint64_t inline Evolve(const uint64_t& temp, const uint64_t& bU0, const uint64_t& bU1, const uint64_t& bB0, const uint64_t& bB1)
{
	uint64_t sum0, sum1, sum2;
	sum0 = temp << 1;
	Add_Init(sum1, sum0, temp >> 1);

	Add(sum1, sum0, bU0);
	Add_Init(sum2, sum1, bU1);
	Add(sum2, sum1, sum0, bB0);
	Add(sum2, sum1, bB1);

	return ~sum2 & sum1 & (temp | sum0);
}

void IterateState(LifeState *lifstate)
{
	uint64_t* state = lifstate->state;
	int min = lifstate->min;
	int max = lifstate->max;

	uint64_t bit0[N];
	uint64_t bit1[N];

	uint64_t tempState[N];

	for (int i = min; i <= max; i++)
	{
		uint64_t l, r, temp;

		temp = state[i];
		l = temp << 1;
		r = temp >> 1;
		bit0[i] = l ^ r ^ temp;
		bit1[i] = ((l | r) & temp) | (l & r);
	}

	int start = min;
	int last = max;

	if (min == 0)
	{
		tempState[0] = Evolve(state[0], 0, 0, bit0[2], bit1[2]);
		start = 1;
	}

	if (max == N - 1)
	{
		tempState[N - 1] = Evolve(state[0], bit0[N - 2], bit1[N - 2], 0, 0);
		last = N - 2;
	}

	for (int i = start; i <= last; i++)
		tempState[i] = Evolve(state[i], bit0[i - 1], bit1[i - 1], bit0[i + 1], bit1[i + 1]);
	
	int s = min + 1;
	int e =  max - 1;
	
	if(s == 1)
		s = 0;
	
	if(e == N - 2)
		e = N - 1;
	
	for (int i = s; i <= e; i++)
	{
		state[i] = tempState[i];
	}	
	
	RefitMinMax(lifstate);
	lifstate->gen++;
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

const char* GetRLE(LifeState *state)
{
    LifeString* result = NewString();
	
	int eol_count = 0; 

	for(int j = 0; j < N; j++)
	{
            int last_val = -1;
            int run_count = 0;
			
           for(int i = 0; i < N; i++)
		{
                    int val = Get(i, j, state->state);

                    // Flush linefeeds if we find a live cell
                    if(val == 1 && eol_count > 0)
					{
                            if(eol_count > 1)
                                Append(result, eol_count);
                            
							Append(result, "$");
                            
                            eol_count = 0;
					}

                    // Flush current run if val changes
                    if (val == 1 - last_val)
                        {
                            if(run_count > 1)
                                Append(result, run_count);
                            
							Append(result, last_val ? "o" : "b");
                            
                            run_count = 0;
			}

                    run_count++;
                    last_val = val;
                }

            // Flush run of live cells at end of line
            if (last_val == 1)
                {
                    if(run_count > 1)
                        Append(result, run_count);
                            
                    Append(result, "o");
                            
                    run_count = 0;
                }

            eol_count++;
	}
        
	return result->value;
}

void PrintRLE(LifeState *state)
{
    printf("\nx = 0, y = 0, rule = B3/S23\n%s!\n\n", GetRLE(state));
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
	for(int i = 0; i < numIters; i++)
	{
		IterateState(state);
		RemoveGliders(state);
	}
}

void Evolve(LifeState* after, LifeState* before, int numIters)
{
        Copy(after, before);
        Evolve(after, numIters);
}

namespace PRNG {

    //Public domain PRNG by Sebastian Vigna 2014, see http://xorshift.di.unimi.it

    uint64_t s[16] = { 0x12345678 }; 
    int p = 0;
    
    uint64_t rand64() { 
        uint64_t s0 = s[ p ];
	uint64_t s1 = s[ p = ( p + 1 ) & 15 ];
	s1 ^= s1 << 31; // a
	s1 ^= s1 >> 11; // b
	s0 ^= s0 >> 30; // c
	return ( s[ p ] = s0 ^ s1 ) * 1181783497276652981ULL; 
    }

}

void RandomState(LifeState* state) {

    for (int i = 0; i < N; i++)
        state->state[i] = PRNG::rand64();

    RecalculateMinMax(state);
}

void RandomState() {
    
    RandomState(GlobalState);
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
		
		_glidersTarget[0] = NewTargetLocator("2o$obo$o!");
		_glidersTarget[1] = NewTargetLocator("o$obo$2o!");
		_glidersTarget[2] = NewTargetLocator("b2o$obo$2bo!", -2, 0);
		_glidersTarget[3] = NewTargetLocator("2bo$obo$b2o!", -2, 0);

		
		_gliders[0].dx = -1;
		_gliders[0].dy = -1;
		
		_gliders[1].dx = -1;
		_gliders[1].dy = 1;
		
		_gliders[2].dx = 1;
		_gliders[2].dy = 1;
		
		_gliders[3].dx = 1;
		_gliders[3].dy = -1;
		
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

LifeIterator* NewIterator(const char* rle, int x, int y, int w, int h, int s)
{
	LifeState* state = NewState(rle);
	return NewIterator(state, x, y, w, h, s, EVOLVE);
}

LifeIterator* NewIterator(const char* rle, int x, int y, int w, int h)
{
	LifeState* state = NewState(rle);
	return NewIterator(state, x, y, w, h);
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

void Print(LifeIterator* iter[], int numIters)
{
	for(int i = 0; i < numIters; i++)
		Print(iter[i]);
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
		if(toPrint == YES)
		{
			if(i == numIters - 1)
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
		FreeState(iter->States[i]);
		
	free(iter);
}

void Join(LifeState* state, LifeIterator* iter)
{
	Join(state, iter->States[iter -> curs], iter->curx, iter->cury);
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
	if(iter1->curx > iter2->curx)
		return SUCCESS;
	
	if(iter1->curx < iter2->curx)
		return FAIL;
	
	if(iter1->cury > iter2->cury)
		return SUCCESS;
	
	if(iter1->cury < iter2->cury)
		return FAIL;
		
	if(iter1->curs > iter2->curs)
		return SUCCESS;
		
	return FAIL;
}

int Validate(LifeIterator *iter1, LifeIterator *iter2, LifeIterator *iter3)
{
	if(Validate(iter1, iter2) == FAIL)
		return FAIL;
	
	if(Validate(iter2, iter3) == FAIL)
		return FAIL;
	
	return SUCCESS;
}

int Validate(LifeIterator *iters[], int iterCount)
{
	for(int i = 0; i < iterCount - 1; i++)
		if(Validate(iters[i], iters[i + 1]) == FAIL)
			return FAIL;
			
	return SUCCESS;
}

typedef struct 
{
	LifeState** results;
	int size;
	int allocated;
	
} LifeResults;

LifeResults* NewResults()
{
	LifeResults* result = (LifeResults*)(malloc(sizeof(LifeResults)));
	
	result->results = (LifeState**)(malloc(10 * sizeof(LifeState*)));
	
	for(int i = 0; i < 10; i++)
	{
		(result->results)[i] = NewState();
	}
	
	result->allocated = 10;
	result->size = 0;
	
	return result;
}

void Add(LifeResults* results, LifeState* state)
{
	if(results->size == results->allocated)
	{
		results->results = (LifeState**)(realloc(results->results, results->allocated * 2 * sizeof(LifeState*)));
		results->allocated *= 2;
		
		for(int i = results->size; i < 2 * (results->size); i++)
		{
			(results->results)[i] = NewState();
		}
	}
	
	Copy((results->results)[results->size], state);
	results->size++;
}

void Add(LifeResults* results)
{
	Add(results, GlobalState);
}

char* ReadFile(const char *filePath)
{
	char* buffer = (char*) malloc(1);
	buffer[0] = '\0';
	long length;
	FILE * f = fopen (filePath, "r");

	if (f)
	{
	  fseek (f, 0, SEEK_END);
	  length = ftell (f);
	  fseek (f, 0, SEEK_SET);
	  buffer = (char*)realloc (buffer, length);
	  if (buffer)
	  {
			fread(buffer, 1, length, f); 
	  }
	  fclose (f);
	  
	}	
	
	return buffer;

}

void SaveResults(LifeResults* results, const char* filePath)
{
	FILE *f;
    f = fopen(filePath, "wb");
	
	for(int i = 0; i < results->size; i++)
	{	
		fputs(GetRLE((results->results)[i]), f);
		fprintf(f,	"129$");
	}
	
	fclose(f);
}

LifeResults* LoadResults(const char* filePath)
{
	LifeResults* results = NewResults();
	
	char* rle = ReadFile(filePath);
	int idx = 0;
	
	while(rle[idx] != '\0')
	{
		ClearData(Temp);
		idx = Parse(Temp, rle, idx);
		Move(Temp, -32, -32);
		Add(results, Temp);
	}
	
	return results;
}


typedef struct 
{
	int minx;
	int maxx;
	
	uint64_t minVal;
	uint64_t maxVal;
	
} LifeBox;

LifeBox* NewBox()
{
	LifeBox* result = (LifeBox*)(malloc(sizeof(LifeBox)));
	
	result->minx = 0;
	result->maxx = 0;
	
	result->minVal = 1ULL;
	result->maxVal = 1ULL;
	
	return result;	
}

LifeBox* NewBox(int minx, int miny, int maxx, int maxy)
{
	LifeBox* result = (LifeBox*)(malloc(sizeof(LifeBox)));
	result->minx = minx + 32;
	result->maxx = maxx + 32;
	
	result->minVal = 1ULL << (miny + 32);
	result->maxVal = 1ULL << (maxy + 32);
	
	return result;
}

int StateWidth(LifeState* state)
{
	RefitMinMax(state);
	return state->max - state->min + 1;
}

int BoxWidth(LifeBox* box)
{
	return box->maxx - box->minx + 1;
}

int BoxX(LifeBox* box)
{
	return box->minx;
}

int BoxY(LifeBox* box)
{
	uint64_t tmp = 1ULL; 
	
	for(int i = 0; i <= N; i++)
	{
		if(tmp == box->minVal)
			return i; 
	}
}



int BoxHeight(LifeBox* box)
{
	int miny; 
	uint64_t tmp = 1ULL; 
	
	for(int i = 0; i <= N; i++)
	{
		if(tmp == box->minVal)
			miny = i;
		
		if(tmp == box->maxVal)
		{
			return i - miny + 1; 
		}
	}
	
	return -1; 
}

int IsInside(LifeState* state, LifeBox* box)
{
	for(int i = state->min; i <= state->max; i++)
	{
		uint64_t curVal = state->state[i];
		
		if(curVal == 0)
			continue;

		if(curVal > box->maxVal || curVal < box->minVal)
			return NO;
		
		if(i < box->minx)
			return NO;
	
		if(i > box->maxx)
			return NO;
	}
	
	return YES;
}


int GetBoundingBox(LifeState* state, LifeBox* box)
{
	if(IsEmpty(state))
		return FAIL; 
	
	RefitMinMax(state);
	uint64_t or_all = 0LL; 
	
	for(int i = state->min; i <= state->max; i++)
	{
		or_all |= state->state[i];
	}
	
	box->minx = state->min;
	box->maxx = state->max;
	
	box->minVal = 1ULL;
	box->maxVal = 1ULL << 63;
	
	for(int i = 0; i <= N; i++)
	{
		//example 0011001000
		//get 0010000000, 0000001000
		
		if(box->minVal <= or_all)
			box->minVal << 1;
		
		if(box->maxVal >= or_all)
			box->minVal >> 1;
		
	}
	
	box->minVal >> 1;
	box->maxVal << 1;
	
	return SUCCESS; 
	
}

int IsInside(LifeBox* box)
{
	return IsInside(GlobalState, box);
}
