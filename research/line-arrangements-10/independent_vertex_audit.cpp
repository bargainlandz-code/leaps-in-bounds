// Independent audit: exact determinant geometry and canonical vertex-only invariant.
// Distinct canonical vertex invariants certify distinct arrangements.
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>
using namespace std;
constexpr int N=10;
using Vector=array<signed char,N>;
static int sgn(long long n){return (n>0)-(n<0);}
int main(){
  ios::sync_with_stdio(false);cin.tie(nullptr);
  unordered_set<string> distinct;distinct.reserve(1200000);
  long long rows=0;array<long long,N> t;int a,b,c;
  while(cin>>a){
    if(a!=0 || !(cin>>b>>c) || b!=1 || c!=0){cerr<<"wrong first line at "<<rows+1<<"\n";return 2;}
    t[0]=0;
    for(int i=1;i<N;i++){
      int ai,bi,ci;if(!(cin>>ai>>bi>>ci) || ai!=-i || bi!=1){cerr<<"invalid line at "<<rows+1<<"\n";return 3;}
      t[i]=ci;
    }
    vector<Vector> vertices;vertices.reserve(45);
    for(int i=0;i<N;i++)for(int j=i+1;j<N;j++){
      Vector signs{};
      auto numerator=t[i]-t[j];
      auto denominator=j-i;
      for(int k=0;k<N;k++) {
        long long determinant=(i-k)*numerator+(t[i]-t[k])*denominator;
        signs[k]=(signed char)sgn(determinant);
        if(k!=i && k!=j && signs[k]==0){cerr<<"triple concurrency at row "<<rows+1<<" lines "<<i<<","<<j<<","<<k<<"\n";return 4;}
      }
      if(signs[i]!=0 || signs[j]!=0){cerr<<"wrong vertex signs\n";return 5;}
      vertices.push_back(signs);
    }
    string best;
    for(int start=0;start<2*N;start++)for(int dir: {-1,1}){
      array<int,N> order{};array<int,N> fac{};int used=0;
      for(int step=0;step<2*N;step++){
        int pos=(start+dir*step+40*N)%(2*N),i=pos%N;
        if(!fac[i]){fac[i]=dir*(pos<N?1:-1);order[used++]=i;}
      }
      if(used!=N){cerr<<"canonical bug\n";return 6;}
      array<uint16_t,45> codes{};int idx=0;
      for(const auto& v:vertices){
        unsigned code=0;
        for(int k=0;k<N;k++)code=3*code+(v[order[k]]*fac[order[k]]+1);
        codes[idx++]=(uint16_t)code;
      }
      sort(codes.begin(),codes.end());
      string sig;sig.reserve(90);
      for(auto code:codes){sig.push_back(char(code>>8));sig.push_back(char(code&255));}
      if(best.empty()||sig<best)best=move(sig);
    }
    if(!distinct.insert(best).second){cerr<<"DUPLICATE VERTEX-ONLY CANONICAL at row "<<rows+1<<"\n";return 7;}
    ++rows;
    if(rows%20000==0)cerr<<"INDEPENDENT_AUDIT "<<rows<<" distinct vertices-canonical signatures\n";
  }
  if(rows!=1000001){cerr<<"BAD ROW COUNT "<<rows<<"\n";return 8;}
  cout<<"PASS: "<<rows<<" arrangements, each 10 nonparallel lines with 45 ordinary intersections, "
      <<distinct.size()<<" pairwise distinct canonical vertex invariants (40 infinity symmetries)\n";
}
