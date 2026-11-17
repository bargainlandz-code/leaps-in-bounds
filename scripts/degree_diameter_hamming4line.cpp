// Independent degree-diameter witness verifier for 4D/4-ary Hamming line matchings.
// Standard C++17 only. Reads a compact binary witness and reconstructs the
// complete graph. Performs an exact BFS from every vertex by default.
// Usage: ./hamming4line_checker witness.bin ORDER MAX_DEGREE MAX_DIAMETER [START COUNT]
// If START/COUNT are supplied, reports partial audit, NOT full verification.
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;
static void need(bool condition,const string& why){if(!condition)throw runtime_error(why);}
static uint32_t little32(const vector<uint8_t>& a,size_t i){return uint32_t(a.at(i))|(uint32_t(a.at(i+1))<<8)|(uint32_t(a.at(i+2))<<16)|(uint32_t(a.at(i+3))<<24);}
static uint32_t unpack(const vector<uint8_t>& a,size_t bitpos,int width){
    uint32_t v=0;
    for(int k=0;k<width;k++){
        size_t ix=16+(bitpos+k)/8;
        need(ix<a.size(),"truncated bit stream");
        v|=((a[ix]>>((bitpos+k)&7u))&1u)<<k;
    }
    return v;
}
static int line_vertices(int line,int t){
    // Each fibre is the Hamming graph H(4,4), with coordinates 0..3 in 4 axes.
    // Each axis contains 4^3=64 canonical lines of four vertices.
    const int f=line/256, local=line%256;
    const int axis=local/64, index=local%64;
    int coord=0,shift=0;
    for(int i=0;i<4;i++) if(i!=axis){coord|=((index>>shift)&3)<<(2*i);shift+=2;}
    return f*256+coord+(t<<(2*axis));
}
int main(int argc,char**argv){
 try{
    need(argc==5 || argc==7,"usage: checker FILE ORDER MAX_DEGREE MAX_DIAMETER [START COUNT]");
    const string path=argv[1];
    const long long claim=stoll(argv[2]), max_degree=stoll(argv[3]);
    const int diameter=stoi(argv[4]);
    need(claim>1 && claim<=3000000 && max_degree>=1 && max_degree<=1000 && diameter>=1 && diameter<=30,"bad claim");
    ifstream f(path,ios::binary);need(bool(f),"cannot open witness");
    vector<uint8_t> data((istreambuf_iterator<char>(f)),{});
    need(data.size()>=16,"header truncated");
    need(equal(data.begin(),data.begin()+8,"H4LINE01"),"wrong witness magic/version");
    uint32_t nf=little32(data,8), ncl=little32(data,12);
    need(nf>0 && nf<=4096 && ncl<=1000000,"bad fibre/clone counts");
    const int nb=int(nf)*256, nl=nb, n=nb+int(ncl);
    need(n==claim,"graph order differs from claim");
    int width=0;while((1u<<width)<(uint32_t)nl)width++;
    need(width>0 && width<=20,"bad index bit width");
    size_t nvalues=(size_t)nl+ncl, length=16+(nvalues*width+7)/8;
    need(data.size()==length,"wrong witness length");
    if(nvalues*width%8){
        unsigned unused=8-(nvalues*width%8);
        need((data.back()>>(8-unused))==0,"nonzero padding bits");
    }
    vector<int> mate(nl);
    size_t offset=0;
    for(int i=0;i<nl;i++,offset+=width){
        mate[i]=(int)unpack(data,offset,width);
        need(mate[i]>=0 && mate[i]<nl,"out-of-range matching partner");
    }
    for(int i=0;i<nl;i++){
        need(mate[i]!=i,"self-matched line");
        need(mate[mate[i]]==i,"non-reciprocal matching");
        need(i/256!=mate[i]/256,"matched lines within same fibre");
    }
    vector<int> sources(ncl);
    vector<uint8_t> sourceused(nb,0);
    for(unsigned j=0;j<ncl;j++,offset+=width){
        sources[j]=(int)unpack(data,offset,width);
        need(sources[j]>=0 && sources[j]<nb,"clone source out of range");
        need(sourceused[sources[j]]++==0,"duplicate clone source");
    }
    vector<vector<int>> adj(n);
    for(auto& row:adj)row.reserve(18);
    long long edges=0;
    for(int line=0;line<nl;line++){
        int partner=mate[line];if(line>=partner)continue;
        for(int a=0;a<4;a++){
            int u=line_vertices(line,a);
            for(int b=0;b<4;b++){
                int v=line_vertices(partner,b);
                need(u!=v,"loop in line pairing");
                adj[u].push_back(v);adj[v].push_back(u);edges++;
            }
        }
    }
    // Snapshot all source neighbourhoods before attaching any clone edges.
    vector<vector<int>> base_source_neighbours(ncl);
    for(unsigned j=0;j<ncl;j++){
        need(adj[sources[j]].size()==16,"base source has degree other than 16");
        base_source_neighbours[j]=adj[sources[j]];
    }
    for(unsigned j=0;j<ncl;j++){
        int newv=nb+(int)j;
        const vector<int>& old_neighbours=base_source_neighbours[j];
        for(int v:old_neighbours){
            need(v<nb,"clone neighbour is not an original vertex");
            adj[newv].push_back(v);adj[v].push_back(newv);edges++;
        }
    }
    int observed_max_degree=0,degree16=0,degree17=0;
    for(int v=0;v<n;v++){
        vector<int>& a=adj[v];
        sort(a.begin(),a.end());
        need(unique(a.begin(),a.end())==a.end(),"duplicate graph edge");
        need(a.size()<=(size_t)max_degree,"maximum degree exceeded");
        observed_max_degree=max(observed_max_degree,int(a.size()));
        degree16+=(a.size()==16);degree17+=(a.size()==17);
    }
    need(edges==(long long)ncl*16+(long long)nl/2*16,"wrong edge count");
    cerr<<"RECONSTRUCTION_PASS n="<<n<<" edges="<<edges<<" maxdeg="<<observed_max_degree
        <<" degree16="<<degree16<<" degree17="<<degree17<<" lines="<<nl<<" clones="<<ncl<<"\n";
    const int start=(argc==7)?stoi(argv[5]):0;
    const int count=(argc==7)?stoi(argv[6]):n;
    need(start>=0 && count>=1 && start+count<=n,"bad BFS root range");
    vector<uint32_t> marked(n,0);
    vector<int> que(n);
    uint32_t stamp=0;
    for(int r=start;r<start+count;r++){
        if(++stamp==0){fill(marked.begin(),marked.end(),0);stamp=1;}
        int head=0,tail=1;que[0]=r;marked[r]=stamp;
        for(int step=0;step<diameter && tail<n;step++){
            int limit=tail;
            while(head<limit){
                int u=que[head++];
                for(int v:adj[u])if(marked[v]!=stamp){marked[v]=stamp;que[tail++]=v;}
            }
        }
        if(tail<n){cerr<<"DIAMETER_FAIL root="<<r<<" missed="<<(n-tail)<<"\n";return 1;}
    }
    cout<<"PASS order="<<n<<" edges="<<edges<<" max_degree="<<observed_max_degree
        <<" diameter_at_most="<<diameter<<" roots="<<count
        <<" all_roots="<<(start==0 && count==n?"yes":"no")<<"\n";
    return 0;
 }catch(const exception& e){cerr<<"ERROR: "<<e.what()<<"\n";return 2;}
}
