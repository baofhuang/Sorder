#ifndef _UNITHEAP_H
#define _UNITHEAP_H

#include<cstdlib>
#include<vector>
#include<climits>
#include<algorithm>

#include "Util.h"

namespace Sorder
{

using namespace std;

const int INITIALVALUE=0;


class ListElement{
public:
	int key;
	int prev;
	int next;
};


class HeadEnd{
public:
	int first;
	int second;

	HeadEnd(){
		first=second=-1;
	}
};

	
class UnitHeap{
public:
	int* update;
	ListElement* LinkedList;
	vector<HeadEnd> Header;
	int top;
	int heapsize;

	UnitHeap(int size);
	~UnitHeap();
	void DeleteElement(const int index);
	int ExtractMax();
	void DecrementKey(const int index);
	void DecreaseTop();
	void ReConstruct();
	
	void IncrementKey(const int index){
		int key=LinkedList[index].key;
		const int head=Header[key].first;
		const int prev=LinkedList[index].prev;
		const int next=LinkedList[index].next;

		if(head!=index){
			LinkedList[prev].next=next;
			if(next>=0)
				LinkedList[next].prev=prev;
			
			int headprev=LinkedList[head].prev;
			LinkedList[index].prev=headprev;
			LinkedList[index].next=head;
			LinkedList[head].prev=index;
			if(headprev>=0)
				LinkedList[headprev].next=index;
		}

		LinkedList[index].key++;
#ifndef Release
		if(key+1>=Header.size()){
			cout << "IncrementKey: key+1>=Header.size()\t" << key+1 << endl;
			quit();
		}
#endif
		if(Header[key].first==Header[key].second)
			Header[key].first=Header[key].second=-1;
		else if(Header[key].first==index)
			Header[key].first=next;
		else if(Header[key].second==index)
			Header[key].second=prev;
		
		key++;
		Header[key].second=index;
		if(Header[key].first<0)
			Header[key].first=index;

		if(LinkedList[top].key<key){
			top=index;
		}

		if(key+4>=Header.size()){
			Header.resize(Header.size()*1.5);
		}		
	}
};

}


#endif
