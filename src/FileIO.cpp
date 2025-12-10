#include<iostream>
#include<cstdio>
#include<fstream>
#include<string>
#include<cstring>
#include<filesystem>
#include<vector>
#include<array>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;

template<class T, int info_len = 2>
class MemoryRiver {
private:
    fstream file;
    string file_name;
    int sizeofT = sizeof(T);
    static_assert(sizeof(T) >= sizeof(int));
public:
    MemoryRiver() = default;

    MemoryRiver(const string& file_name) : file_name(file_name) {}
    void open(){file.open(file_name, std::ios::in|std::ios::out|std::ios::binary);return ;}
    void close(){file.close();return ;}
    void initialise(string FN = "") {
        if (FN != "") file_name = FN;
        if(std::filesystem::exists(file_name))return ;
        file.open(file_name,std::ios::out|std::ios::binary);
        int tmp = 0;
        for (int i = 0; i < info_len; ++i){
            file.seekp(i*sizeof(int),std::ios::beg);
            file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        }
        file.close();
        return ;
    }

    void get_info(int &tmp, int n) {
        if (n > info_len)return ;
        file.seekg((n-1)*sizeof(int),std::ios::beg);
        file.read(reinterpret_cast<char*>(&tmp),sizeof(int));
        return ;
    }

    void write_info(int tmp, int n) {
        if (n > info_len)return ;
        file.seekp((n-1)*sizeof(int),std::ios::beg);
        file.write(reinterpret_cast<char*>(&tmp),sizeof(int));
        return ;
    }

    int write(T &t) {
        int head=0;
        get_info(head,1);
        int index=0;
        if(head){
            index=head,file.seekg(head,std::ios::beg);
            int nxt=0;file.read(reinterpret_cast<char *>(&nxt),sizeof(int));
            write_info(nxt,1);
            file.seekp(index,std::ios::beg);
        }
        else{
            file.seekp(0,std::ios::end);
            index=static_cast<int>(file.tellp());
        }
        file.write(reinterpret_cast<char*>(&t),sizeofT);
        return index;
    }

    void update(T &t, const int index) {
        file.seekp(index,std::ios::beg);
        file.write(reinterpret_cast<char*>(&t),sizeofT);
        return ;
    }

    void read(T &t, const int index) {
        file.seekg(index,std::ios::beg);
        file.read(reinterpret_cast<char*>(&t),sizeofT);
        return ;
    }

    void Delete(int index){
        if(index<=0)return ;
        int head=0;
        get_info(head,1);
        file.seekp(index,std::ios::beg),file.write(reinterpret_cast<char*>(&head),sizeof(int));
        write_info(index,1);
        return ;
    }
};
template<class Key,class Val,int B1,int B2,int maxb>
class BlockList{
private:
    string file_name;
public:
    struct Data{
        Key key;Val val;
        Data(){}
        bool operator ==(Data tmp)const{return (key==tmp.key)&&(val==tmp.val);}
        bool operator <(Data tmp)const{
            if(key<tmp.key)return 1;
            if(tmp.key<key)return 0;
            return (val<tmp.val);
        }
    };
    struct node{
        int nxt,len;
        Data v[maxb];
        node(){
            nxt=len=0;
        }
    };
private:
    MemoryRiver<node> m;
public:
    void init(string FN){
        file_name=FN;
        if(std::filesystem::exists(file_name)){m.initialise(file_name);return ;}
        m.initialise(file_name);
        m.open();
        node aux;
        int index=m.write(aux);
        m.write_info(index,2);
        m.close();
        return ;
    }
    void split(int it){
        node cur,aux;
        m.read(cur,it);
        int len=cur.len,l=len/2;
        aux.nxt=cur.nxt;aux.len=len-l;
        for(int i=0;i<aux.len;i++)aux.v[i]=cur.v[i+l];
        int index=m.write(aux);
        cur.nxt=index,cur.len=l;
        m.update(cur,it);
        return ;
    }
    void merge(int it,int nxt){
        node cur,aux;
        m.read(cur,it),m.read(aux,nxt);
        for(int i=0;i<aux.len;i++)cur.v[i+cur.len]=aux.v[i];
        cur.len+=aux.len;cur.nxt=aux.nxt;
        m.Delete(nxt);
        m.update(cur,it);
        return ;
    }
    void chk(int it){
        node aux;
        m.read(aux,it);
        int nxt=aux.nxt,l1=aux.len,l2=0;
        if(l1>B2){split(it);return ;}
        if(nxt){
            m.read(aux,nxt);l2=aux.len;
            if(l1<B1&&l1+l2<B2)merge(it,nxt);
        }
        return ;
    }
    void ins(int it,Data v){
        node aux;
        m.read(aux,it);
        int len=aux.len;
        auto pos=std::lower_bound(aux.v,aux.v+len,v);
        int idx=pos-aux.v;
        std::copy_backward(pos,aux.v+len,aux.v+len+1);
        aux.v[idx]=v;aux.len++;
        m.update(aux,it);
        chk(it);
        return ;
    }
    void ins(Key key,Val val){
        m.open();
        Data v;v.key=key,v.val=val;
        int it=0;
        m.get_info(it,2);
        while(1){
            node cur;
            m.read(cur,it);
            int nxt=cur.nxt;
            if(!nxt){ins(it,v);break;}
            Data aux=cur.v[cur.len-1];
            if(aux<v){it=nxt;continue;}
            ins(it,v);break;
        }
        m.close();
        return ;
    }
    void del(int it,Data v){
        node aux;
        m.read(aux,it);
        int len=aux.len,pos=-1;
        for(int i=0;i<len;i++)if(aux.v[i]==v)pos=i;
        if(pos>=0){
            for(int i=pos;i<len-1;i++)aux.v[i]=aux.v[i+1];
            aux.len--;
            m.update(aux,it);
            chk(it);
        }
        return ;
    }
    void del(Key key,Val val){
        m.open();
        Data v;v.key=key,v.val=val;
        int it=0;
        m.get_info(it,2);
        while(1){
            node cur;
            m.read(cur,it);
            int nxt=cur.nxt;
            if(!nxt){del(it,v);break;}
            Data aux=cur.v[cur.len-1];
            if(aux<v){it=nxt;continue;}
            del(it,v);break;
        }
        m.close();
        return ;
    }
    vector<Val> ans;
    void find(int it,Key key){
        node cur;
        while(it){
            m.read(cur,it);
            bool tag=0,vis=0;
            for(int i=0;i<cur.len;i++){
                if(!(cur.v[i].key==key)){tag=0;continue;}
                ans.emplace_back(cur.v[i].val);
                vis=1;tag=1;
            }
            if(!vis||!tag)return ;
            it=cur.nxt;
        }
        return ;
    }
    std::vector<Val> find(Key key){
        m.open();
        int it=0;
        m.get_info(it,2);
        ans.clear();
        while(1){
            node cur;
            m.read(cur,it);
            int nxt=cur.nxt;
            if(!nxt){find(it,key);break;}
            Key aux=cur.v[cur.len-1].key;
            if(aux<key){it=nxt;continue;}
            find(it,key);break;
        }
        m.close();
        return ans;
    }
};
class String64{
private:
    int len;
    std::array<char,64> str;
public:
    String64(){len=0;}
    String64(std::string s){
        len=s.length();
        for(int i=0;i<len;i++)str[i]=s[i];
    }
    bool operator <(String64 tmp)const{
        for(int i=0;i<len&&i<tmp.len;i++)if(str[i]<tmp.str[i])return 1;else if(str[i]>tmp.str[i])return 0;
        return len<tmp.len;
    }
    bool operator ==(String64 tmp)const{
        if(len!=tmp.len)return 0;
        for(int i=0;i<len;i++)if(str[i]!=tmp.str[i])return 0;
        return 1;
    }
};
int main(){
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr),cout.tie(nullptr);
    BlockList<String64,int,450,1000,1100> B;B.init("data");
    int n;
    cin>>n;
    for(int i=1;i<=n;i++){
        string op,str;
        int val;
        cin>>op>>str;
        String64 s(str);
        if(op=="insert")cin>>val,B.ins(s,val);
        else if(op=="delete")cin>>val,B.del(s,val);
        else{
            vector<int> ans=B.find(s);
            for(auto v:ans)cout<<v<<" ";
            if(!ans.size())cout<<"null";
            cout<<endl;
        }
    }
    return 0;
}