#ifndef CIRCUIT_FINDER_H
#define CIRCUIT_FINDER_H

#include <algorithm>
#include <iostream>
#include <list>
#include <vector>

typedef std::list<int> NodeList;

class CircuitFinder
{
  std::vector<NodeList> AK;
  std::vector<int> Stack;
  std::vector<bool> Blocked;
  std::vector<NodeList> B;
  std::vector<std::vector<int>> output_list;
  int S{};
  int N;

  void unblock(int U);
  bool circuit(int V);
  void output();

public:
  // CircuitFinder(int N, int Array[N][])
  //   : AK(N), Blocked(N), B(N) {
  //   for (int I = 0; I < N; ++I) {
  //     for (int J = 0; J < N; ++J) {
  //       if (Array[I][J]) {
  //         AK[I].push_back(Array[I][J]);
  //       }
  //     }
  //   }
  //   this->N = N;
  // }

  CircuitFinder(int N, std::vector<std::pair<int, int>> Array)
    : AK(N), Blocked(N), B(N), S(0) {
    for (int I = 0; I < N; ++I) {
      AK[Array[I].first].push_back(Array[I].second);
      AK[Array[I].second].push_back(Array[I].first);
    }
    this->N = N;
  }


  std::vector<std::vector<int>>& run();
};

inline void CircuitFinder::unblock(int U)
{
  Blocked[U - 1] = false;

  while (!B[U - 1].empty()) {
    int W = B[U - 1].front();
    B[U - 1].pop_front();

    if (Blocked[W - 1]) {
      unblock(W);
    }
  }
}

inline bool CircuitFinder::circuit(int V)
{
  bool F = false;
  Stack.push_back(V);
  Blocked[V - 1] = true;

  for (int W : AK[V - 1]) {
    if (W == S) {
      output();
      F = true;
    } else if (W > S && !Blocked[W - 1]) {
      F = circuit(W);
    }
  }

  if (F) {
    unblock(V);
  } else {
    for (int W : AK[V - 1]) {
      auto IT = std::find(B[W - 1].begin(), B[W - 1].end(), V);
      if (IT == B[W - 1].end()) {
        B[W - 1].push_back(V);
      }
    }
  }

  Stack.pop_back();
  return F;
}

inline void CircuitFinder::output()
{
//  std::cout << "circuit: ";
//  for (auto I = Stack.begin(), E = Stack.end(); I != E; ++I) {
//    std::cout << *I << " -> ";
//  }
//  std::cout << *Stack.begin() << std::endl;
    std::vector<int> temp;
    for (auto I = Stack.begin(), E = Stack.end(); I != E; ++I) {
        temp.push_back(*I);
    }
    output_list.push_back(temp);
}

inline std::vector<std::vector<int>>& CircuitFinder::run()
{
  Stack.clear();
  S = 1;

  while (S < N) {
    for (int I = S; I <= N; ++I) {
      Blocked[I - 1] = false;
      B[I - 1].clear();
    }
    circuit(S);
    ++S;
  }

  return output_list;
}

#endif // CIRCUIT_FINDER_H