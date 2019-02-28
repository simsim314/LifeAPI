#include "LifeAPI.h"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include<omp.h>

#include <iterator>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std; 

class SingleSoup
{
public:
	vector<int> creation_seq; 
	LifeState* state; 
	int escaping_glider = -1; 
	bool in_use = false; 
	
	SingleSoup()
	{
		state = NewState();
	}
	
	~SingleSoup()
	{
		FreeState(state);
		free(state);
	}
	
	void Print()
	{
		for(auto i : creation_seq)
			cout << i << ",";
		
		cout << "\n";
	}
	
	string ToString()
	{
		
		std::stringstream ss;
		for(size_t i = 0; i < creation_seq.size(); ++i)
		{
		  if(i != 0)
			ss << ",";
			
		  ss << creation_seq[i];
		}
		
		return  ss.str();
	}
};

class SoupMemoryManager
{
public: 
	vector<SingleSoup*> memory_soups; 
	uint64_t idx; 
	uint64_t num_soup;
	uint64_t num_soup_delata;
	
	void Init(uint64_t num_soups)
	{
		for(uint64_t i = 0; i < num_soups; i++)
		{
			memory_soups.push_back(new SingleSoup());
		}
		
		idx = 0; 
		num_soup = num_soups;
		num_soup_delata = num_soup;
	}
	
	SingleSoup* GiveMeSoup()
	{	
		for(uint64_t i = 0; i < memory_soups.size(); i++)
		{
			uint64_t c = (idx + i) % memory_soups.size(); 
			
			if(memory_soups[c]-> in_use == false)
			{
				memory_soups[c]->in_use = true; 
				idx = c + 1; 
				
				ClearData(memory_soups[c]->state);
				return memory_soups[c];
			}
		}
		
		for(uint64_t i = 0; i < num_soup_delata; i++)
			memory_soups.push_back(new SingleSoup());
		
		cout << "Soups:" << memory_soups.size() << "\n";
		
		num_soup += num_soup_delata; 
		return GiveMeSoup();
	}
	
	void Clean(vector<SingleSoup*>& soups)
	{
		for(auto soup : soups)
			soup->in_use = false; 
	}
	
	uint64_t CountInUse()
	{
		uint64_t res = 0; 
		for(auto soup : memory_soups)
			if(soup->in_use)
				res++;
		
		return res; 
	}
};

class SoupGoo
{
public: 
	unordered_map<uint64_t, int> hash_list; 
	vector<SingleSoup*> all_soups; 
	vector<SingleSoup*> results; 
	vector<SingleSoup*> new_soups;
	
	SoupMemoryManager gc;
	int MAX_SOUPS = 750000;
	int dblck = 0;
	//int dblck = 7;
	int block_X = 0 - dblck; 
	int block_Y = -4 - dblck; 
	LifeState* initialBlock = NewState("2o$2o!", -4, 0);
	//LifeState* initialBlock = NewState("6bo$5bobo$5bobo$6bo2$b2o7b2o$o2bo5bo2bo$b2o7b2o2$6bo$5bobo$5bobo$6bo!", -7, -8);

	LifeState* gld = NewState("3o$o$bo!", 28, 28);
	
	LifeBox* left = NewBox(-31, -31, -27, 31);
	LifeBox* right = NewBox(27, -31, 31, 22);
	LifeBox* top = NewBox(-31, -31, 31, -26);
	LifeBox* bottom = NewBox(-31, 27, 22, 31);
	
	vector<LifeBox*> boxes = {left, right, top, bottom};
	
	SoupGoo()
	{
		New();
		gc.Init(1ULL<<19);
		
		PutState(initialBlock);
		SingleSoup* new_soup = gc.GiveMeSoup();
		new_soup->state = NewState();
		Copy(new_soup->state, GlobalState, OR);
		all_soups.push_back(new_soup);
		hash_list[GetHash()] = 1; 
	}
	
	//Add escaping gliders once
	//-1 = No
	//0 Yes
	//1 block in the same place 
	int IsResult()
	{
		if(GetPop() != 4)
			return false; 
		
		if(StateWidth(GlobalState) == 2)
			return true; 
		
		//if(GetPop() <= 12 && StateWidth(GlobalState) <= 6 && StateHeight(GlobalState) <= 6)
		//	return true; 
		
		//if(GetPop() == 4 && StateWidth(GlobalState) == 2)
		//	result = true; 

		//if(GetPop() == 24 && StateWidth(GlobalState) == 13)
		//{
		//	if(GlobalState->min != 23 && all_soups.size() > 50)
		//		result = true; 
		//}

		return false; 
	}
	
	//-1 false
	//0 true 
	//1 result 
	//the same block
	int ValidateAndUpdate()
	{
		if(IsResult())
		{
			return 1; 
		}	
		
		uint64_t hash = GetHash();
		auto pairIter = hash_list.find(hash);
		int result = 0; 
		
		#pragma omp critical
		{
			if(pairIter == hash_list.end())
			{
				hash_list[hash] = 1;
				result = 0; 
			}
			else
			{
				pairIter->second++;
				result = -1;
			}
		}
		
		return result;
	}
	
	bool IsViolation()
	{
		for(auto box : boxes)
		{
			if(EmptyInside(GlobalState, box) == NO)
				return true; 
		}
	
		return false; 
	}
	
	bool SoupRun()
	{
		for(int i = 0; i < 30; i++)
		{
			Run(1);
			
			if(IsViolation())
				return false; 
			
			if(all_soups.size() > 10000)
			{
				float barier = ((float)(all_soups.size()) - 10000.0) / (MAX_SOUPS - 10000);
				
				if(GetPop() > 130 - 70 * barier)
					return false;
			}
		}
		
		return true; 
	}
	
	void UpdateSoups(SingleSoup* old_soup, int operation, int validate)
	{		
		SingleSoup* new_soup = gc.GiveMeSoup();
		new_soup->creation_seq = old_soup->creation_seq;
		new_soup->creation_seq.push_back(operation);
		Copy(new_soup->state, GlobalState, OR);
		
		if(validate == 0) //not result but balid soup
			new_soups.push_back(new_soup);
		else //validate == 1 result 
			results.push_back(new_soup);
	}
	
	void IterSingleSoup(SingleSoup* good_soup)
	{
		New();
		PutState(good_soup->state);
		if(SoupRun())
		{
			int validate = ValidateAndUpdate();
			
			if(validate >= 0)
			{
				#pragma omp critical
				{
					UpdateSoups(good_soup, 0, validate);
				}
			}
		}
		
		New();
		PutState(good_soup->state);
		PutState(gld);
		
		if(SoupRun())
		{
			int validate = ValidateAndUpdate();
			
			if(validate >= 0)
			{
				#pragma omp critical
				{
					UpdateSoups(good_soup, 1, validate);
				}
			}
		}
	}
	
	void ClearHashs()
	{
		if(hash_list.size() < 2000000)
			return; 
		
		static std::vector<uint64_t> vals;

		for (const auto& elem: hash_list) 
		{	
			if(elem.second >= 4)
			{
				vals.push_back(elem.first);
			}
		}

		hash_list.clear();

		for(int i = 0; i < vals.size(); i++)
			hash_list[vals[i]] = 1;
		
		cout << "NewSize : " << hash_list.size() << "\n";
	}
	
	void CopyToAll(int num_remain)
	{
		vector<pair<int, int> > soup_priority; 
		
		int i = 0; 
		
		#pragma omp parallel for
		for(int i = 0; i < new_soups.size(); i++)
		{
			SingleSoup* soup = new_soups[i];
			
			if(IsEmpty(soup->state))
			{	
				#pragma omp critical
				soup_priority.push_back(pair<int, int>(10000, i));
			}
			else
			{
				New();
				PutState(soup->state);
				Run(150);
				
				#pragma omp critical
				soup_priority.push_back(pair<int, int>(StateWidth(GlobalState) + StateHeight(GlobalState), i));
			}
			
			i++;
		}
		
		sort(soup_priority.begin(), soup_priority.end()); 
		
		int idx = soup_priority[0].second;
		all_soups.clear();
		
		if(num_remain >= new_soups.size())
			all_soups = new_soups;
		else
		{
			gc.Clean(new_soups);
			
			for(i = 0; i < num_remain; i++)
			{
				all_soups.push_back(new_soups[soup_priority[i].second]);			
				all_soups[i]->in_use = true; 
			}
		}		
	}
	
	void IterateAll()
	{
		#pragma omp parallel 
		{
			#pragma omp for
			for(int i = 0; i < all_soups.size(); i++)
			{
				IterSingleSoup(all_soups[i]);
			}	
		}
		
		gc.Clean(all_soups);	

		CopyToAll(MAX_SOUPS);
		new_soups.clear();
		
		ClearHashs();
	}
	
	void SaveResults(const string& fname)
	{
		ofstream file;
		file.open(fname);
		
		for(int j = 0; j < results.size(); j++)
			file << results[j]->ToString() << endl;
		
		file.close();
	}

};

// Not parallelized well. 
int main()
{
	omp_set_num_threads(6);
	New();
	
	SoupGoo goo; 
	
	for(int i = 0; i < 100000; i++)
	{
		goo.IterateAll();
		cout << i << " , " << goo.all_soups.size() << " , " << goo.results.size() << "\n";
		
		goo.SaveResults(string("P30_results_") + to_string(i) + std::string(".txt"));
					
		//for(auto r : goo.results)
		//{
		//	r->Print();
		//}
	}
	
	cout << "\nFinish!!";
	getchar();
}