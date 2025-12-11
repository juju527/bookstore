#ifndef BPT_MEMORYRIVER_HPP
#define BPT_MEMORYRIVER_HPP

#include<fstream>
#include<iostream>
#include<filesystem>
#include<vector>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;
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

    void readorder(T &t,const int id){
        file.seekg(info_len*sizeof(int)+(id-1)*sizeofT);
        file.read(reinterpret_cast<char*>(&t),sizeofT);
        return ;
    }

    void writeorder(T t,const int id){
        int num=0;
        get_info(num,1);
        if(num>=id)file.seekp(info_len*sizeof(int)+(id-1)*sizeofT);
        else{
            write_info(num+1,1);
            file.seekp(0,std::ios::end),file.tellp();
        }
        file.write(reinterpret_cast<char*>(&t),sizeofT);
        return ;
    }

    vector<T> query(){
        int len;
        open();get_info(len,1);
        vector<T> ans(len);
        for(int i=0;i<len;i++)read(ans[i],info_len*sizeof(int)+i*sizeofT);
        close();
        return ans;
    }
};


#endif //BPT_MEMORYRIVER_HPP