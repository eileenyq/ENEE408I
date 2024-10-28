#ifndef MAP_HPP
#define MAP_HPP

#include <iostream>
#include <queue>
#include <vector>

using namespace std;

#define MAX_VECT 100
extern int lineStatus;

//Definitions for States
#define T_SHAPE 2
#define WHITE_BOX 3
#define PLUS_SHAPE 4
#define LEFT_CORNER 5
#define LEFT_STRAIGHT 7
#define RIGHT_CORNER 8
#define RIGHT_STRAIGHT 10


struct node {
    int id; 
    int type; 
};

void bfs(int s, int dest, queue<int>& pathQueue);
void addEdge(int u, int v);
void printMap();

#endif
