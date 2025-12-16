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
    FinanceInfo(){}
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
    LogInfo(){}
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
    EmployeeLog(){}
    EmployeeLog(int t,LogInfo l){tim=t,log=l;}
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
        employees.initialise("Empolyee");
        employees.open();
        int num=0;
        employees.get_info(num,1);
        if(!num){
            employees.write_info(1,1);
            String30 str("root");
            employees.writeorder(str,1);
        }
        employees.close();
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

        return ;
    }
    void addLog(String30,int,array<char,operlen>);
    void showFinance(){
        financeStorage.open();
        int num=0;
        financeStorage.get_info(num,1);
        //cerr<<"!"<<num<<endl;
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
        //cerr<<num<<endl;
        if(Count>num){financeStorage.close();return 0;}
        Finance R,L;
        financeStorage.readorder(R,num);
        if(num-Count)financeStorage.readorder(L,num-Count);
        cout<<(R-L)<<endl;
        financeStorage.close();
        return 1;
    }
    void showFinanceInfo();
    void showEmployeeLog();
    void showLog();
};
#endif