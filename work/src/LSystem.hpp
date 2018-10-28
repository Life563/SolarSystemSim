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
		float probability = 1;
		string transform;

		//TODO: implement specific probabilities for branches eg 0.9 and 0.1
		Rule(string b, string t) : base(b), transform(t) {

		}
		Rule(string b, string t, float p) : base(b), transform(t), probability(p) {

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
	string name;
	int generations;

	void generate();
	void setRules();
	void readRules(string filename);
	void resetTree();
	void treeGeneration(int generation);
};