// Name: Het Bharatkumar Patel
// SID: 1742431
// CCID: hetbhara
// AnonID: 1000348298
// CMPUT 275, Winter 2023

// Assignment #1 Part 2: Navigation System
//  ---------------------------------------------------

#include <iostream>
#include <cassert>
#include <fstream>
#include <string>
#include <list>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <cstring>

#include "wdigraph.h"
#include "dijkstra.h"

struct Point
{
  long long lat, lon;
};

// return the manhattan distance between the two points
long long manhattan(const Point &pt1, const Point &pt2)
{
  long long dLat = pt1.lat - pt2.lat, dLon = pt1.lon - pt2.lon;
  return abs(dLat) + abs(dLon);
}

// find the ID of the point that is closest to the given point "pt"
int findClosest(const Point &pt, const unordered_map<int, Point> &points)
{
  pair<int, Point> best = *points.begin();

  for (const auto &check : points)
  {
    if (manhattan(pt, check.second) < manhattan(pt, best.second))
    {
      best = check;
    }
  }
  return best.first;
}

// read the graph from the file that has the same format as the "Edmonton graph" file
void readGraph(const string &filename, WDigraph &g, unordered_map<int, Point> &points)
{
  ifstream fin(filename);
  string line;

  while (getline(fin, line))
  {
    // split the string around the commas, there will be 4 substrings either way
    string p[4];
    int at = 0;
    for (auto c : line)
    {
      if (c == ',')
      {
        // start new string
        ++at;
      }
      else
      {
        // append character to the string we are building
        p[at] += c;
      }
    }

    if (at != 3)
    {
      // empty line
      break;
    }

    if (p[0] == "V")
    {
      // new Point
      int id = stoi(p[1]);
      assert(id == stoll(p[1])); // sanity check: asserts if some id is not 32-bit
      points[id].lat = static_cast<long long>(stod(p[2]) * 100000);
      points[id].lon = static_cast<long long>(stod(p[3]) * 100000);
      g.addVertex(id);
    }
    else
    {
      // new directed edge
      int u = stoi(p[1]), v = stoi(p[2]);
      g.addEdge(u, v, manhattan(points[u], points[v]));
    }
  }
}

int create_and_open_fifo(const char *pname, int mode)
{
  // create a fifo special file in the current working directory with
  // read-write permissions for communication with the plotter app
  // both proecsses must open the fifo before they perform I/O operations
  // Note: pipe can't be created in a directory shared between
  // the host OS and VM. Move your code outside the shared directory
  if (mkfifo(pname, 0666) == -1)
  {
    cout << "Unable to make a fifo. Make sure the pipe does not exist already!" << endl;
    cout << errno << ": " << strerror(errno) << endl
         << flush;
    exit(-1);
  }

  // opening the fifo for read-only or write-only access
  // a file descriptor that refers to the open file description is
  // returned
  int fd = open(pname, mode);

  if (fd == -1)
  {
    cout << "Error: failed on opening named pipe." << endl;
    cout << errno << ": " << strerror(errno) << endl
         << flush;
    exit(-1);
  }

  return fd;
}

// keep in mind that in part 1, the program should only handle 1 request
// in part 2, you need to listen for a new request the moment you are done
// handling one request
int main()
{
  WDigraph graph;
  unordered_map<int, Point> points;

  const char *inpipe = "inpipe";
  const char *outpipe = "outpipe";

  // Open the two pipes
  int in = create_and_open_fifo(inpipe, O_RDONLY);
  cout << "inpipe opened..." << endl;
  int out = create_and_open_fifo(outpipe, O_WRONLY);
  cout << "outpipe opened..." << endl;

  // build the graph
  readGraph("server/edmonton-roads-2.0.1.txt", graph, points);

  while (true)
  {
    char Buffer[1024];

    // read a request
    read(in, Buffer, 1024);

    // close program if client is done sending the requests
    if (Buffer[0] == 'Q')
    {
      break;
    }

    // convert request to string
    string strBuffer;
    strBuffer = Buffer;

    // find the positon of space and new line character in the string
    int space_pos = strBuffer.find(" ");
    int newline_pos = strBuffer.find("\n");
    // find the coordinates (latitude and longitude) in data type double
    double d_Slat = stod(strBuffer.substr(0, space_pos));
    double d_Slon = stod(strBuffer.substr(space_pos + 1, newline_pos - space_pos - 1));
    space_pos = strBuffer.find(" ", newline_pos + 1);
    double d_Elat = stod(strBuffer.substr(newline_pos, space_pos - newline_pos - 1));
    double d_Elon = stod(strBuffer.substr(space_pos + 1));

    // initialize the starting and ending points using latitudes and longitudes we found above
    Point sPoint, ePoint;
    sPoint.lat = static_cast<long long>(d_Slat * 100000.0); // convert the latitude and longitude from double type to long long type
    sPoint.lon = static_cast<long long>(d_Slon * 100000.0);
    ePoint.lat = static_cast<long long>(d_Elat * 100000.0);
    ePoint.lon = static_cast<long long>(d_Elon * 100000.0);

    // find closest vertices by using findClosest function (for start and end point requested)
    int start = findClosest(sPoint, points), end = findClosest(ePoint, points);

    // run dijkstra's algorithm, this is the unoptimized version that
    // does not stop when the end is reached but it is still fast enough
    unordered_map<int, PIL> tree;
    dijkstra(graph, start, tree);

    // if no path is found
    if (tree.find(end) == tree.end())
    {
      char last_line[2] = {'E', '\n'};
      write(out, last_line, 2);
    }
    else
    {
      // read off the path by stepping back through the search tree
      list<int> path;
      while (end != start)
      {
        path.push_front(end);
        end = tree[end].first;
      }
      path.push_front(start);

      // write path to pipeline
      for (int v : path)
      {
        // obtain coordinates (latitude and longitude) to be written in pipeline in string type
        string out_lat = to_string(static_cast<double>(points[v].lat / 100000.0));
        string out_lon = to_string(static_cast<double>(points[v].lon / 100000.0));

        // write coordinates to pipeline in form of "latitude longitude 'newline'"
        string writeBuffer = out_lat + " " + out_lon + "\n";
        write(out, writeBuffer.c_str(), writeBuffer.size());
      }
    }
    // write E at the end of the path
    char last_line[2] = {'E', '\n'};
    write(out, last_line, 2);
  }
  // close and unlink both pipes
  close(in);
  close(out);
  unlink(inpipe);
  unlink(outpipe);
  return 0;
}