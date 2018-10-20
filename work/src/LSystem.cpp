#include "LSystem.hpp"

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
					float t = distribution(randGen) *(currentRule.transforms.size()-1);
					int index = round(t);
					nextTree.append(currentRule.transforms.at(index));
					break;
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
