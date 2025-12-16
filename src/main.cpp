#include "Command.hpp"
#include "Account.hpp"
#include "Book.hpp"
#include "Log.hpp"
using std::cin;
string com;
void format(){
    int len=com.size();
    int p1=len+1,p2=-1;
    for(int i=0;i<len;i++)if(com[i]!=' '){p1=i;break;}
    for(int i=len-1;i>=0;i--)if(com[i]!=' '){p2=i;break;}
    if(p1>p2)com="";else com=com.substr(p1,p2-p1+1);
    len=com.size();
    string cur="",aux="";
    int cnt=0;
    for(int i=0;i<len;i++){
        if(com[i]==' '){
            if(!cnt)aux+=cur,cur="";
            cnt++;
        }
        else{
            if(cnt)aux+=" ",cnt=0;
            cur+=com[i];
        }
    }
    aux+=cur;
    com=cur;
    return ;
}
int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr), std::cout.tie(nullptr);
    Command command;
    command.init();
    while(getline(cin,com)){
        format();
        if(com=="quit"||com=="exit")return 0;
        else command.Run(com);
    }
    return 0;
}
