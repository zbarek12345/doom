#include <set>
#include <vector>
#include <cstdint>
#include <stack>

class CircuitFinder {
	uint16_t N;
	std::vector<std::vector<uint16_t>> lines;

	std::vector<std::multiset<uint16_t>> edges;

public:
	CircuitFinder(int N, std::vector<std::pair<uint16_t,uint16_t>> edges);

	void Calculate();

	std::vector<std::vector<uint16_t>>& GetLines();
};
