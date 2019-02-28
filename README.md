General Information
===

LifeAPI provides comfortable functions (API) to manipulate, search, evolve, iterate, compare and report Life objects. This is mainly done in order to provide fast (using C++) but still comfortable search utility. 

LifeAPI is C++ application (compiles only with c++ compiler, specifically g++ is recommended), that uses mainly the overload functionality of C++. It doesn't use classes, and compiling it will not require Make, you only need to #include "LifeAPI.h". Many times it's the only #include that you will need, for this sort of applications. It works as fast as C. 

LifeAPI can utilize all 64 bits of 64 bit machine. That means if you compile it on 64-bit, it will work twice as fast just due to 64 bit, just make sure to set the compiler options correctly. Of course it utilize the full potential of 32 bit machine as well. 

LifeAPI uses CamelCase convention for its functions, but it's very similar to golly python functionality, naming, and parameters input/overloads. As it was made to replace to some extent search I've made previously in golly, much of golly function naming and other conventions are present. 

How to use
---

1. Download  [LifeAPI.h](https://github.com/simsim314/LifeAPI/blob/master/LifeAPI.h)
2. Place it in the same folder with you code.
3. #include "LifeAPI.h" in your code. 
4. Compile with C++ compiler.*
5. See samples [here](https://github.com/simsim314/LifeAPI). 

Enjoy. 


**NOTE** LifeAPI is in rapid development so we update the UnitTest.c to its latest abilities (actually no commit is done before the UnitTest.c was updated and made sure to work properly, with the latest changes). The samples might be outdated, or even not work. 

Compilation
==========

LifeAPI.h is written in a way that benefits from AVX/SSE compilation flags enabled (called vectorization). 
Check what architecture you CPU supports SSE/AVX/AVX2 and add them to g++ compilation flags. 

For example: 

g++ "PerformanceTest.cpp" -o PerformanceTest -O3 [-mavx2|-mavx|-msse]

LifeAPI.h is also written with OpenMP support in mind. This means that all global objects are thread private. To compile with OpenMP one should add _-fopenmp_ flag

g++ "PerformanceTest.cpp" -o PerformanceTest -O3 -msse -fopenmp

Here is an example of well tuned g++ flags: 

g++ "PerformanceTest.cpp" -o PerformanceTest -O3 -fopenmp -mavx2 -fno-tree-loop-distribute-patterns -march=haswell

**NOTE** LifeAPI can also be compiled using MSVC. To enable AVX/SSE in VisualStudio right click on Project->Properties->C/C++->Command Line->Additional Options: add /arch:[IA32|SSE|SSE2|AVX|AVX2] /Qvec-report. It's recommended to use VS2015 as it has the best vectorization optimizations. 

API Documentation 
==========
---


General
====


LifeAPI works with object called **LifeState** (like cells in golly). Each state is 64x64. The iterations are on torus.  
Each LifeState is treated as part of the universe, with (0, 0) in the centre. Each LifeStates contains array of N longs, 
and Life Rule is hard coded into bitwise iteration. LifeStates also contain min and max - and don't iterate on all the space when it's not necessary. Automatic emitted glider removal was added, into emittedGldiers property of LifeState, with information about where the gliders were found, like their direction, generation and location. 
 
**NOTE:** min-max are not optimized to use torus, and patterns that use the torus properties of LifeAPI will work slower.

**NOTE:** LifeStates is "column" based, in the sense that long and thin patterns will work faster, but this is usually neglectable - unless one has very long and very thin pattern. 

**NOTE:** The space is 64x64 and it's currently intended to work with this limited space, it's enough for large amount of searches. 

---
Global Objects
====


First of all there are some static objects in LifeAPI. 

**GlobalState** - it's the "main state" (like state in golly). This is very important object, any funciton that emits LifeState as input, usually just works with *GlobalState*.

*Captures* - LifeAPI provide the option to capture object into some array of "Captures". This allows to return to the LifeState later. 

*Temp* - is just a temporary LifeState used in few places (so that malloc and free is not needed anywhere). 

Each LifeAPI application Starts with the line **New();** in main. This is made to initialize the GlobalState (empty universe), Capture, Temp etc. 

**NOTE:** GlobalState is not made to improve performance, and iterating any other state will works at the same speed, but it allows more simple coding (usually functions that emit the state as first parameter, mainpulate GlobalState). 

**NOTE** When using OpenMP, to make sure all LifeAPI global objects like **GlobalState** are thread private, one should call New() only after #pragma omp parallel. 

---
Initializing LifeState
====


`LifeState* NewState()`

`LifeState* NewState(char* rle)`

`LifeState* NewState(char* rle, int dx, int dy)`

`LifeState* NewState(char* rle, int dx, int dy, int dxx, int dxy, int dyx, int dyy)`

To crate new LifeState you first use one of the initializers. 

*NewState()* creates an empty state, otherwise you can pass *rle*, and *transformation* to it. 

One can also use the *Parse* (see lower) function after the initial NewState(). 

**NOTE:** Returns SUCCESS on succesfull parsing otherwise FAIL. Doesn't do anything if Parse failed

====

`int Parse(LifeState* lifeState, char* rle)`

`int Parse(LifeState* lifeState, char* rle, int dx, int dy)`

`int Parse(LifeState* lifeState, char* rle, int dx, int dy, int dx, int dy, int dxx, int dxy, int dyx, int dyy)`

Parse *rle* and place it at (0,0) - i.e. in the middle. 

**NOTE:** The overloads are doing the usual parsing, and then apply transformation if the parsing was succesfull, otherwise they leave the LifeState as is, and return FAIL. 

**NOTE:** Returns SUCCESS on succesfull parsing otherwise FAIL. Doesn't do anything if Parse failed.

---
Manipulating States
====


For any LifeState
---

`void Transform(LifeState* state, int dx, int dy)`

`void Move(LifeState* state, int x, int y)`

`void Transform(LifeState* state, int dx, int dy, int dxx, int dxy, int dyx, int dyy)`

Transforming is done by Transform. In the usual way golly does that. 

**NOTE:** Transform with rotation/reflection parameters is relatively slow function that takes the same amount as about 20 iterations. Inside the search use only Transform/Move function, prepare beforehand all necessary transformations. This option is made mainly to simplify setup. 

**NOTE:**  Move and Transform do exactly the same thing. It was created to conserve golly naming conventions. 

====

`void Copy(LifeState* main, LifeState* delta, char* op)`

The copy is made to apply "delta" LifeState onto main LifeState. 
The third parameter *op, should be one of the four strings:  "copy", "or", "xor", "and".


`void Copy(LifeState* main, LifeState* delta)`

Copy with *op = "copy". 


`void Join(LifeState* main, LifeState* delta)`

Copy with *op =  "or".


`void Join(LifeState* main, LifeState* delta, int dx, int dy)`

Fast joining operation - as fast as move, but doesn't change the delta state. 

====

`void SetCell(LifeState* state, int x, int y, int val)`

`int GetCell(LifeState* state, int x, int y)`

Set or get specific cell value. Works with one-two clock ticks, but might easily be overused. 

====

`void Evolve(LifeState* state, int numIters)`

Evolve pattern numIter generations.


====

`void Capture(LifeState* cap, int idx)`

Capture state into Captures array with index idx.


====

`void ClearData(LifeState* state)`

Clears all the data inside LifeState.

====

`void Inverse(LifeState* state)`

Sets the 0 to 1 and 1 to 0 of LifeState.


For GlobalState
---------

Almost each function that manipulates LifeState has counterpart (or overload) that manipulates GlobalState.

----

`int PutState(char* rle)`

`int PutState(char* rle, int x, int y)`

`int PutState(char* rle, int dx, int dy, int dxx, int dxy, int dyx, int dyy)`

These *PutState* overloads do to the GlobalState what *NewState* + *Parse* do to any state. 

**NOTE:** Returns SUCCESS on succesfull parsing otherwise FAIL. Doesn't do anything if Parse failed:



`void PutState(LifeState* state)`

`void PutState(LifeState* state, char* op)`

`void PutState(LifeState* state, int dx, int dy)`

`void PutState(LifeState* state, int dx, int dy, int dy, int dxx, int dxy, int dyx, int dyy)`

These *PutState* overloads *PutState* doing what *Copy* does to regular state, but to the GlobalState:

====

`void SetCell(int x, int y, int val)`

`void GetCell(int x, int y, int val)`

Overloads for GlobalState (no input state). 

====

`void Run(int numIter)`

Evolve GlobalState

====

`void New()`

Clears GlobalState (on the first call in main, initializes all statics) 


---
Getting Information and reporting States
====


For any LifeState
-----

`int GetPop(LifeState* state)`

Get number of cells turned on - uses fast built in function. 

====

`int AreEqual(LifeState* pat1, LifeState* pat2)`

Returns YES if pat1 and pat2 are the same otherwise returns NO

====

`int Contains(LifeState* main, LifeState* spark)`

Returns YES of main contains some "spark" on the same exact place - doesn't include the 0 state. 


====

`int ContainsInverse(LifeState* main, LifeState* inverseSpark)`

Returns YES of inverse of main contains some "inverseSpark". 


====

`void PrintRLE(LifeState  * lifeState)`

Print the rle of LifeState into console. 

**NOTE:** The rle comes out messed up many times due to console issue. Copy-paste it into text editor, and remove all "\n" from the rle, only then paste into golly 

====

`void Print(LifeState *state)`
Print the pattern into the console


For GlobalState
-----

`int GetPop()`

`void Print()`

`void PrintRLE()`

`int Contains(LifeState* spark)`

`int ContainsInverse(LifeState* inverseSpark)`


---
Iterators 
==============


General
---

Iterators are made to replace the need for huge amount of loops. Say we want to place two gliders in two places, without iterators we will make loop on x, y, s of the first gliders and x1, y1, s1 of the second glider. This is 6 loops one inside the other which is pretty annoying. With iterators you will simply create two LifeIterators, and single loop which iterates on them both. 

**LifeIterator** is small struct that has been built to easily iterate the usual searchable queries. 
Usually we take some pattern, place it in some square, and evolve it some iterations. 
Doing it for each place in this square. LifeIterator holds array of LifeStates, for each possible iteration to improve performance. 

===


Initialization
--

`LifeIterator* NewIterator(LifeState* state, int x, int y, int w, int h, int s)`

Usual LifeIterator initializer requires 6 parameter. NewIterator is creating the pattern and evolving it s times (holding the array of states in memory).

*state* - LifeState that should be placed. 
*x* - StartX 
*y* - StartY 
*w* - Width 
*h* - Height
*s* - Number of iterations to evolve in each place. 

**NOTE** Those are also the names of the properties of LifeIterator struct. 


`LifeIterator* NewIterator(LifeState* state, int x, int y, int w, int h, int s, char* op)`

If you don'y want to evolve the input *state*, *"leave"* can be added as *op* parameter to avoid evolving. 


`LifeIterator* NewIterator(LifeState* states[], int x, int y, int w, int h, int s)`

It's also possible to specify list of states, the iterator will copy the states and will iterate on them.

`LifeIterator* NewIterator(LifeState* state, int x, int y, int w, int h)`

It's also possible to drop the s parameter, this will mean no evolution to the pattern (same as setting s = 1).

`LifeIterator* NewIterator(int x, int y, int w, int h)`

You can also just iterate on (x, y) without using any life state. This iterator will have empty state as default. 


====

`int Next(LifeIterator* iter)`

Iterators have current state. *LifeIterator* has the following properties for the current state:

curx - current X location 
cury - current Y location 
curs - current State
States - input states array evolved (or otherwise given). 

The next operator, goes to the next thing that needs to be checked. It usually returns SUCCESS, while FAIL returned only when it finish, and returns to it's *initial state* after that. 


`int Next(LifeIterator* iter1, LifeIterator* iter2)`

`int Next(LifeIterator* iter1, LifeIterator* iter2, LifeIterator* iter3)`

Few iterator can be combined, and Next will work for few iterators as well, this works for up to 6 input iterators. 

`int Next(LifeIterator *iter1[], int numIters)`

If for some reason you have more than 6 iterators, or just want to use arrays, you can use this overload. 

**NOTE:** For two or more iterators Next function will print the state of the last iterator, when it changes. 

====

`void Reset(LifeIterator* iter)`

Set the current state to *initial state*. 


====

`void FreeIterator(LifeIterator* iter)`

Iterator is a bit heavy object, so it needs function to free memory. 

====

`void PutState(LifeState* state, LifeIterator* iter)`

LifeIterator can be placed in any LifeState object using PutState. 

`void PutState(LifeIterator* iter)`

This will place the iterator into GlobalState.

====

`void Print(LifeIterator* iter)`

Print iterator state (x, y, s)

`void Print(LifeIterator* iter, char*  name)`

Print iterator state as functional code.

====

`void SetCurrent(LifeIterator* iter, int curx, int cury, int curs)`

Set state of iterator (will continue to iterate from this state)

====

`int Validate(LifeIterator *iter1, LifeIterator *iter2)`

If two iterators searching the same space (like two gliders in the same square), 
Validate will return FAIL half the times, so that the pair space will search each pair only once. 
It's also works only in coinciding regions of both of the iterators (the space where the duplication will occur).

NOTE: if you use Validate, make sure the components are placed at same relative place (usually (0,0)), so that (i1, i2) and (i2, i1) will be the same. 

---
Search for target
======================


General
---

LifeTargets are made to simplify the search of specific target inside any LifeState. LifeTarget is struct that contains target, and negative of it (the expected on and off cells). While the negative, is with "off" cells on. 


Usage
---

`LifeTarget* NewTarget(LifeState* wanted, LifeState* unwanted)`

NewTarget needs target and inverse. 

**NOTE:** Make sure the inverse and the target are placed relative to the same (0,0) (using rle, it's easy to mess up). 

`LifeTarget* NewTarget(LifeState* wanted)`

Calculates unwanted using GetBoundary.

`LifeTarget* NewTarget(const char* rle)`

Creates a pattern from rle as wanted, and calculates unwanted from it. 

`LifeTarget* NewTarget(const char* rle, int x, int y, int dxx, int dxy, int dyx, int dyy)`

Creates a pattern with transofrmation and then calculates the unwanted from it. 

`LifeTarget* NewTarget(const char* rle, int x, int y)`

The same as above only with movement transformation. 


====

`int ContainsTarget(LifeState* state, LifeTarget* target)`

Check if LifeState contains some target. Returns YES if it is and NO if not. 

`int ContainsTarget(LifeTarget* target)`

Check if GlobalState contains some target. Returns YES if it is and NO if not. 

====

`void FreeTarget(LifeTarget* iter)`

Free memory of LifeTarget. 

====


`void GetBoundary(LifeState* state, LifeState* boundary)`

To simplity the boundry off cells definition, you can use this funciotn. It gets the boundary of the input *state* and places it in *boundary* LifeState. 

`void GetBoundary(LifeState* state, int captureIdx)`

Uses *Captures* array, to place the boundary result (in captureIdx index). 

`void GetBoundary(LifeState* boundary)`

Place the Global state boundary into *boundary* object. 

`void GetBoundary(int captureIdx)`

Place the Global state boundary into *Captures* array (at index captureIdx)

---
Locate target
======================


General
---

Unlike LifeTarget that only confirms the target exists in certain place, the TargetLocator allows finding the location of specific Target. It performs very fast for small targets, which is what is needed for GOL. It uses bitwise operands to eliminate options, and it's probably much faster than FFT. 

Creating a new target is relatively time consuming, so prepare all your targets before the main search loop. 

Usage
---

`TargetLocator* NewTargetLocator()`

`TargetLocator* NewTargetLocator(const char* rle)`

`TargetLocator* NewTargetLocator(const char* rle, int x, int y)`

To create new TargetLocator use one of these. 


`TargetLocator* Target2Locator(LifeTarget* target)`

To convert LifeTarget to  TargetLocator use this. 

---


 `LocateTarget(LifeState* state, TargetLocator* targetLocator, LifeState* result)`
 
 To locate the target and place it in result (all places where result is not 0, the targets' (0,0) will be 1). 
 
 `LocateTarget(TargetLocator* targetLocator, LifeState* result)`
 
 To locate the target in GlobalState and place it in result. 
 



---
==============================
Written by Michael Simkin 2014
