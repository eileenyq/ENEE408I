#include "map.hpp"

int lineStatus;
vector<vector<int>> maze_map (MAX_VECT);

void bfs(int s, int dest, queue<int>& pathQueue)
{
    // Create a queue for BFS
    queue<int> q;

    // Initially mark all the vertices as not visited
    // When we push a vertex into the q, we mark it as
    // visited
    vector<bool> visited(maze_map.size(), false);

    // Mark the source node as visited and
    // enqueue it
    visited[s] = true;
    q.push(s);

    // Iterate over the queue
    while (!q.empty()) {

        // Dequeue a vertex from queue and print it
        int curr = q.front();
        q.pop();
        pathQueue.push(curr);
        cout << curr << " ";
        if (curr == dest) {
            return;
        }

        // Get all adjacent vertices of the dequeued
        // vertex curr If an adjacent has not been
        // visited, mark it visited and enqueue it
        for (int x : maze_map[curr]) {
            if (!visited[x]) {
                visited[x] = true;
                q.push(x);
            }
        }
    }
}

// Function to add an edge to the graph
void addEdge(int u, int v)
{
    maze_map[u].push_back(v);
    maze_map[v].push_back(u); // Undirected Graph
}

void printMap() {
    for (int i = 0; i < maze_map.size(); i++) {
        if(maze_map[i].size() > 0) {
            printf("Node %d's Neighbors: ", i);
            for (int j = 0; j < maze_map[i].size(); j++){
                printf("%d ", maze_map[i][j]);
            }
        } else {
            break;
        }
        printf("\n");
    }
    printf("\n");
}
