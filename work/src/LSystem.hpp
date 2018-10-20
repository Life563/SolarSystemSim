#pragma once
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include<vector>
#include <random>
#include <chrono>
#include "glm/glm.hpp"

using namespace glm;
using namespace std;

class LSystem {

	struct Rule
	{
		string base;
		vector<string> transforms;
		//TODO: implement specific probabilities for branches eg 0.9 and 0.1
		Rule(string b, vector<string> t ): base(b), transforms(t) {
		
		}
	};

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	default_random_engine randGen = default_random_engine(seed);

	vector<string> variables;
	vector<Rule> Rules;

public:
	float angle;
	string axiom;
	string currentTree;

	void generate();
	void setRules();

};