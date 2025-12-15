#include "Command.hpp"
#include "Account.hpp"
#include "Book.hpp"
#include "Log.hpp"
string com;
bool read(){
    com="";char ch;
    while(std::cin.get(ch)){
        if(ch=='\r'||ch=='\n')return 1;
        com+=ch;
    }
    return false;
}
void format(){
    int len=com.size();
    int p1=len+1,p2=-1;
    for(int i=0;i<len;i++)if(com[i]!=' '){p1=i;break;}
    for(int i=len-1;i>=0;i--)if(com[i]!=' '){p2=i;break;}
    if(p1>p2)com="";else com=com.substr(p1,p2-p1+1);
    return ;
}
int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr), std::cout.tie(nullptr);
    Command command;
    command.init();
    while(1){
        bool tag=read();
        if(!com.size()){
            if(!tag)return 0;
            continue;
        }
        format();
        if(com=="quit"||com=="exit")return 0;
        else command.Run(com);
        if(!tag)return 0;
    }
    return 0;
}
