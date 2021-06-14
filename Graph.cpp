#include "Graph.h"

#ifdef __GNUC__
#define likely(cond) __builtin_expect(!!(cond), 1)
#define unlikely(cond) __builtin_expect(!!(cond), 0)
#else
#define likely(cond) (!!(cond))
#define unlikely(cond) (!!(cond))
#endif // GNUC

namespace Sorder
{

string Graph::getFilename()
{
    return name;
}

void Graph::setFilename(string name)
{
    this->name.swap(name);
}

Graph::Graph()
{
    edgenum=vsize=0;
}

Graph::~Graph()
{
}

void Graph::clear()
{
    vsize = 0;
    edgenum=0;
    name.clear();
    graph.clear();
    outedge.clear();
    inedge.clear();
}


void Graph::readGraph(const string& fullname)
{
    FILE* fp;
    fp=fopen(fullname.c_str(), "r");
    if(fp==NULL)
    {
        cout << "Fail to open " << fullname << endl;
        quit();
    }

    char line[40];
    int u, v;
    const char* str=NULL;

    vsize=0;
    edgenum=0;
    vector< pair<int, int> > edges;
    edges.reserve(100000000);

    while(feof(fp)!=true)
    {
        if(fgets(line, 40, fp))
        {
            u=v=0;
            str=line;
            while(isdigit(*str))
                u=u*10+(*str++ - '0');
            str++;
            while(isdigit(*str))
                v=v*10+(*str++ - '0');

            if(u==v)
                continue;
            edgenum++;
            if(u>vsize)
                vsize=u;
            if(v>vsize)
                vsize=v;

            edges.push_back(make_pair(u, v));
        }
    }
    vsize++;

    fclose(fp);
    graph.resize(vsize+1);
    for(long long i=0; i<edges.size(); i++)
    {
        graph[edges[i].first].outdegree++;
        graph[edges[i].second].indegree++;
    }
    graph[0].outstart=0;
    graph[0].instart=0;
    for(int i=1; i<vsize; i++)
    {
        graph[i].outstart=graph[i-1].outstart+graph[i-1].outdegree;
        graph[i].instart=graph[i-1].instart+graph[i-1].indegree;
    }

    sort(edges.begin(), edges.end(), [](const pair<int, int>& a, const pair<int, int>& b)->bool
    {
        if(a.first<b.first)
            return true;
        else if(a.first>b.first)
            return false;
        else
        {

            if(a.second<=b.second)
                return true;
            else
                return false;
        }

    });
    outedge.resize(edgenum);
    for(long long i=0; i<edges.size(); i++)
    {
        outedge[i]=edges[i].second;
    }

    vector< pair<int, int> >().swap(edges);
#ifndef Release
    vector<int> inpos(vsize);
    for(int i=0; i<vsize; i++)
    {
        inpos[i]=graph[i].instart;
    }
    inedge.resize(edgenum);
    for(int u=0; u<vsize; u++)
    {
        for(int j=graph[u].outstart; j<graph[u].outstart+graph[u].outdegree; j++)
        {
            inedge[inpos[outedge[j]]]=u;
            inpos[outedge[j]]++;
        }
    }
#endif

    cout << "vsize: " << vsize << endl;
    cout << "edgenum: " << edgenum << endl;
    graph[vsize].outstart=edgenum;
    graph[vsize].instart=edgenum;
}

void Graph::Transform()
{
    vector<int> order;
    RCMOrder(order);
    if(order.size()!=vsize)
    {
        cout << "order.size()!=vsize" << endl;
        quit();
    }
    if(graph.size()!=(vsize+1))
    {
        cout << "graph.size()!=(vsize+1)" << endl;
        quit();
    }

    vector<int>().swap(inedge);
    vector< pair<int, int> > edges;
    edges.reserve(edgenum);
    for(int i=0; i<vsize; i++)
    {
        for(int j=graph[i].outstart, limit=graph[i+1].outstart; j<limit; j++)
            edges.push_back(make_pair(order[i], order[outedge[j]]));
    }
    if(edges.size()!=edgenum)
    {
        cout << "edges.size()!=edgenum" << endl;
        quit();
    }

    for(int i=0; i<vsize; i++)
    {
        graph[i].outdegree=graph[i].indegree=0;
    }
    for(int i=0; i<edges.size(); i++)
    {
        graph[edges[i].first].outdegree++;
        graph[edges[i].second].indegree++;
    }

    graph[0].outstart=0;
    graph[0].instart=0;
    for(int i=1; i<vsize; i++)
    {
        graph[i].outstart=graph[i-1].outstart+graph[i-1].outdegree;
        graph[i].instart=graph[i-1].instart+graph[i-1].indegree;
    }
    graph[vsize].outstart=edgenum;
    graph[vsize].instart=edgenum;

    sort(edges.begin(), edges.end(), [](const pair<int, int>& a, const pair<int, int>& b)->bool
    {
        if(a.first<b.first)
            return true;
        else if(a.first>b.first)
            return false;
        else
        {

            if(a.second<=b.second)
                return true;
            else
                return false;
        }
    });

    outedge.resize(edgenum);
    for(long long i=0; i<edges.size(); i++)
    {
        outedge[i]=edges[i].second;
    }
    vector< pair<int, int> >().swap(edges);
    vector<int> inpos(vsize);
    for(int i=0; i<vsize; i++)
    {
        inpos[i]=graph[i].instart;
    }
    inedge.resize(edgenum);
    for(int u=0; u<vsize; u++)
    {
        for(int j=graph[u].outstart; j<graph[u].outstart+graph[u].outdegree; j++)
        {
            inedge[inpos[outedge[j]]]=u;
            inpos[outedge[j]]++;
        }
    }
}


void Graph::writeGraph(ostream& out)
{
    for(int u=0; u<vsize; u++)
    {
        for(int j=graph[u].outstart; j<graph[u].outdegree+graph[u].outstart; j++)
        {
            int v=outedge[j];
            out << u << '\t' << v << endl;
        }
    }
}


void Graph::PrintReOrderedGraph(const vector<int>& order)
{
    ofstream out((name+"_Sorder.txt").c_str());

    vector<int>().swap(inedge);

    vector< vector<int> > ReOrderedGraph(vsize);
    int u, v;
    for(int i=0; i<vsize; i++)
    {
        u=order[i];
        ReOrderedGraph[u].reserve(graph[i+1].outstart-graph[i].outstart);
        for(int j=graph[i].outstart; j<graph[i].outstart+graph[i].outdegree; j++)
        {
            v=order[outedge[j]];
            ReOrderedGraph[u].push_back(v);
        }
        sort(ReOrderedGraph[u].begin(), ReOrderedGraph[u].end());
    }
    /*
    	for(int u=0; u<vsize; u++){
    		sort(ReOrderedGraph[u].begin(), ReOrderedGraph[u].end());
    	}
    */
    for(int u=0; u<vsize; u++)
    {
        for(int j=0; j<ReOrderedGraph[u].size(); j++)
        {
            out << u << '\t' << ReOrderedGraph[u][j] << endl;
        }
    }
    out.close();
}


void Graph::GraphAnalysis()
{
    vector<int> tmp(vsize);
    for(int i=0; i<vsize; i++)
    {
        tmp[i]=graph[i].outdegree;
    }
    sort(tmp.begin(), tmp.end());

    cout << "outdegree:" << endl;
    vector<int>::iterator tmpit1, tmpit2;
    tmpit1=tmp.begin();
    for(int i=1; i<vsize; i*=10)
    {
        tmpit2=tmpit1;
        tmpit1=upper_bound(tmp.begin(), tmp.end(), i);
        cout << i << ": " << tmpit1-tmpit2 << endl;
    }


    for(int i=0; i<vsize; i++)
    {
        tmp[i]=graph[i].indegree;
    }
    sort(tmp.begin(), tmp.end());

    cout << "indegree:" << endl;
    tmpit1=tmp.begin();
    for(int i=1; i<vsize; i*=10)
    {
        tmpit2=tmpit1;
        tmpit1=upper_bound(tmp.begin(), tmp.end(), i);
        cout << i << ": " << tmpit1-tmpit2 << endl;
    }
}


void Graph::RemoveDuplicate(const string& fullname)
{
    FILE* fp;
    fp=fopen(fullname.c_str(), "r");
    if(fp==NULL)
    {
        cout << "Fail to open " << fullname << endl;
        quit();
    }

    char line[40];
    int u, v;
    const char* str=NULL;

    set< pair<int, int> > edges;

    while(feof(fp)!=true)
    {
        if(fgets(line, 40, fp))
        {
            u=v=0;
            str=line;
            while(isdigit(*str))
                u=u*10+(*str++ - '0');
            str++;
            while(isdigit(*str))
                v=v*10+(*str++ - '0');

            if(u==v)
                continue;

            edges.insert(make_pair(u, v));
        }
    }

    fclose(fp);

    cout << "after remove, the size is " << edges.size() << endl;

    ofstream fout;
    fout.open("NoDuplicate.txt");
    for(set< pair<int, int> >::iterator it=edges.begin(); it!=edges.end(); it++)
    {
        fout << it->first << '\t' << it->second << endl;
    }
    fout.close();
}


vector<string>& Graph::split(const string &s, char delim, vector<string> &elems)
{
    int begin, end;

    begin=0;
    end=s.find(delim);
    while(end!=string::npos)
    {
        elems.push_back(s.substr(begin, end-begin));
        begin=end+1;
        end=s.find(delim, begin);
    }
    if(begin!=s.size())
    {
        elems.push_back(s.substr(begin));
    }

    return elems;
}

vector<string> Graph::split(const string &s, char delim)
{
    vector<string> elems;
    return split(s, delim, elems);
}

void Graph::strTrimRight(string& str)
{
    string whitespaces(" \t\r\n");
    int index = str.find_last_not_of(whitespaces);
    if (index != string::npos)
        str.erase(index+1);
    else
        str.clear();
}

void Graph::GapCount()
{
    int* gap = new int[vsize];
    memset(gap, 0, sizeof(int)*vsize);

    for(int i=0; i<vsize; i++)
    {
        for(int j=graph[i].outstart+1; j<graph[i].outdegree+graph[i].outstart; j++)
        {
            gap[outedge[j]-outedge[j-1]]++;
        }
        gap[outedge[graph[i].outstart]]++;
    }

    double entropy=0;
    for(int i=0; i<vsize; i++)
    {
        if(gap[i]==0)
            continue;
        else
        {
            entropy+=(double)gap[i]/edgenum*log2((double)gap[i]/edgenum);
        }
    }
    cout << "shannon: " << entropy << endl;
    delete[] gap;
}

double Graph::GapCost(vector<int>& order)
{
    double gaplog=0;
    double gaplog2=0;
    vector<int> edgelist;
    edgelist.reserve(100000);
    for(int i=0; i<vsize; i++)
    {
        for(int j=graph[i].outstart+1; j<graph[i].outdegree+graph[i].outstart; j++)
        {
            if(outedge[j]-outedge[j-1])
                gaplog+=log(double(outedge[j]-outedge[j-1]))/log(double(2));
        }
        edgelist.clear();
        for(int j=graph[i].outstart; j<graph[i].outstart+graph[i].outdegree; j++)
        {
            edgelist.push_back(order[outedge[j]]);
        }
        sort(edgelist.begin(), edgelist.end());
        for(int j=1; j<edgelist.size(); j++)
        {
            if(edgelist[j]-edgelist[j-1])
                gaplog2+=log(double(edgelist[j]-edgelist[j-1]))/log(double(2));
        }
    }
    cout << "original average gap cost: " << gaplog/edgenum << endl;
    cout << "new average gap cost: " << gaplog2/edgenum << endl;

    return gaplog2/edgenum;
}


void Graph::Sorder(vector<int>& retorder, int hop,const int HDegree)
{
    vector<int> order;
    order.resize(vsize);
    vector<int> type;
    type.resize(vsize);
    struct round
    {
        int seedsize;
        int highnum;
        int lownum;
        int total;
    };

    //round
    vector<round> rd;
    rd.push_back({0,0,0,1});

    int RdCount = 0;
    int seedsize = 0;
    int highnum = 0;
    int lownum = 0;
    int seed = -1;

    for(int i=0; i<vsize; i++)
    {
        if(order[i]!=0)//Assigned!
            continue;
        /*for FusionEx
        RdCount++;
        seedsize++;
        order[i] = RdCount;
        type[i] = 1;
        */
        seed = i;

        while(seed != -1)
        {
            seedsize = 0;
            highnum = 0;
            lownum = 0;
            RdCount++;

            Graph::Fusion(seed,hop,order,type,RdCount,seedsize,highnum,lownum,HDegree);
            rd.push_back({seedsize,highnum,lownum,seedsize+highnum+lownum});
        }
    }
    for(int i = 1; i<rd.size(); i++)
    {
        rd[i].lownum  = rd[i].highnum+rd[i].seedsize+rd[i-1].total;
        rd[i].highnum = rd[i].seedsize+rd[i-1].total;
        rd[i].seedsize = rd[i-1].total;
        rd[i].total += rd[i-1].total;
    }

    retorder.resize(vsize,-1);
    for(int i = 0; i<vsize; i++)
    {
        int od = order[i];
        switch(type[i])
        {
        case 1:
            retorder[i] = rd[od].seedsize++;
            break;
        case 2:
            retorder[i] = rd[od].highnum++;
            break;
        case 3:
            retorder[i] = rd[od].lownum++;
            break;
        }
    }
}

void Graph::Fusion(int& seed,int hop,vector<int>& order,vector<int>& type,const int RdCount,int& seedsize,int& highnum,int& lownum,const int HDegree)
{
    vector<int> parent;
    vector<int> children;
    parent.push_back(seed);
    seed = -1;
    //inner
    while(hop--)
    {
        for(int i = 0 ; i < parent.size(); i++) //fusion
        {
            int u = parent[i];
            if(order[u]==0)//The first seedvertex may have been included in the previous seed
                order[u] = RdCount;
            if(type[u]==0)
            {
                type[u] = 1;
                seedsize++;
            }
            for(int j = graph[u].instart, limit1=graph[u+1].instart; j<limit1; j++) ////inNeighbors
            {
                int w=inedge[j];
                if(order[w]!=0)
                    continue;
                if(graph[w].indegree >= HDegree)
                {
                    highnum++;
                    type[w] = 2;
                }
                else
                {
                    seedsize++;
                    type[w] = 1;
                }
                order[w] = RdCount;
                children.push_back(w);
            }//end inN
            for(int j=graph[u].outstart, limit1=graph[u+1].outstart; j<limit1; j++) //outNeighbors
            {
                int w=outedge[j];
                if(order[w]!=0)
                    continue;
                if(graph[w].indegree >= HDegree)
                {
                    highnum++;
                    type[w] = 2;
                }
                else
                {
                    seedsize++;
                    type[w] = 1;
                }
                order[w] = RdCount;
                children.push_back(w);
            }//end outN
        }//end fusion
        parent.swap(children);
        children.clear();
    }
    //outer
    for(int i = 0 ; i < parent.size(); i++)
    {
        int u = parent[i];
        if(order[u]==0)
            order[u] = RdCount;
        if(type[u]==0)
        {
            type[u] = 1;
            seedsize++;
        }

        for(int j = graph[u].instart, limit1=graph[u+1].instart; j<limit1; j++) ////inNeighbors
        {
            int w=inedge[j];
            if(order[w]!=0)
                continue;
            if(graph[w].indegree >= HDegree)
            {
                highnum++;
                type[w] = 2;
            }
            else
            {
                lownum++;
                type[w] = 3;
                seed = w; //next seedvertex
            }
            order[w] = RdCount;
        }//end inN

        for(int j=graph[u].outstart, limit1=graph[u+1].outstart; j<limit1; j++) //outNeighbors
        {
            int w=outedge[j];
            if(order[w]!=0)
                continue;
            if(graph[w].indegree >= HDegree)
            {
                highnum++;
                type[w] = 2;
            }
            else
            {
                lownum++;
                type[w] = 3;
                seed = w;//next seedvertex
            }
            order[w] = RdCount;
        }//end outN
    }
}

void Graph::FusionEx(int vid,int hop,vector<int>& order,vector<int>& type,const int RdCount,int& seedsize,int& highnum,int& lownum,int& seed,const int HDegree)
{

    if(hop==0) //outer
    {
        //inNeighbors
        for(int j = graph[vid].instart, limit1=graph[vid+1].instart; j<limit1; j++)
        {
            int u=inedge[j];
            if(order[u]!=0)
                continue;
            order[u] = RdCount;
            if(graph[u].indegree >= HDegree)
            {
                highnum++;
                type[u] = 2;
            }
            else
            {
                lownum++;
                type[u] = 3;
                seed = u;
            }
        }
        //outNeighbors
        for(int j=graph[vid].outstart, limit1=graph[vid+1].outstart; j<limit1; j++)
        {
            int u=outedge[j];
            if(order[u]!=0)
                continue;

            order[u] = RdCount;
            if(graph[u].indegree >= HDegree)
            {
                highnum++;
                type[u] = 2;
            }
            else
            {
                lownum++;
                type[u] = 3;
                seed = u;
            }

        }

        return;

    }

    //inner -->Fusion
    //inNeighbors
    for(int j=graph[vid].instart, limit1=graph[vid+1].instart; j<limit1; j++)
    {
        int u=inedge[j];
        if(order[u]!=0)

            continue;

        order[u] = RdCount;

        if(graph[u].indegree >= HDegree)
        {
            highnum++;
            type[u] = 2;

        }
        else
        {
            seedsize++;
            order[u] = RdCount;
            type[u] = 1;
            Graph::Fusion(u,hop-1,order,type,RdCount,seedsize,highnum,lownum,seed,HDegree);
        }
    }
    //outNeighbors
    for(int j=graph[vid].outstart, limit1=graph[vid+1].outstart; j<limit1; j++)
    {
        int u=outedge[j];
        if(order[u]!=0)
            continue;
        order[u] = RdCount;

        if(graph[u].indegree >= HDegree)
        {
            highnum++;
            order[u] = RdCount;
            type[u] = 2;
        }
        else
        {
            seedsize++;
            type[u] = 1;
            Graph::Fusion(u,hop-1,order,type,RdCount,seedsize,highnum,lownum,seed,HDegree);
        }
    }
}




void Graph::RCMOrder(vector<int>& retorder)
{
    queue<int> que;
    bool* BFSflag=new bool[vsize];
    bool* QueFlag=new bool[vsize];
    memset(BFSflag, 0, sizeof(bool)*vsize);
    memset(QueFlag, 0, sizeof(bool)*vsize);

    vector<int> tmp;
    vector<int> degreevertex(vsize);
    for(int i=0; i<vsize; i++)
    {
        degreevertex[i]=i;
    }

    sort(degreevertex.begin(), degreevertex.end(), [&](const int& a, const int& b)->bool
    {
        if(graph[a].outdegree+graph[a].indegree<graph[b].outdegree+graph[b].indegree)
            return true;
        else
            return false;
    });

    int now;
    vector<int> order;

    for(int k=0; k<vsize; k++)
    {
        int i=degreevertex[k];
        if(BFSflag[i]==false)
        {
            que.push(i);
//			QueFlag[i]=true;
            BFSflag[i]=true;
            order.push_back(i);

            while(que.empty()==false)
            {
                now=que.front();
                que.pop();

//				BFSflag[now]=true;
                tmp.clear();
                for(int it=graph[now].outstart, limit=graph[now+1].outstart; it<limit; it++)
                {
                    tmp.push_back(outedge[it]);
                }
                sort(tmp.begin(), tmp.end(), [&](const int& a, const int& b)->bool
                {
                    if(graph[a].outdegree+graph[a].indegree<graph[b].outdegree+graph[b].indegree)
                        return true;
                    else
                        return false;
                });
                if(tmp.size()!=graph[now].outdegree)
                    cout << "tmp.size()!=graph[now].outdegree" << endl;

                for(int i=0; i<tmp.size(); i++)
                {
//					if((BFSflag[tmp[i]]==false)&&(QueFlag[tmp[i]]==false)){
                    if(BFSflag[tmp[i]]==false)
                    {
                        que.push(tmp[i]);
                        BFSflag[tmp[i]]=true;
                        order.push_back(tmp[i]);
                    }
                }
            }
        }
    }

    delete[] BFSflag;
    delete[] QueFlag;

    if(order.size()!=vsize)
    {
        cout << "order.size()!=vsize" << endl;
        quit();
    }

    retorder.resize(vsize);
    for(int i=0; i<order.size(); i++)
    {
        retorder[order[i]]=order.size()-1-i;
    }
}


unsigned long long Graph::LocalityScore(const int w)
{
    unsigned long long sum=0;
    for(int i=0; i<vsize; i++)
    {
        for(int j=i-1; j>=i-w && j>=0; j--)
        {
            sum+=IntersectionSize(inedge.data()+graph[i].instart, inedge.data()+graph[j].instart, graph[i].indegree, graph[j].indegree, -1);
            if(binary_search(inedge.data()+graph[i].instart, inedge.data()+graph[i].instart+graph[i].indegree, j))
                sum++;
            if(binary_search(inedge.data()+graph[j].instart, inedge.data()+graph[j].instart+graph[j].indegree, i))
                sum++;
        }
    }
    return sum;
}

}
