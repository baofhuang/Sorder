#ifndef _GRAPH_H
#define _GRAPH_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <queue>
#include <algorithm>
#include <utility>
#include <cmath>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdint>
#include <chrono>

#include "Util.h"
#include "UnitHeap.h"

namespace Sorder
{

using namespace std;

class Vertex
{
public:
    int outstart;
    int outdegree;
    int instart;
    int indegree;

    Vertex()
    {
        outdegree=indegree=0;
        outstart=instart=-1;
    }
};

class Graph
{
public:
    int vsize;
    long long edgenum;
    string name;

    vector<Vertex> graph;
    vector<int> outedge;
    vector<int> inedge;

    string getFilename();
    void setFilename(string name);

    Graph();
    ~Graph();
    void clear();
    void readGraph(const string& fullname);
    void writeGraph(ostream&);
    void PrintReOrderedGraph(const vector<int>& order);
    void GraphAnalysis();
    void RemoveDuplicate(const string& fullname);

    void strTrimRight(string& str);
    static vector<string> split(const string &s, char delim);
    static vector<string>& split(const string &s, char delim, vector<string> &elems);

    void GapCount();
    double GapCost(vector<int>& order);
    void Transform();
    void Sorder(vector<int>& order, int hop,const int HDegree);
    void Fusion(int vid,int hop,vector<int>& order,vector<int>& type,int RdCount,int& seednum,int& highnum,int& lownum,int& seed,const int HDegree);
    void FusionEx(int vid,int hop,vector<int>& order,vector<int>& type,int RdCount,int& seednum,int& highnum,int& lownum,int& seed,const int HDegree);

    void RCMOrder(vector<int>& order);
    unsigned long long LocalityScore(const int w);
};

}

#endif

