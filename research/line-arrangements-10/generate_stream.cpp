#include <bits/stdc++.h>
using namespace std;
using ll=long long;
constexpr int N=10;
struct Cross { int j; ll num,den; }; // x = num/den, positive den
int signll(ll x){return (x>0)-(x<0);}
string key(const array<int,N>& t){
    vector<array<int,N>> cov;
    cov.reserve(60);
    for(int i=0;i<N;i++) for(int j=i+1;j<N;j++){
        array<int,N> v;
        ll den=j-i;
        ll xn=(ll)t[i]-t[j];
        for(int k=0;k<N;k++){
            ll val=(ll)(i-k)*xn + (ll)(t[i]-t[k])*den;
            v[k]=signll(val);
        }
        if(v[i]!=0 || v[j]!=0) {cerr<<"bad intersection"; exit(1);}
        for(int k=0;k<N;k++) if(k!=i && k!=j && v[k]==0)return "";
        cov.push_back(v);
    }
    string best;
    for(int start=0;start<2*N;start++) for(int dir: {-1,1}){
        array<int,N> order,fac; fac.fill(0);int count=0;
        for(int step=0;step<2*N;step++){
            int pos=((start+dir*step)%(2*N)+(2*N))%(2*N);
            int i=pos%N;
            if(fac[i]==0){order[count++]=i;fac[i]=dir*(pos<N ? 1:-1);}
        }
        vector<uint16_t> codes; codes.reserve(cov.size());
        for(auto &v:cov){int code=0;for(int oi:order)code=3*code+ (v[oi]*fac[oi]+1); codes.push_back(code);}
        sort(codes.begin(),codes.end());
        string candidate;candidate.reserve(codes.size()*2);
        for(int v:codes){candidate+=(char)(v>>8);candidate+=(char)(v&255);}
        if(best.empty()||candidate<best)best=move(candidate);
    }
    return best;
}
int main(int argc, char **argv) {
    ios::sync_with_stdio(false); cin.tie(nullptr);
    int target=argc>1 ? atoi(argv[1]) : 1000001;
    mt19937_64 rng(20261118);
    unordered_set<string> seen; seen.reserve(target*2);
    int found=0;
    for(long long trials=0;trials<(long long)target*200 && found<target;trials++){
        array<int,N> t; t[0]=0;
        for(int i=1;i<N;i++) t[i]=(int)(rng()%2000001)-1000000;
        string k=key(t); if(k.empty())continue;
        if(seen.insert(move(k)).second){
            for(int i=0;i<N;i++) {
                if(i)cout<<' ';
                cout<<-i<<" 1 "<<t[i];
            }
            cout<<'\n';
            ++found;
            if(found%10000==0){cout.flush();cerr<<"found "<<found<<" trials "<<trials+1<<"\n";}
        }
    }
    cerr<<"RESULT "<<found<<"\n";
    return found==target?0:1;
}
