﻿#include <iostream>
#include "RegexToDFA.h"
#include <stack>
#include <algorithm>
#include <map>
#include <gvc.h>

namespace RegexLib {

	void ST_to_DFA_transformer::clear() {
		start_state = nullptr;
		getting_states.clear();
		followPos.clear();
		alphabet.clear();
		states.clear();
		Node* ptr = get_root();
		std::stack <Node*> S;
		int i = 0;
		while (!S.empty() || ptr) {
			while (ptr) {
				S.push(ptr);
				ptr = ptr->left;
			}
			if (!S.empty()) {
				ptr = S.top();
				ptr->first.clear();
				ptr->last.clear();
				S.pop();
				ptr = ptr->right;
			}
		}
	}

	void ST_to_DFA_transformer::numerate() {
		Node* ptr = get_root();
		std::stack <Node*> S;
		int i = 0;
		while (!S.empty() || ptr) {
			while (ptr) {
				S.push(ptr);
				ptr = ptr->left;
			}
			if (!S.empty()) {
				ptr = S.top();

				if ((ptr->tag == A_node) || (ptr->tag == Empty_str)) {
					ptr->id_pos = i;
					if ((ptr->symb != "$") && (ptr->symb != "#")){
						alphabet.insert(ptr->symb);
					}
					i++;
				}
				S.pop();
				ptr = ptr->right;
			}
		}
	}

	void ST_to_DFA_transformer::set_nullable(Node* temp_node) {
		switch (temp_node->tag)
		{
		case Empty_str:
			temp_node->nullable = true;
			break;
		case A_node:
			temp_node->nullable = false;
			break;
		case And_node:
			temp_node->nullable = temp_node->left->nullable && temp_node->right->nullable;
			break;
		case Or_node:
			temp_node->nullable = temp_node->left->nullable || temp_node->right->nullable;
			break;
		case Star_node:
			temp_node->nullable = true;
			break;
		case Repeat_node:
			temp_node->nullable = temp_node->left->nullable;
			break;
		case Choise_group_node:
			temp_node->nullable = temp_node->left->nullable || temp_node->right->nullable;
			break;
		}
	}

	void ST_to_DFA_transformer::set_first(Node* temp_node) {
		switch (temp_node->tag)
		{
		case Empty_str:
			break;
		case A_node:
			temp_node->first.insert(temp_node->id_pos);
			break;
		case And_node:
			if (temp_node->left->nullable) {
				temp_node->first.insert(temp_node->left->first.begin(), temp_node->left->first.end());
				temp_node->first.insert(temp_node->right->first.begin(), temp_node->right->first.end());
			}
			else {
				temp_node->first.insert(temp_node->left->first.begin(), temp_node->left->first.end());
			}
			break;
		case Or_node:
			temp_node->first.insert(temp_node->left->first.begin(), temp_node->left->first.end());
			temp_node->first.insert(temp_node->right->first.begin(), temp_node->right->first.end());
			break;
		case Star_node:
			temp_node->first.insert(temp_node->left->first.begin(), temp_node->left->first.end());
			break;
		case Repeat_node:
			temp_node->first.insert(temp_node->left->first.begin(), temp_node->left->first.end());
			break;
		case Choise_group_node:
			temp_node->first.insert(temp_node->left->first.begin(), temp_node->left->first.end());
			temp_node->first.insert(temp_node->right->first.begin(), temp_node->right->first.end());
			break;
		}
	}

	void ST_to_DFA_transformer::set_last(Node* temp_node) {
		switch (temp_node->tag)
		{
		case Empty_str:
			break;
		case A_node:
			temp_node->last.insert(temp_node->id_pos);
			break;
		case And_node:
			if (temp_node->right->nullable) {
				temp_node->last.insert(temp_node->left->last.begin(), temp_node->left->last.end());
				temp_node->last.insert(temp_node->right->last.begin(), temp_node->right->last.end());
			}
			else {
				temp_node->last.insert(temp_node->right->last.begin(), temp_node->right->last.end());
			}
			break;
		case Or_node:
			temp_node->last.insert(temp_node->left->last.begin(), temp_node->left->last.end());
			temp_node->last.insert(temp_node->right->last.begin(), temp_node->right->last.end());
			break;
		case Star_node:
			temp_node->last.insert(temp_node->left->last.begin(), temp_node->left->last.end());
			break;
		case Repeat_node:
			temp_node->last.insert(temp_node->left->last.begin(), temp_node->left->last.end());
			break;
		case Choise_group_node:
			temp_node->last.insert(temp_node->left->last.begin(), temp_node->left->last.end());
			temp_node->last.insert(temp_node->right->last.begin(), temp_node->right->last.end());
			break;
		}
	}

	void ST_to_DFA_transformer::set_follow_pos(Node* temp_node) {
		switch (temp_node->tag)
		{
		case And_node:
			for (auto p : temp_node->left->last) {
				followPos[p].insert(temp_node->right->first.begin(), temp_node->right->first.end());
			}
			break;
		case Star_node:
			for (auto p : temp_node->left->last) {
				followPos[p].insert(temp_node->left->first.begin(), temp_node->left->first.end());
			}
			break;
		case Repeat_node:
			for (auto p : temp_node->left->last) {
				followPos[p].insert(temp_node->left->first.begin(), temp_node->left->first.end());
			}
			break;
		default:
			break;
		}
	}

	void ST_to_DFA_transformer::Calc_attributes(Node* ptr) {
		if (ptr->left) {
			Calc_attributes(ptr->left);
		}
		if (ptr->right) {
			Calc_attributes(ptr->right);
		}
		set_nullable(ptr);
		set_first(ptr);
		set_last(ptr);
		set_follow_pos(ptr);
	}

	std::map<std::string, std::set<long>> ST_to_DFA_transformer::get_regex_map() {
		std::map<std::string, std::set<long>> regex_map;
		Node* ptr = get_root();
		std::stack <Node*> S;
		int i = 0;
		while (!S.empty() || ptr) {
			while (ptr) {
				S.push(ptr);
				ptr = ptr->left;
			}
			if (!S.empty()) {
				ptr = S.top();
				if (ptr->id_pos > -1) {
					regex_map[ptr->symb].insert(ptr->id_pos);
				}
				S.pop();
				ptr = ptr->right;
			}
		}
		return regex_map;
	}

	state* find_state(std::vector<state*> set_state, std::set<long> state_positions) {
		for (auto state : set_state) {
			if (state->positions == state_positions) {
				return state;
			}
		}
		return NULL;
	}

	void ST_to_DFA_transformer::transform() {
		std::map<std::string, std::set<long>> regex_map = get_regex_map(); // каждому символу сопоставляется набор позиций,
		// на которых он находится

		start_state = new state();
		start_state->positions = get_root()->first;

		std::vector<state*> set_states;
		std::vector<state*> processed_states;
		set_states.push_back(start_state);
		//add start state to getting state
		for (auto pos : regex_map["$"]) {
			if ((start_state->positions.find(pos) != start_state->positions.end())
				&& (!find_state(getting_states, start_state->positions))) {
				getting_states.push_back(start_state);
				start_state->isgetting = true;
			}
		}

		while (!set_states.empty()) {
			state* temp_state = set_states.back();
			set_states.pop_back();
			

			for (auto a : alphabet) {
				std::set<long> symbol_positions = regex_map[a];
				std::set<long> state_positions;

				std::set<int> intersect;
				std::set_intersection(temp_state->positions.begin(), temp_state->positions.end(),
					symbol_positions.begin(), symbol_positions.end(),
					std::inserter(intersect, intersect.begin()));
				for (auto pos : intersect) {
					state_positions.insert(followPos[pos].begin(), followPos[pos].end());
				}
				std::vector<state*> union_;
				union_.insert(union_.end(), processed_states.begin(), processed_states.end());
				union_.insert(union_.end(), set_states.begin(), set_states.end());
				union_.push_back(temp_state);
				state* f_state = find_state(union_, state_positions);
				union_.clear();
				if (!f_state) {
					state* new_state = new state();
					new_state->positions = state_positions;
					for (auto pos : regex_map["$"]) {
						if ((new_state->positions.find(pos) != new_state->positions.end()) 
							&& (!find_state(getting_states, new_state->positions))) {
							getting_states.push_back(new_state);
							new_state->isgetting = true;
						}
					}
					
					temp_state->transitions[a] = new transition(a, temp_state, new_state);
					set_states.push_back(new_state);
				}
				else {
					temp_state->transitions[a] = new transition(a, temp_state, f_state);
				}
			}
			processed_states.push_back(temp_state);
		}
		states = processed_states;
	}

	std::string ToString(std::set<long> s) {
		std::string str;
		for (auto i : s) {
			str += std::to_string(i) + ",";
		}
		return str;
	}

	void ST_to_DFA_transformer::getDFAImg() {
		std::string graph = "digraph graphname {";
		std::vector<state*> set_states;
		std::vector<state*> whatched_states;
		set_states.push_back(start_state);
		int i = 0;
		std::map<std::set<long>, long> index_map;
		while (!set_states.empty()) {
			state* temp_state = set_states.back();
			set_states.pop_back();
			
			if (!index_map.count(temp_state->positions)) {
				graph += std::to_string(i) + " [label=\"" + ToString(temp_state->positions) + "\"];\n";
				index_map[temp_state->positions] = i;
				i++;
			}
			else {
				graph += std::to_string(index_map[temp_state->positions]) + " [label=\"" + ToString(temp_state->positions) + "\"];\n";
			}

			for (auto a: alphabet) {

				int first_ind, second_ind;
				if (!index_map.count(temp_state->transitions[a]->start->positions)) {
					index_map[temp_state->transitions[a]->start->positions] = i;
					first_ind = i;
					i++;
				}
				else {
					first_ind = index_map[temp_state->transitions[a]->start->positions];
				}
				if (!index_map.count(temp_state->transitions[a]->end->positions)) {
					index_map[temp_state->transitions[a]->end->positions] = i;
					second_ind = i;
					i++;
				}
				else {
					second_ind = index_map[temp_state->transitions[a]->end->positions];
				}
				std::string s = temp_state->transitions[a]->symbol;
				if (s == "\\") {
					s = "\\\\";
				}
				graph += std::to_string(first_ind) + "->" + std::to_string(second_ind) + " [label=\"" + s + "\"];\n";

				std::vector<state*> union_states;
				union_states.insert(union_states.end(), set_states.begin(), set_states.end());
				union_states.insert(union_states.end(), whatched_states.begin(), whatched_states.end());
				union_states.push_back(temp_state);
				state* f_state = find_state(union_states, temp_state->transitions[a]->end->positions);
				union_states.clear();
				if (!f_state) {
					set_states.push_back(temp_state->transitions[a]->end);
				}
			}
			whatched_states.push_back(temp_state);
		}

		graph += "}";
		Agraph_t* Graph = agmemread(graph.c_str());
		GVC_t* gvc = gvContext();
		if (Graph) {
			gvLayout(gvc, Graph, "dot");
			int res = gvRenderFilename(gvc, Graph, "jpeg", "D:\\Automata-Theory-main\\LAB2\\outDFA.jpg");
			agclose(Graph);
		}
		else {
			std::cout << "Graph was not built";
		}
	}

	std::vector<state*> remove_states(std::vector<state*> set_states, std::vector<state*> removing_states) {
		std::vector<state*>::iterator it;
		for (auto rm_state : removing_states) {
			for (it = set_states.begin(); it != set_states.end(); it++) {
				if (rm_state == *it) {
					set_states.erase(it);
					break;
				}
			}
		}
		return set_states;
	}

	bool belong(state* st1, std::vector<state*> set_states) {
		for (auto st2 : set_states) {
			if (st1 == st2) {
				return true;
			}
		}
		return false;
	}

	std::vector<state*> get_group_owner(state* st1, std::vector<std::vector<state*>> pi_splitting) {
		for (auto group : pi_splitting) {
			if (belong(st1, group)) {
				return group;
			}
		}
	}

	state* ST_to_DFA_transformer::union_states(std::vector<state*> group) {
		state* new_state = new state();
		std::map<state*, std::vector<transition*>> incoming_ribs;
		for (auto st : states) {
			for (auto a : alphabet) {
				incoming_ribs[st->transitions[a]->end].push_back(st->transitions[a]);
			}
		}
		for (auto st : group) {
			new_state->positions.insert(st->positions.begin(), st->positions.end());
			for (auto rib : incoming_ribs[st]) { //put the ends of incoming ribs
				rib->end = new_state;
			}
			
		}
		
		for (auto st : group) {
			for (auto a : alphabet) {
				new_state->transitions[a] = st->transitions[a];
				st->transitions[a]->start = new_state;		
			}
		}
		incoming_ribs.clear();
		return new_state;
	}

	void ST_to_DFA_transformer::minimize() {
		std::vector<std::vector<state*>> pi_splitting, new_pi_splitting;
		std::vector<state*> notgetting_states = states;
		notgetting_states = remove_states(notgetting_states, getting_states);
		pi_splitting.push_back(getting_states);
		pi_splitting.push_back(notgetting_states);
		new_pi_splitting = pi_splitting;
		do {
			pi_splitting = new_pi_splitting;
			for (auto group : pi_splitting) {
				
				for (auto st1 : group) {
					std::vector<state*> new_group;
					for (auto st2 : group) {
						for (auto a : alphabet) {
							std::vector<state*> gr1, gr2;
							gr1 = get_group_owner(st1->transitions[a]->end, pi_splitting);
							gr2 = get_group_owner(st2->transitions[a]->end, pi_splitting);
							if (gr1 != gr2) {
								new_group.push_back(st2);
								break;
							}
						}
					}
					if (new_group.size() > 0) {
						//разделяем мы эту же группу, поэтому возможно её не нужно искать
						std::vector<std::vector<state*>>::iterator it;
						for (it = new_pi_splitting.begin(); it != new_pi_splitting.end(); it++) { 
							if (*it == group) {
								*it = remove_states(group, new_group);
								break;
							}
						}

						new_pi_splitting.push_back(new_group);
						new_group.clear();
						break;
					}
					else {
						new_group.clear();
					}
				}
			}
		} while (new_pi_splitting != pi_splitting);
		
		getting_states.clear();
		std::map<std::string, std::set<long>> regex_map = get_regex_map();
		std::vector<state*> new_states;
		for (auto group : pi_splitting) {
			if (group.size() == 0) {
				continue;
			}
			state* new_state = union_states(group);
			if (belong(start_state, group)) {
				start_state = new_state;
			}
			for (auto pos : regex_map["$"]) {
				if (new_state->positions.find(pos) != new_state->positions.end()) {
					getting_states.push_back(new_state);
				}
			}
			new_states.push_back(new_state);
		}
		states.clear();
		states = new_states;
	}
}