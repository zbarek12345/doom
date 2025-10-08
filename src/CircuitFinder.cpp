#include "headers/CircuitFinder.h"


CircuitFinder::CircuitFinder(int N, std::vector<std::pair<uint16_t, uint16_t> > edges) : N(N){
	this->edges = std::vector<std::multiset<uint16_t>>(N);

	for (auto& edge : edges) {
		this->edges[edge.first].emplace(edge.second);
		this->edges[edge.second].emplace(edge.first);
	}
}


void CircuitFinder::Calculate() {
	// Clear any existing cycles
	lines.clear();

	// Stack for iterative DFS
	std::stack<uint16_t> stack;
	uint16_t last_start = 0;

	while (last_start < N) {
		// Skip vertices with no edges
		while (last_start < N && edges[last_start].empty()) {
			last_start++;
		}
		if (last_start >= N) {
			return; // No more vertices with edges
		}

		// Start DFS from last_start
		uint16_t start = last_start;
		uint16_t next = *edges[start].begin();
		std::vector<uint16_t> cycle;
		stack.push(start);
		cycle.push_back(start);
		stack.push(next);
		cycle.push_back(next);

		edges[start].erase(edges[start].lower_bound(next));
		edges[next].erase(edges[next].lower_bound(start));

		while (!stack.empty()) {
			uint16_t current = stack.top();


			if (current == start) {
				while (!stack.empty()) {
					stack.pop();
				}
				cycle.pop_back();
				lines.push_back(cycle);
				cycle.clear();
			}
			// If current vertex has edges, explore the next neighbor
			else if (!edges[current].empty()) {
				next = *edges[current].begin();
				stack.push(next);
				cycle.push_back(next);

				// Remove the edge in both directions (undirected graph)
				edges[current].erase(edges[current].lower_bound(next));
				edges[next].erase(edges[next].lower_bound(current));
			} else {
				// Backtrack if no edges remain
				bool break_cond = false;
				while (!break_cond) {
					auto t_previous = stack.top();
					cycle.pop_back();
					stack.pop();
					auto t_current = stack.top();

					edges[t_previous].emplace(t_current);
					edges[t_current].emplace(t_previous);

					for (auto& edge : edges[t_current]) {
						if (edge>t_previous) {
							stack.push(edge);
							break_cond = true;
							break;
						}
					}
				}

				// If we return to the start vertex, we've found a cycle
				if (!stack.empty() && stack.top() == start) {
					lines.push_back(cycle);
					cycle.clear();
					stack.pop();
					// Start a new cycle from the next vertex with edges
					break;
				}
			}
		}
	}
}

std::vector<std::vector<uint16_t>>& CircuitFinder::GetLines() {
	return lines;
}