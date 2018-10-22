#include "LSystem.hpp"
#include <fstream>
#include <sstream> 
#include <string>

	void LSystem::generate() {
		string nextTree = "";
		for (int i = 0; i < currentTree.length(); i++) {
			string current = "";
			current += currentTree.at(i);
			bool foundRule = false;

			for (Rule currentRule: Rules) {
				if (current == currentRule.base) {
					foundRule = true;

					uniform_real_distribution<float> distribution(0.0, 1);
					float t = distribution(randGen);

					// if  check if probability and then append tree
					if (t < currentRule.probability) {
						nextTree.append(currentRule.transform);
						break;
					}
				}

			}
			if (!foundRule) {
				nextTree.append(current);
			}
		}
		currentTree = nextTree;
		
	}

	void LSystem::setRules(){
		axiom = "A";
		currentTree = axiom;
		angle = radians(22.5f);

		Rule r1 = Rule("A", { "[&FL!A]/////[&FL!A]///////[&FL!A]"});
		Rule r2 = Rule("F", { "S/////F" });
		Rule r3 = Rule("S", { "FL" });
		Rule r4 = Rule("L", { "[^^T]" });
	

		Rules.push_back(r1);
		Rules.push_back(r2);
		Rules.push_back(r3);
		Rules.push_back(r4);

	}

	void LSystem::readRules(string filename) {
		ifstream file(filename);

		if (!file.is_open()) {
			cerr << "Failed to open file " << filename << endl;
			throw runtime_error("Error :: could not open file.");
		}
		while (file.good()) {

			string line;
			std::getline(file, line);
			istringstream lineStream(line);

			// Pulled from the file
			string key;
			vector<string> values;

			lineStream >> key;

			while (lineStream.good()) {
				string v;
				lineStream >> v;
				values.push_back(v);
			}


			if (key == "name") {
				name = values.at(0);
			}
			else if (key == "axiom") {
				axiom = values.at(0);
				currentTree = axiom;
			}
			else if (key == "angle") {
				angle = radians(stof(values.at(0)));
			}
			else if (key == "generations") {
				generations = stoi(values.at(0));
			}
			else if (key == "rule") {
				string base = values.at(0);
				string transform = values.at(1);
				//DO THE PROBABILITY
				Rule rule = Rule(base, transform);
				if (values.size() > 2) {
					rule = Rule(base, transform, stof(values.at(2)));
				}
				Rules.push_back(rule);

			}

		}
		for (int i = 0; i < generations; i++) {
			generate();
		}

	}

	void LSystem::resetTree()
	{
		currentTree = axiom;
	}
