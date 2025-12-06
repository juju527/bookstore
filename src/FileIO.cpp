#include<iostream>
#include<cstdio>
#include<fstream>
#include<string>
#include<cstring>
#include <filesystem>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;

struct Data{
    char str[64];
    int val;
    bool operator <(Data tmp)const{
        int n=strlen(str),m=strlen(tmp.str);
        for(int i=0;i<n&&i<m;i++)if(str[i]<tmp.str[i])return 1;else if(str[i]>tmp.str[i])return 0;
        if(n<m)return 1;else if(n>m)return 0;
        return val<tmp.val;
    }
    bool operator ==(Data tmp)const{
        int n=strlen(str),m=strlen(tmp.str);
        if(n!=m||val!=tmp.val)return 0;
        for(int i=0;i<n;i++)if(str[i]!=tmp.str[i])return 0;
        return 1;
    }
};
Data Mx;

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

    void initialise(string FN = "") {
        if (FN != "") file_name = FN;
        if(std::filesystem::exists("data"))return ;
        file.open(file_name, std::ios::out|std::ios::binary);
        int tmp = 0;
        for (int i = 0; i < info_len; ++i)
            file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        file.close();
        return ;
    }

    //读出第n个int的值赋给tmp，1_base
    void get_info(int &tmp, int n) {
        if (n > info_len)return ;
        file.open(file_name, std::ios::in|std::ios::out|std::ios::binary);
        file.seekg((n-1)*sizeof(int),std::ios::beg);
        file.read(reinterpret_cast<char*>(&tmp),sizeof(int));
        file.close();
        return ;
    }

    //将tmp写入第n个int的位置，1_base
    void write_info(int tmp, int n) {
        if (n > info_len)return ;
        file.open(file_name,std::ios::out|std::ios::in|std::ios::binary);
        file.seekp((n-1)*sizeof(int),std::ios::beg);
        file.write(reinterpret_cast<char*>(&tmp),sizeof(int));
        file.close();
        return ;
    }

    //在文件合适位置写入类对象t，并返回写入的位置索引index
    //位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
    //位置索引index可以取为对象写入的起始位置
    int write(T &t) {
        int head=0;
        get_info(head,1);
        file.open(file_name,std::ios::in|std::ios::out|std::ios::binary);
        int index=0;
        if(head){
            index=head,file.seekg(head,std::ios::beg);
            int nxt=0;file.read(reinterpret_cast<char *>(&nxt),sizeof(int));
            file.close();
            write_info(nxt,1);
            file.open(file_name,std::ios::in|std::ios::out|std::ios::binary);
            file.seekp(index,std::ios::beg);
        }
        else{
            file.seekp(0,std::ios::end);
            index=static_cast<int>(file.tellp());
        }
        file.write(reinterpret_cast<char*>(&t),sizeofT);
        file.close();
        return index;
    }

    //用t的值更新位置索引index对应的对象，保证调用的index都是由write函数产生
    void update(T &t, const int index) {
        file.open(file_name,std::ios::in|std::ios::out|std::ios::binary);
        file.seekp(index,std::ios::beg);
        file.write(reinterpret_cast<char*>(&t),sizeofT);
        file.close();
        return ;
    }

    //读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
    void read(T &t, const int index) {
        file.open(file_name,std::ios::in|std::ios::out|std::ios::binary);
        file.seekg(index,std::ios::beg);
        file.read(reinterpret_cast<char*>(&t),sizeofT);
        file.close();
        return ;
    }

    //删除位置索引index对应的对象(不涉及空间回收时，可忽略此函数)，保证调用的index都是由write函数产生
    void Delete(int index){
        if(index<=0)return ;
        int head=0;
        get_info(head,1);
        file.open(file_name,std::ios::in|std::ios::out|std::ios::binary);
        file.seekp(index,std::ios::beg),file.write(reinterpret_cast<char*>(&head),sizeof(int));
        file.close();
        write_info(index,1);
        return ;
    }

    int getnxt(int index){
        file.open(file_name,std::ios::in|std::ios::out|std::ios::binary);
        file.seekg(index,std::ios::beg);
        int nxt=0;
        file.read(reinterpret_cast<char*>(&nxt),sizeof(int));
        file.close();
        return nxt;
    }
    int getlen(int index){
        file.open(file_name,std::ios::in|std::ios::out|std::ios::binary);
        file.seekg(index+sizeof(int),std::ios::beg);
        int len=0;
        file.read(reinterpret_cast<char*>(&len),sizeof(int));
        file.close();
        return len;
    }
    Data getmx(int index){
        file.open(file_name,std::ios::in|std::ios::out|std::ios::binary);
        file.seekg(index+sizeof(int),std::ios::beg);
        int len=0;
        file.read(reinterpret_cast<char*>(&len),sizeof(int));
        if(!len){
            file.close();
            return Mx;
        }
        file.seekg(index+2*sizeof(int)+(len-1)*sizeof(Data),std::ios::beg);
        Data d;
        file.read(reinterpret_cast<char*>(&d),sizeof(Data));
        file.close();
        return d;
    }
};
const int B1=400,B2=900,maxb=1005;
struct node{
    int nxt,len;
    Data v[maxb];
    node(){nxt=len=0;}
    void push_back(Data c){v[len++]=c;return ;}
};
Data o;
int ok_split,ok_merge;
bool TAG;
class BlockList{
private:
    MemoryRiver<node> m;
public:
    void init(){
        if(std::filesystem::exists("data")){
            m.initialise("data");
            return ;
        }
        m.initialise("data");
        node aux;
        int index=m.write(aux);
        m.write_info(index,2);
        return ;
    }
    void split(int it){
        node cur,aux;
        m.read(cur,it);
        int len=cur.len,l=len/2;
        aux.nxt=cur.nxt;aux.len=len-l;
        for(int i=0;i<aux.len;i++)aux.v[i]=cur.v[i+l];
        int index=m.write(aux);
//        cerr<<index<<" "<<l<<" "<<aux.len<<endl;
        cur.nxt=index,cur.len=l;
        for(int i=l;i<len;i++)cur.v[i]=o;
        m.update(cur,it);
        ok_split++;
        return ;
    }
    void merge(int it,int nxt){
        node cur,aux;
        m.read(cur,it),m.read(aux,nxt);
        for(int i=0;i<aux.len;i++)cur.v[i+cur.len]=aux.v[i];
        cur.len+=aux.len;cur.nxt=aux.nxt;
        m.Delete(nxt);
        m.update(cur,it);
        ok_merge++;
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
        int len=aux.len,pos=-1;
        for(int i=0;i<len;i++)if(aux.v[i]<v)pos=i;
        for(int i=len-1;i>pos;i--)aux.v[i+1]=aux.v[i];
        aux.v[pos+1]=v;aux.len++;
        m.update(aux,it);
        chk(it);
        return ;
    }
    void ins(Data v){
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
    void del(Data v){
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
        return ;
    }
    bool find(int it,Data v){
        node cur;
        m.read(cur,it);
        bool tag=0,vis=0;
        for(int i=0;i<cur.len;i++){
            if(strlen(cur.v[i].str)!=strlen(v.str)){tag=0;continue;}
            int m=strlen(v.str);bool flag=0;
            for(int j=0;j<m;j++)if(cur.v[i].str[j]!=v.str[j]){flag=1;break;}
            if(flag){tag=0;continue;}
            cout<<cur.v[i].val<<" ";
            vis=1;tag=1;
        }
        if(!vis)return 0;
        if(!tag)return 1;
        int nxt=cur.nxt;
        if(!nxt)return 1;
        find(nxt,v);
        return 1;
    }
    void find(Data v){
        int it=0;
        m.get_info(it,2);
        bool flag=0;
        while(1){
            node cur;
            m.read(cur,it);
            int nxt=cur.nxt;
            if(!nxt){flag=find(it,v);break;}
            Data aux=cur.v[cur.len-1];
            if(aux<v){it=nxt;continue;}
            flag=find(it,v);break;
        }
        if(!flag)cout<<"null";
        cout<<endl;
        return ;
    }
    void out(){
        int it=0;
        m.get_info(it,2);
        while(it){
            int nxt=m.getnxt(it);
            node cur;
            m.read(cur,it);
            for(int i=0;i<cur.len;i++){
                for(int j=0;j<strlen(cur.v[i].str);j++)cout<<cur.v[i].str[j];
                cout<<" ";
                cout<<cur.v[i].val<<endl;
            }
            for(int i=0;i<strlen(cur.v[cur.len-1].str);i++)cerr<<cur.v[cur.len-1].str[i];
            cerr<<" ";cerr<<cur.v[cur.len-1].val<<endl;
            it=nxt;
        }
        return ;
    }
};
int main(){
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr),cout.tie(nullptr);
    for(int i=0;i<64;i++)Mx.str[i]=255;Mx.val=(1ll<<31)-1;
    BlockList B;B.init();
    int n;
    cin>>n;
    for(int i=1;i<=n;i++){
        string op;Data d;
        cin>>op>>d.str;
        if(op=="insert")cin>>d.val,B.ins(d);
        else if(op=="delete")cin>>d.val,B.del(d);
        else d.val=0,B.find(d);
    }
    cerr<<ok_split<<" "<<ok_merge<<endl;
    return 0;
}