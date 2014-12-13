General Information
===

LifeAPI provides comfortable functions (API) to manipulate, iterate, evolve, compare and report Life objects. This is mainly done in order to provide fast (using C++) but still comfortable search utility. 

LifeAPI is C++ application (compiles only with c++ compiler, specifically g++ is recommended), that uses mainly the overload functionality of C++. It doesn't use classes, and compiling it will not require Make, you only need to #include "LifeAPI.h". Many times it's the only #include that you will need, for this sort of applications. It works as fast as C. 

LifeAPI can utilize all 64 bits of 64 bit machine. That means if you compile it on 64-bit, it will work twice as fast just due to 64 bit, just make sure to set the compiler options correctly. Of course it utilize the full potential of 32 bit machine as well. 

LifeAPI uses CamelCase convention for its functions, but it's very similar to golly python functionality, naming, and parameters input/overloads. As it was made to replace to some extent search I've made previously in golly, much of golly function naming and other conventions are present. 



API Documentation 
==========
---
---



General
---

LifeAPI works with object called **LifeState** (like cells in golly). Each state is 64x64. The iterations are on torus.  
Each LifeState is treated as part of the universe, with (0, 0) in the centre. Each LifeStates contains array of N longs, 
and Life Rule is hard coded into bitwise iteration. LifeStates also contains min and max - and doesn't iterate on all the space when it's not necessary. 
 
**NOTE:** min-max are not optimized to use torus, and patterns that use the torus properties of LifeAPI will work slower.

**NOTE:** LifeStates is "column" based, in the sense that long and thin patterns will work faster, but this is usually neglectable - unless one has very long and very thin pattern. 

**NOTE:** The space is 64x64 and it's currently intended to work with this limited space, it's enough for large amount of serchs. 

---
Global Objects
---

First of all there are some static objects in LifeAPI. 

**GlobalState** - it's the "main state" (like state in golly). This is very important object, any funciton that emits LifeState as input, usually just works with *GlobalState*.

*Captures* - LifeAPI provide the option to capture object into some array of "Captures". This allows to return to the LifeState later. 

*Temp* - is just a temporary LifeState used in few places (so that malloc and free is not needed anywhere). 

Each LifeAPI application Starts with the line **New();** in main. This is made to initialize the GlobalState (empty universe), Capture, Temp etc. 

**NOTE:** GlobalState is not made to improve performance, and iterating any other state will works at the same speed, but it allows more simple coding (usually functions that emit the state as first parameter, mainpulate GlobalState). 


---
Initializing LifeState
---

**LifeState* NewState()**

**LifeState* NewState(char* rle)**

**LifeState* NewState(char* rle, int dx, int dy)**

**LifeState* NewState(char* rle, int dx, int dy, int dxx, int dxy, int dyx, int dyy)**

To crate new LifeState you first use one of the initializers. 

*NewState()* creates an empty state, otherwise you can pass *rle*, and *transformation* to it. 

One can also use the *Parse* (see lower) function after the initial NewState(). 

**NOTE:** Returns SUCCESS on succesfull parsing otherwise FAIL. Doesn't do anything if Parse failed

====

**int Parse(LifeState* lifeState, char* rle)**


**int Parse(LifeState* lifeState, char* rle, int dx, int dy)**


**int Parse(LifeState* lifeState, char* rle, int dx, int dy, int dx, int dy, int dxx, int dxy, int dyx, int dyy)**

Parse *rle* and place it at (0,0) - i.e. in the middle. 

**NOTE:** The overloads are doing the usual parsing, and then apply transformation if the parsing was succesfull, otherwise they leave the LifeState as is, and return FAIL. 

**NOTE:** Returns SUCCESS on succesfull parsing otherwise FAIL. Doesn't do anything if Parse failed.

---
Manipulating States
---


###For any LifeState
---

**void Transform(LifeState* state, int dx, int dy)**

**void Move(LifeState* state, int x, int y)**

**void Transform(LifeState* state, int dx, int dy, int dxx, int dxy, int dyx, int dyy)**

Transforming is done by Transform. In the usual way golly does that. 

**NOTE:** Transform with rotation/reflection parameters is relatively slow function that takes the same amount as about 20 iterations. Inside the search use only Transform/Move function, prepare beforehand all necessary transformations. This option is made mainly to simplify setup. 

**NOTE:**  Move and Transform do exactly the same thing. It was created to conserve golly naming conventions. 

====

**void Copy(LifeState* main, LifeState* delta, char* op)**

The copy is made to apply "delta" LifeState onto main LifeState. 
The third parameter *op, should be one of the four strings:  "copy", "or", "xor", "and".


**void Copy(LifeState* main, LifeState* delta)**

Copy with *op = "copy". 


**void Join(LifeState* main, LifeState* delta)**

Copy with *op =  "or".


**void Join(LifeState* main, LifeState* delta, int dx, int dy)**

Fast joining operation - as fast as move, but doesn't change the delta state. 


====

Set or get specific cell value. Works with one-two clock ticks, but might easily be overused. 
void SetCell(LifeState* state, int x, int y, int val)
int GetCell(LifeState* state, int x, int y)

====

To evolve pattern numIter generations use: 
void Evolve(LifeState* state, int numIters)

====

To capture state into Captures array with index (idx): 
void Capture(LifeState* cap, int idx)

====

clears all the data inside LifeState
void ClearData(LifeState* state)

====

sets the 0 to 1 and 1 to 0 of LifeState
void Inverse(LifeState* state)

---------For GlobalState----------

Almost each function that manipulates LifeState has counterpart (or overload) that manipulates GlobalState


Returns SUCCESS on succesfull parsing otherwise FAIL. Doesn't do anything if Parse failed:
int PutState(char* rle)
int PutState(char* rle, int x, int y)
int PutState(char* rle, int dx, int dy, int dxx, int dxy, int dyx, int dyy)


PutState doing what Copy does but to the GlobalState: 
void PutState(LifeState* state)
void PutState(LifeState* state, char* op)
void PutState(LifeState* state, int dx, int dy)
void PutState(LifeState* state, int dx, int dy, int dy, int dxx, int dxy, int dyx, int dyy)

====

Overloads for GlobalState:
void SetCell(int x, int y, int val)
void GetCell(int x, int y, int val)

====

Evolve GlobalState: 
void Run(int numIter)

====

Clears GlobalState (on the first call initialize all statics) 
void New()

---

====================Getting Information and reporting States================

----------For any LifeState--------

get number of cells turned on - uses fast built in function. 
int GetPop(LifeState* state)

====

returns YES if pat1 and pat2 are the same otherwise returns NO
int AreEqual(LifeState* pat1, LifeState* pat2)

====

returns YES of main contains some "spark" on the same exact place - doesn't include the 0 state. 
int Contains(LifeState* main, LifeState* spark)

====

returns YES of inverse of main contains some "inverseSpark". 
int ContainsInverse(LifeState* main, LifeState* inverseSpark)

====

Print the rle of LifeState into console. 
void PrintRLE(LifeState  * lifeState)

NOTE: The rle comes out messed up many times due to console issue. Copy-paste it into text editor, and remove all "\n" from the rle, only then paste into golly 

====

Print the pattern into the console
void Print(LifeState *state)

---------For GlobalState----------


int GetPop()
void Print()
void PrintRLE()
int Contains(LifeState* spark)
int ContainsInverse(LifeState* inverseSpark)

---

===================== Iterators =================

LifeIterator is small struct that have been built to easily iterate the usual searchable queries. 
Usually we take some pattern, place it in some square, and evolve it some iterations. 
Doing it for each place in this square. LifeIterator  holds array of LifeStates, for each possible iteration to improve performance. 

LifeIterator initializer requires 6 parameter 
state - LifeState that should be placed. 
x - StartX 
y - StartY 
w - Width 
h - Height
s - Number of iterations to evolve in each place. 

====

NewIterator by default is creating the pattern and evolving it s times (holding the array of states in memory). "leave" can be added to avoid evolving. 
It's also possible to specify list of states, the iterator will copy the states and will iterate on them. It's also possible to drop the s parameter, 
this will mean no evolution to the pattern (same as setting s = 1). 

LifeIterator* NewIterator(LifeState* state, int x, int y, int w, int h)
LifeIterator* NewIterator(LifeState* state, int x, int y, int w, int h, int s)
LifeIterator* NewIterator(LifeState* states[], int x, int y, int w, int h, int s)
LifeIterator* NewIterator(LifeState* state, int x, int y, int w, int h, int s, char* op)

====

You can also just iterate on (x, y) without using any life state. This iterator will have empty state as default. 
LifeIterator* NewIterator(int x, int y, int w, int h)

LifeIterator has also 
curx - current X location 
cury - current Y location 
curs - current State
States - evolved states of input state

The next operator, goes to the next thing that needs to be checked. It usually returns SUCCESS, while FAIL returned only when it finish, and returns to 0 initial after that. 
int Next(LifeIterator* iter)

Few iterator can be combined, and Next will work for few iterators as well: 
int Next(LifeIterator* iter1, LifeIterator* iter2)
int Next(LifeIterator* iter1, LifeIterator* iter2, LifeIterator* iter2)

etc.. up to 6 input iterators. Or you can use array of iterators
int Next(LifeIterator *iter1[], int numIters)

NOTE: For two or more iterators Next function will print the state of the last iterator, when it changes. 

====

Set the current state to 0 initial state. 
void Reset(LifeIterator* iter)

====

Iterator is a bit heavy object, so it needs function to free memory. 
void FreeIterator(LifeIterator* iter)

====

Except of that, LifeIterator can be placed in any LifeState object using Join 
void PutState(LifeState* state, LifeIterator* iter)

Or into GlobalState
void PutState(LifeIterator* iter)

====

Print iterator state
void Print(LifeIterator* iter)

Print iterator state as functional code
void Print(LifeIterator* iter, char*  name)

====

Set state of iterator (will continue to iterate from this state)
void SetCurrent(LifeIterator* iter, int curx, int cury, int curs)

====

If two iterators searching the same space (like two gliders in the same square), 
Validate will return FAIL half the times, so that the pair space will search each pair only once. 
It's also works only in coinciding regions of both of the iterators (the space where the duplication will occur).

int Validate(LifeIterator *iter1, LifeIterator *iter2)

NOTE: if you use Validate, make sure the components are placed at same relative place (usually (0,0)), so that (i1, i2) and (i2, i1) will be the same. 

---

======================Search for target===============

LifeTarget is struct that contains target, and negative of it (the expected on and off cells). While the negative, is with "off" cells on. 

NewTarget needs target and inverse. 
LifeTarget* NewTarget(LifeState* target, LifeState* inverse)

NOTE: Make sure the inverse and the target are placed relative to the same (0,0) (using rle, it's easy to mess up). 

====

Check if LifeState contains some target. Returns YES if it is and NO if not. 
int ContainsTarget(LifeState* state, LifeTarget* target)

Check if GlobalState contains some target. Returns YES if it is and NO if not. 
int ContainsTarget(LifeTarget* target)

====

Freee memory of targer with
void FreeTarget(LifeTarget* iter)

====

---
==============================
Written by Michael Simkin 2014
