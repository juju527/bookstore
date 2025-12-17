#ifndef BOOK_HPP
#define BOOK_HPP
#include"String.hpp"
#include<vector>
#include<algorithm>

using std::vector;
using std::cout;
using std::endl;
using std::fixed;
using std::setprecision;

class Book{
public:
    int Bookid;
    String20 ISBN;
    String60 BookName,Author,Keyword;
    int number;
    double price;
    Book(){Bookid=number=price=0;}
    Book(int id,String20 isbn,String60 bookname,String60 author,String60 keyword,int num,double pri){
        Bookid=id,ISBN=isbn,BookName=bookname,Author=author,Keyword=keyword,number=num,price=pri;
    }
    bool operator <(Book tmp)const{return ISBN<tmp.ISBN;}
    bool operator ==(Book tmp)const{return ISBN==tmp.ISBN;}
    friend std::ostream& operator <<(std::ostream &out, const Book &book){
        out<<book.ISBN;out<<'\t';
        out<<book.BookName;out<<'\t';
        out<<book.Author;out<<'\t';
        out<<book.Keyword;out<<'\t';
        out<<fixed<<setprecision(2)<<book.price;out<<'\t';
        out<<book.number;
        return out;
    }
};
class BookStorage{
private:
    MemoryRiver<Book,1> books;
    BlockList<String20,int,200,500,505> BL1; // ISBN -> Bookid
    BlockList<String60,int,200,500,505> BL2; // BookName -> Bookid
    BlockList<String60,int,200,500,505> BL3; // Author -> Bookid
    BlockList<String60,int,400,1000,1005> BL4; // Keyword -> Bookid
    bool selected;
    int current;
public:
    void initialize(){
        books.initialise("Books");
        BL1.init("ISBN");
        BL2.init("BookName");
        BL3.init("Author");
        BL4.init("Keyword");
        selected=0,current=-1;
        return ;
    }
    ~BookStorage() = default;
    void query(){
        vector<Book> res=books.query();
        sort(res.begin(),res.end());
        for(auto b:res)cout<<b<<endl;
        if(!res.size())cout<<endl;
        return ;
    }
    String20 querycur(){
        books.open();
        Book ans;
        books.readorder(ans,current);
        books.close();
        return ans.ISBN;
    }
    void queryISBN(String20 ISBN){
        vector<int> res=BL1.find(ISBN);
        for(auto id:res){
            Book ans;
            books.open();
            books.readorder(ans,id);
            cout<<ans<<endl;
            books.close();
        }
        if(!res.size())cout<<endl;
        return ;
    }
    void queryBookName(String60 BookName){
        vector<int> res=BL2.find(BookName);
        vector<Book> ans;
        books.open();
        for(auto id:res){
            Book aux;
            books.readorder(aux,id);
            ans.emplace_back(aux);
        }
        books.close();
        sort(ans.begin(),ans.end());
        for(auto b:ans)cout<<b<<endl;
        if(!res.size())cout<<endl;
        return ;
    }
    void queryAuthor(String60 Author){
        vector<int> res=BL3.find(Author);
        vector<Book> ans;
        books.open();
        for(auto id:res){
            Book aux;
            books.readorder(aux,id);
            ans.emplace_back(aux);
        }
        books.close();
        sort(ans.begin(),ans.end());
        for(auto b:ans)cout<<b<<endl;
        if(!res.size())cout<<endl;
        return ;
    }
    bool queryKeyword(String60 Keyword){
        int len=Keyword.getlen();
        std::array<char,60> str=Keyword.getstr();
        for(int i=0;i<len;i++)if(str[i]=='|')return 0;
        vector<int> res=BL4.find(Keyword);
        vector<Book> ans;
        books.open();
        for(auto id:res){
            Book aux;
            books.readorder(aux,id);
            ans.emplace_back(aux);
        }
        books.close();
        sort(ans.begin(),ans.end());
        for(auto b:ans)cout<<b<<endl;
        if(!res.size())cout<<endl;
        return 1;
    }
    double buy(String20 ISBN,int quantity){
        if(quantity<=0)return -1;
        vector<int> res=BL1.find(ISBN);
        if(!res.size())return -1;
        Book cur;
        books.open();
        books.readorder(cur,res[0]);
        if(cur.number<quantity){books.close();return -1;}
        cout<<fixed<<setprecision(2)<<cur.price*quantity<<endl;
        cur.number-=quantity;books.writeorder(cur,res[0]);
        books.close();
        return cur.price*quantity;
    }
    void select(int id){
        if(id==-1)selected=0;else selected=1;
        current=id;
        return ;
    }
    int select(String20 ISBN){
        if(!ISBN.getlen())return -1;
        vector<int> res=BL1.find(ISBN);
        if(!res.size()){
            books.open();
            int num=0;
            books.get_info(num,1);num++;
            books.write_info(num,1);
            Book cur;
            cur.Bookid=num;cur.ISBN=ISBN;
            books.writeorder(cur,num);books.close();
            BL1.ins(ISBN,num);BL2.ins(String60(),num);
            BL3.ins(String60(),num),BL4.ins(String60(),num);
            res.emplace_back(num);
        }
        select(res[0]);
        return res[0];
    }
    bool modifyISBN(String20 ISBN){
        if(!selected)return 0;
        Book cur;
        books.open(),books.readorder(cur,current),books.close();
        if(cur.ISBN==ISBN)return 0;
        vector<int> res=BL1.find(ISBN);
        if(res.size())return 0;
        BL1.del(cur.ISBN,current);cur.ISBN=ISBN;BL1.ins(cur.ISBN,current);
        books.open(),books.writeorder(cur,current),books.close();
        return 1;
    }
    bool modifyBookName(String60 BookName){
        if(!selected)return 0;
        Book cur;
        books.open(),books.readorder(cur,current),books.close();
        BL2.del(cur.BookName,current),cur.BookName=BookName,BL2.ins(cur.BookName,current);
        books.open(),books.writeorder(cur,current),books.close();
        return 1;
    }
    bool modifyAuthor(String60 Author){
        if(!selected)return 0;
        Book cur;
        books.open(),books.readorder(cur,current),books.close();
        BL3.del(cur.Author,current),cur.Author=Author,BL3.ins(cur.Author,current);
        books.open(),books.writeorder(cur,current),books.close();
        return 1;
    }
    bool modifyKeyword(String60 Keyword){
        if(!selected)return 0;
        Book cur;
        books.open(),books.readorder(cur,current),books.close();
        int len=cur.Keyword.getlen();
        auto str=cur.Keyword.getstr();
        string aux="";
        for(int i=0;i<len;i++){
            if(str[i]!='|')aux+=str[i];
            else BL4.del(aux,current),aux="";
        }
        BL4.del(aux,current),aux="";
        cur.Keyword=Keyword;
        len=Keyword.getlen(),str=Keyword.getstr();
        for(int i=0;i<len;i++){
            if(str[i]!='|')aux+=str[i];
            else BL4.ins(aux,current),aux="";
        }
        BL4.ins(aux,current),aux="";
        books.open();books.writeorder(cur,current),books.close();
        return 1;
    }
    bool modifyPrice(double price){
        if(!selected)return 0;
        Book cur;
        books.open(),books.readorder(cur,current);
        cur.price=price,books.writeorder(cur,current);
        books.close();
        return 1;
    }
    bool import(int quantity){
        if(!selected)return 0;
        Book cur;
        books.open(),books.readorder(cur,current);
        cur.number+=quantity;books.writeorder(cur,current);
        books.close();
        return 1;
    }
};
#endif