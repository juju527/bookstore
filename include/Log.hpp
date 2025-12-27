#ifndef LOG_HPP
#define LOG_HPP

#include<vector>
#include<array>
#include"String.hpp"
#include"MemoryRiver.hpp"
#include"Account.hpp"
#include"Book.hpp"

using std::vector;
using std::array;

const int operlen=500;

class Finance{
public:
    double income,expend;
    Finance(){income=expend=0;}
    Finance(double inc,double exp){income=inc,expend=exp;}
    Finance operator -(Finance tmp)const{return (Finance){income-tmp.income,expend-tmp.expend};}
    friend std::ostream& operator <<(std::ostream &out,const Finance &f){
        out<<"+ "<<fixed<<setprecision(2)<<f.income<<" - "<<fixed<<setprecision(2)<<f.expend;
        return out;
    }
};
class FinanceInfo{
public:
    String30 UserID;
    int len;
    array<char,operlen> Operation;
    Finance finance;
    FinanceInfo(){len=0;}
    FinanceInfo(String30 userid,int l,array<char,operlen> oper,Finance fin){UserID=userid,len=l,Operation=oper,finance=fin;}
    friend std::ostream& operator <<(std::ostream &out,const FinanceInfo &f){
        out<<f.UserID<<" ";
        for(int i=0;i<f.len;i++)out<<f.Operation[i];
        out<<" "<<f.finance;
        return out;
    }
};
class LogInfo{
public:
    String30 UserID;
    int len;
    array<char,operlen> Operation;
    LogInfo(){len=0;}
    LogInfo(String30 userid,int l,array<char,operlen> oper){UserID=userid,len=l,Operation=oper;}
    friend std::ostream& operator <<(std::ostream &out, const LogInfo &log){
        out<<log.UserID<<" ";
        for(int i=0;i<log.len;i++)out<<log.Operation[i];
        return out;
    }
};
class EmployeeLog {
public:
    int tim;
    LogInfo log;
    EmployeeLog(){tim=0;}
    EmployeeLog(int t,LogInfo l){tim=t,log=l;}
    bool operator <(EmployeeLog tmp)const{return tim<tmp.tim;}
    friend std::ostream& operator <<(std::ostream &out, const EmployeeLog &emp){
        out<<emp.tim<<" "<<emp.log;
        return out;
    }
};
class LogInfoStorage{
private:
    MemoryRiver<Finance,1> financeStorage;
    MemoryRiver<FinanceInfo,1> financeInfoStorage;
    MemoryRiver<LogInfo,1> logStorage;
    MemoryRiver<String30,2> employees;// UserID
    BlockList<String30,EmployeeLog,200,500,505> BL;
public:
    void initialize(){
        financeStorage.initialise("Finance");
        financeInfoStorage.initialise("FinanceInfo");
        logStorage.initialise("Log");
        employees.initialise("Employee");
        employees.open();
        int num=0;
        employees.get_info(num,1);
        if(!num){
            employees.write_info(1,1);
            String30 str("root");
            employees.writeorder(str,1);
        }
        employees.close();
        BL.init("EmployLog");
        return ;
    }
    void addFinance(double inc,double exp,String30 userid,int len,array<char,operlen> oper){
        financeStorage.open();
        int num=0;
        financeStorage.get_info(num,1);
        Finance lst;
        if(num)financeStorage.readorder(lst,num);
        lst.income+=inc,lst.expend+=exp;
        financeStorage.writeorder(lst,num+1);
        financeStorage.close();
        FinanceInfo aux(userid,len,oper,lst);
        financeInfoStorage.open();
        financeInfoStorage.writeorder(aux,num+1);
        financeInfoStorage.close();
        return ;
    }
    void addEmployee(String30 userid){
        employees.open();
        int num=0;
        employees.get_info(num,1);
        employees.writeorder(userid,num+1);
        employees.close();
        return ;
    }
    void addEmployeeLog(String30 userid,int len,array<char,operlen> oper){
        int tim=0;
        employees.open();
        employees.get_info(tim,2);tim++;
        employees.write_info(tim,2);
        employees.close();
        LogInfo aux(userid,len,oper);
        EmployeeLog info(tim,aux);
        BL.ins(userid,info);
        auto v=BL.find(userid);
        return ;
    }
    void addLog(String30 userid,int len,array<char,operlen> oper){
        int pos=0;
        logStorage.open();
        logStorage.get_info(pos,1);
        LogInfo info(userid,len,oper);
        logStorage.writeorder(info,pos+1);
        logStorage.close();
        return ;
    }
    void showFinance(){
        financeStorage.open();
        int num=0;
        financeStorage.get_info(num,1);
        Finance lst;
        if(num)financeStorage.readorder(lst,num);
        cout<<lst<<endl;
        financeStorage.close();
        return ;
    }
    bool showFinance(int Count){
        if(!Count){cout<<endl;return 1;}
        financeStorage.open();
        int num=0;
        financeStorage.get_info(num,1);
        if(Count>num){financeStorage.close();return 0;}
        Finance R,L;
        financeStorage.readorder(R,num);
        if(num-Count)financeStorage.readorder(L,num-Count);
        cout<<(R-L)<<endl;
        financeStorage.close();
        return 1;
    }
    void showFinanceInfo(){
        cout<<"Here is the finance report of the bookstore:"<<endl;
        int len=0;
        financeInfoStorage.open();
        financeInfoStorage.get_info(len,1);
        for(int i=1;i<=len;i++){
            FinanceInfo info;
            financeInfoStorage.readorder(info,i);
            cout<<info<<endl;
        }
        cout<<"End of the finance report."<<endl;
        financeInfoStorage.close();
        return ;
    }
    void showEmployeeLog(){
        cout<<"Here is the employee report of the bookstore:"<<endl;
        int num=0;
        employees.open();
        employees.get_info(num,1);
        for(int i=1;i<=num;i++){
            String30 userid;
            employees.readorder(userid,i);
            cout<<"The log of "<<userid<<":"<<endl;
            auto v=BL.find(userid);
            for(auto info:v)cout<<info<<endl;
            if(!v.size())cout<<"no operation"<<endl;
        }
        cout<<"End of the employee report."<<endl;
        employees.close();
        return ;
    }
    void showLog(){
        cout<<"Here is the log of the bookstore:"<<endl;
        int len=0;
        logStorage.open();
        logStorage.get_info(len,1);
        for(int i=1;i<=len;i++){
            LogInfo info;
            logStorage.readorder(info,i);
            cout<<info<<endl;
        }
        cout<<"End of the log."<<endl;
        logStorage.close();
        return ;
    }
};
#endif