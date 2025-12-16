#ifndef ACCOUNT_HPP
#define ACCOUNT_HPP
#include<vector>
#include"BlockList.hpp"
#include"String.hpp"
using std::pair;
using std::make_pair;
using std::cerr;
using std::endl;
class Account{
public:
    String30 UserID,Password,Username;
    int Privilege;
    Account(){Privilege=0;}
    Account(String30 id,String30 psd,String30 name,int pri){UserID=id,Password=psd,Username=name,Privilege=pri;}
    bool operator <(Account tmp)const{return UserID<tmp.UserID;}
    bool operator ==(Account tmp)const{return UserID==tmp.UserID;}
    friend std::ostream& operator <<(std::ostream &out,const Account &account){
        out<<account.UserID<<' '<<account.Privilege;
        return out;
    }
};
class AccountStorage{
private:
    BlockList<String30,Account,200,400,500> BL; // UserID -> Account class
    vector<pair<Account,int>> UserStack;
public:
    void initialize(){
        BL.init("Account");
        String30 UserID("root"),Password("sjtu");
        Account root(UserID,Password,UserID,7);
        auto res=BL.find(root.UserID);
        if(!res.size())BL.ins(root.UserID,root);
        UserStack.clear();
        return ;
    }
    ~AccountStorage() = default;
    void select(int id){
        if(!UserStack.size())return ;
        UserStack.back().second=id;
        return ;
    }
    Account getAccount(){
        if(!UserStack.size())return Account();
        return UserStack.back().first;
    }
    int getSelect(){
        if(!UserStack.size())return -1;
        return UserStack.back().second;
    }
    bool login(String30 UserID,String30 Password){
        auto res=BL.find(UserID);
        if(!res.size())return 0;
        if(Password.getlen()){
            if(!(res[0].Password==Password))return 0;
        }
        else{
            if(!UserStack.size())return 0;
            if(UserStack.back().first.Privilege<=res[0].Privilege)return 0;
        }
        UserStack.emplace_back(make_pair(res[0],-1));
        return 1;
    }
    bool logout(){
        if(!UserStack.size())return 0;
        UserStack.pop_back();
        return 1;
    }
    bool signup(String30 UserID,String30 Password,String30 Username){
        auto res=BL.find(UserID);
        if(res.size())return 0;
        Account acc(UserID,Password,Username,1);
        BL.ins(UserID,acc);
        return 1;
    }
    bool changePassword(String30 UserID,String30 CurrentPassword,String30 NewPassword){
        auto res=BL.find(UserID);
        if(!res.size())return 0;
        if(!CurrentPassword.getlen()){
            if(!UserStack.size()||UserStack.back().first.Privilege<7)return 0;
        }
        else{
            if(!(CurrentPassword==res[0].Password))return 0;
        }
        BL.del(UserID,res[0]),res[0].Password=NewPassword,BL.ins(UserID,res[0]);
        return 1;
    }
    bool useradd(String30 UserID,String30 Password,int Privilege,String30 Username){
        if(!UserStack.size()||UserStack.back().first.Privilege<=Privilege)return 0;
        auto res=BL.find(UserID);
        if(res.size())return 0;
        Account acc(UserID,Password,Username,Privilege);
        BL.ins(UserID,acc);
        return 1;
    }
    bool deleteUser(String30 UserID){
        auto res=BL.find(UserID);
        if(!res.size())return 0;
        int len=UserStack.size();
        for(int i=0;i<len;i++)if(UserStack[i].first.UserID==UserID)return 0;
        BL.del(UserID,res[0]);
        return 1;
    }
};
#endif