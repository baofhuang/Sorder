#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <set>
#include <functional>
#include <climits>
#include <ctime>
#include <stdlib.h>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <chrono>

#include "Graph.h"
#include "Util.h"

using namespace std;
using namespace Sorder;

const int INPUTNUM=1;

int main(int argc, char* argv[]){
	ios::sync_with_stdio(false);
	int i;
	int k = 2;
	clock_t start, end;
	string filename;

	if(argc==1){
		cout << "please provide parameter" << endl;
		quit();
	}

	i=1;
	while(i<argc){
		if(strcmp("-k", argv[i])==0){
			i++;
			k=atoi(argv[i]);
			if(k<=0){
				cout << "k should be larger than 0" << endl;
				quit();
			}
			i++;
		}
		else{
			filename=argv[i++];
		}
	}

	srand(time(0));

	Graph g;
	string name;
	name=extractFilename(filename.c_str());
	g.setFilename(name);

	start=clock();
	g.readGraph(filename);
	g.Transform();
	cout << name << " readGraph is complete." << endl;
	end=clock();
	cout << "Time Cost: " << (double)(end-start)/CLOCKS_PER_SEC << endl;

	start=clock();
	vector<int> order;
	g.Sorder(order,k,50);
	end=clock();
	cout << "ReOrdered Time Cost: " << (double)(end-start)/CLOCKS_PER_SEC << endl;
	cout << "Begin Output the Reordered Graph" << endl;
	g.PrintReOrderedGraph(order);
	cout << endl;
}

