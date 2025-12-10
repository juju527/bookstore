#ifndef STRING_HPP
#define STRING_HPP
#include<array>
#include<string>
using std::array;
class String20{
private:
    int len;
    array<char,20> str;
public:
    String20(){len=0;}
    String20(std::string s){
        len=s.length();
        for(int i=0;i<len;i++)str[i]=s[i];
    }
    int getlen(){return len;}
    array<char,20> getstr(){return str;}
    bool operator <(String20 tmp)const{
        for(int i=0;i<len&&i<tmp.len;i++)if(str[i]<tmp.str[i])return 1;else if(str[i]>tmp.str[i])return 0;
        return len<tmp.len;
    }
    bool operator ==(String20 tmp)const{
        if(len!=tmp.len)return 0;
        for(int i=0;i<len;i++)if(str[i]!=tmp.str[i])return 0;
        return 1;
    }
    friend std::ostream& operator <<(std::ostream &out, const String20 &s){
        for(int i=0;i<s.len;i++)out<<s.str[i];
        return out;
    }
};
class String30{
private:
    int len;
    array<char,30> str;
public:
    String30(){len=0;}
    String30(std::string s){
        len=s.length();
        for(int i=0;i<len;i++)str[i]=s[i];
    }
    int getlen(){return len;}
    array<char,30> getstr(){return str;}
    bool operator <(String30 tmp)const{
        for(int i=0;i<len&&i<tmp.len;i++)if(str[i]<tmp.str[i])return 1;else if(str[i]>tmp.str[i])return 0;
        return len<tmp.len;
    }
    bool operator ==(String30 tmp)const{
        if(len!=tmp.len)return 0;
        for(int i=0;i<len;i++)if(str[i]!=tmp.str[i])return 0;
        return 1;
    }
    friend std::ostream& operator <<(std::ostream &out, const String30 &s){
        for(int i=0;i<s.len;i++)out<<s.str[i];
        return out;
    }
};
class String60{
private:
    int len;
    array<char,60> str;
public:
    String60(){len=0;}
    String60(std::string s){
        len=s.length();
        for(int i=0;i<len;i++)str[i]=s[i];
    }
    int getlen(){return len;}
    array<char,60> getstr(){return str;}
    bool operator <(String60 tmp)const{
        for(int i=0;i<len&&i<tmp.len;i++)if(str[i]<tmp.str[i])return 1;else if(str[i]>tmp.str[i])return 0;
        return len<tmp.len;
    }
    bool operator ==(String60 tmp)const{
        if(len!=tmp.len)return 0;
        for(int i=0;i<len;i++)if(str[i]!=tmp.str[i])return 0;
        return 1;
    }
    friend std::ostream& operator <<(std::ostream &out, const String60 &s){
        for(int i=0;i<s.len;i++)out<<s.str[i];
        return out;
    }
};
#endif