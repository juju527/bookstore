#include<bits/stdc++.h>
using namespace std;
const int maxn=1e5+5;
string s[maxn];
mt19937 rnd(233);
vector<int> mp[maxn];
int main(){
    int n=100,m=10,W=1e9;
    printf("%d\n",n);
    for(int i=1;i<=m;i++){
        int len=5;
        for(int j=1;j<=len;j++)s[i]+=('a'+rnd()%26);
    }
    while(n--){
        int op=rnd()%3,id=rnd()%m+1,val=rnd()%W;
        if(!op)cout<<"insert "<<s[id]<<" "<<val<<endl,mp[id].push_back(val);
        else if(op==1)cout<<"delete "<<s[id]<<" "<<(mp[id].size()?mp[id][rnd()%mp[id].size()]:0)<<endl;
        else cout<<"find "<<s[id]<<endl;
    }
    return 0;
}