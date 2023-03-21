// Name: Het Bharatkumar Patel
// SID: 1742431
// CCID: hetbhara
// AnonID: 1000348298
// CMPUT 275, Winter 2023

// Assignment #1 Part 1: Navigation System
//  ---------------------------------------------------

#include <iostream>
#include <unordered_map>
#include <list>
#include <utility>
#include <queue>
#include "wdigraph.h"
#include "dijkstra.h"
using namespace std;

// typedef creates an alias for the specified type
// PIL is the value type of our searchTree
typedef pair<int, long long> PIL;

// PIPIL is used to insert a key-value pair in our searchTree
// if we declare a variable 'x' as follows:  PIPIL x;
// x.first gives the start vertex of the edge,
// x.second.first gives the end vertex of the edge,
// x.second.second gives the cost of the edge
typedef pair<int, PIL> PIPIL;

/* 	Description: Comparison class for the priority queue template.
                     Effectively makes the priority queue a min heap.

    Arguments: None.

    Returns: Nothing.
*/
class Greater
{
public:
    bool operator()(const PIPIL &lhs, const PIPIL &rhs) const
    {
        // min heap using distance/weight values in PIPIL
        return (lhs.second.second > rhs.second.second);
    }
};

/*
Description: Uses dijkstra's algorithm to compute least cost paths starting from a given vertex.
             Some of these code is taken from the lecture code implemented in CMPUT 275 lecture 14
             I have used queue instead of list to make the algorithm more efficient

Arguments:
        graph (WDigraph): The graph containing all edged ans vertices for Edmonton city map
        startVertex (integer): The start vertex
        searchTree (unordered_map): A search tree used to construct the least cost path to some vertex

Returns: None
*/
void dijkstra(const WDigraph &graph, int startVertex, unordered_map<int, PIL> &searchTree)
{

    // used min heap here for efficient implementation
    priority_queue<PIPIL, vector<PIPIL>, Greater> fires;

    // at time 0, the startVertex burns, we set the predecesor of
    // startVertex to startVertex (as it is the first vertex)
    fires.push(PIPIL(startVertex, PIL(startVertex, 0)));

    while (!fires.empty())
    {
        // finding the fire that reaches its endpoint earliest
        auto earliestFire = fires.top();

        int v = earliestFire.first;
        int u = earliestFire.second.first;
        long long d = earliestFire.second.second;

        // remove this fire
        fires.pop();

        // if v is already "burned", there nothing to do
        if (searchTree.find(v) != searchTree.end())
        {
            continue;
        }

        // record that 'v' is burned at time 'd' by a fire started from 'u'
        searchTree[v] = PIL(u, d);

        // now start fires from all edges exiting vertex 'v'
        for (auto iter = graph.neighbours(v); iter != graph.endIterator(v); iter++)
        {
            int nbr = *iter;

            // 'v' catches on fire at time 'd' and the fire will reach 'nbr'
            // at time d + (length of v->nbr edge)
            long long t_burn = d + graph.getCost(v, nbr);
            fires.push(PIPIL(nbr, PIL(v, t_burn)));
        }
    }
}