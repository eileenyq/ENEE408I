#ifndef MAP_HPP
#define MAP_HPP

#include <iostream>
#include <queue>
#include <vector>

using namespace std;

#define MAX_VECT 100

//extern vector<vector<int>> maze_map (MAX_VECT);

void bfs(int s, int dest, queue<int>& pathQueue);
void addEdge(int u, int v);
void printMap();

#endif
