// Name: Het Bharatkumar Patel
// SID: 1742431
// CCID: hetbhara
// AnonID: 1000348298
// CMPUT 275, Winter 2023

// Assignment #1 Part 1: Navigation System
//  ---------------------------------------------------

#include "dijkstra.h"
#include "wdigraph.h"
#include <iostream>
#include <list>
#include <string>
#include <climits>
#include <fstream>
#include <map>
using namespace std;

/*
Description: Struct in which objects contains members of latitude and longitude for each object
*/
struct Point
{
    long long lat; // latitude of the point
    long long lon; // longitude of the point
};

/*
Description: Return the Manhattan distance between the two given points
Arguments:
        pt1 (Points&): instance of struct points
        pt2 (Points&): instance of struct points
Returns:
        Manhattan_distance (long long): Distance betweeen two given points
*/
long long manhattan(const Point &pt1, const Point &pt2)
{
    long long distance_lat = abs(pt1.lat - pt2.lat);
    long long distance_lon = abs(pt1.lon - pt2.lon);
    long long Manhattan_distance = distance_lat + distance_lon;
    return Manhattan_distance;
}

/*
Description: Read Edmonton map data from the provided file and
             load it into the WDigraph object passed to this function.
             Store vertex coordinates in Point struct and map each
             vertex identifier to its corresponding Point struct variable.

Arguments:
        filename: the name of the file that describes a road network
        graph: an instance of the weighted directed graph (WDigraph) class
        points: a mapping between vertex identifiers and their coordinates

Returns: None
*/
void readGraph(string filename, WDigraph &graph, unordered_map<int, Point> &points)
{
    int start, end, ID;
    string command, in_line, s_lat, s_lon, s_ID;
    double d_lat, d_lon;
    long long lat, lon;

    ifstream file(filename);

    while (getline(file, in_line))
    {
        command = in_line.substr(0, 1); // take first character of a line entered

        if (command == "V")
        {
            auto pos_comma1 = in_line.find(',');
            auto pos_comma2 = in_line.find(',', pos_comma1 + 1);
            auto pos_comma3 = in_line.find(',', pos_comma2 + 1);

            s_ID = in_line.substr(pos_comma1 + 1, pos_comma2 - pos_comma1 - 1); // find string between two commas ('ID' in string)
            ID = stoi(s_ID);                                                    // convert 'ID' to integer type
            graph.addVertex(ID);

            s_lat = in_line.substr(pos_comma2 + 1, pos_comma3 - pos_comma2 - 1); // find string between two commas ('Latitide' in string)
            s_lon = in_line.substr(pos_comma3 + 1);                              // find string between two commas ('Longitude' in string)

            d_lat = stod(s_lat); // convert the latitude and longitude from string type to double type
            d_lon = stod(s_lon);

            lat = static_cast<long long>(d_lat * 100000); // convert the latitude and longitude from double type to long long type
            lon = static_cast<long long>(d_lon * 100000);

            Point coordinate; // create an instance of a structure Point
            coordinate.lat = lat;
            coordinate.lon = lon;

            points[ID] = coordinate; // set the value to the key ID in unordered map points
        }
        else
        {
            auto pos_comma1 = in_line.find(',');
            auto pos_comma2 = in_line.find(',', pos_comma1 + 1);
            auto pos_comma3 = in_line.find(',', pos_comma2 + 1);
            string str_start = in_line.substr(pos_comma1 + 1, pos_comma2 - pos_comma1 - 1); // find string between first two commas ('start' in string)
            string str_end = in_line.substr(pos_comma2 + 1, pos_comma3 - pos_comma2 - 1);   // find string between two commas ('end' in string)
            start = stoi(str_start);                                                        // convert 'start' to integer type
            end = stoi(str_end);                                                            // convert 'end' to integer type
            graph.addEdge(start, end, manhattan(points[start], points[end]));               // add 'start' and 'end' point as an edge to the graph
        }
    }
}

/*
    Description: Finds the vertex closest to any given reference point

    Arguments:
            pt (Point struct): Given reference point
            points (unordered map): list containing every vertex and their points

    Returns:
            closest_vertex(int): The closest vertex to the reference point
*/
int FindClosestVertex(Point &pt, unordered_map<int, Point> &points)
{
    map<long long, int> points_dist;

    // Compute the distance between pt and all vertices on the map
    for (auto it = points.begin(); it != points.end(); it++)
    {
        long long distance = manhattan(pt, it->second);
        points_dist[distance] = it->first;
    }

    // Return vertex closest to the given point
    int closest_vertex = points_dist.begin()->second;
    return closest_vertex;
}

int main()
{
    WDigraph graph;

    // initalize searchTree and points
    unordered_map<int, PIL> searchTree;
    unordered_map<int, Point> points;

    readGraph("edmonton-roads-2.0.1.txt", graph, points); // use readGraph function to inialize the graph

    string req;
    long long lat1, lon1, lat2, lon2;
    cin >> req >> lat1 >> lon1 >> lat2 >> lon2;

    // coord1 has the latitude and longitude for start point
    // coord2 has the latitude and longitude for end point
    Point coord1, coord2;
    coord1.lat = lat1;
    coord1.lon = lon1;
    coord2.lat = lat2;
    coord2.lon = lon2;

    // find closest vertices by using FindClosestVertex function (for start and end point requested by user)
    int closest_vertex_start = FindClosestVertex(coord1, points);
    int closest_vertex_end = FindClosestVertex(coord2, points);

    dijkstra(graph, closest_vertex_start, searchTree);

    list<int> waypoint;

    if (searchTree.find(closest_vertex_end) != searchTree.end())
    {
        int step = closest_vertex_end;

        while (step != closest_vertex_start)
        {
            waypoint.push_front(step);     // extend the waypoint by adding the elements in front
            step = searchTree[step].first; // go up the tree by a step
        }
        waypoint.push_front(closest_vertex_start); // add the first vertex to the waypoints

        cout << "N"
             << " " << waypoint.size() << endl; // print the number of waypoints

        for (auto i : waypoint)
        {
            string response;
            cin >> response;
            if (response == "A")
            {
                cout << "W"
                     << " " << points[i].lat << " " << points[i].lon << endl; // print the coordinates of the waypoint
            }
        }
        cout << "E" << endl; // if user doesn't respond with "A"
    }
    else
    {
        cout << "N 0" << endl; // if there are no nodes
        cout << "E" << endl;
    }

    return 0;
}