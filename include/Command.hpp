#ifndef COMMAND_HPP
#define COMMAND_HPP
#include "Account.hpp"
#include "Book.hpp"
#include "Log.hpp"
#include <sstream>
using std::cerr;
using std::stringstream;
class Command {
private:
  AccountStorage account;
  BookStorage book;
  LogInfoStorage log;

public:
  void init() {
    account.initialize();
    book.initialize();
    log.initialize();
    return;
  }
  void Invalid() {
    cout << "Invalid" << endl;
    return;
  }
  void Run(string s) {
    if (!s.size())
      return;
    // cerr<<s<<endl;
    if (s.size() >= 12 && s.substr(0, 12) == "show finance")
      Runshowfinance(s);
    else if (s.size() >= 6 && s.substr(0, 6) == "logout")
      Runlogout(s);
    else if (s.size() >= 3 && s.substr(0, 3) == "log")
      Runlog(s);
    else if (s.size() >= 6 && s.substr(0, 6) == "report")
      Runreport(s);
    else if (s.size() >= 2 && s.substr(0, 2) == "su")
      Runsu(s);
    else if (s.size() >= 8 && s.substr(0, 8) == "register")
      Runregister(s);
    else if (s.size() >= 6 && s.substr(0, 6) == "passwd")
      Runpasswd(s);
    else if (s.size() >= 7 && s.substr(0, 7) == "useradd")
      Runuseradd(s);
    else if (s.size() >= 6 && s.substr(0, 6) == "delete")
      Rundelete(s);
    else if (s.size() >= 4 && s.substr(0, 4) == "show")
      Runshow(s);
    else if (s.size() >= 3 && s.substr(0, 3) == "buy")
      Runbuy(s);
    else if (s.size() >= 6 && s.substr(0, 6) == "select")
      Runselect(s);
    else if (s.size() >= 6 && s.substr(0, 6) == "modify")
      Runmodify(s);
    else if (s.size() >= 6 && s.substr(0, 6) == "import")
      Runimport(s);
    else
      Invalid();
    return;
  }
  bool chk1(string s) { // UserID, Password
    if (s.size() > 30)
      return 0;
    int len = s.size();
    for (int i = 0; i < len; i++)
      if (!((s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') ||
            (s[i] >= 'A' && s[i] <= 'Z') || s[i] == '_'))
        return 0;
    return 1;
  }
  bool chk2(string s) { // Username
    if (s.size() > 30)
      return 0;
    int len = s.size();
    for (int i = 0; i < len; i++)
      if (s[i] < 32 || s[i] > 126)
        return 0;
    return 1;
  }
  bool chk3(string s) { // Privilege
    if (s.size() > 1)
      return 0;
    int len = s.size();
    for (int i = 0; i < len; i++)
      if (!(s[i] == '0' || s[i] == '1' || s[i] == '3' || s[i] == '7'))
        return 0;
    return 1;
  }

  void LOG(string S, String30 UserID) {
    int len = S.size();
    array<char, operlen> oper;
    for (int i = 0; i < len; i++)
      oper[i] = S[i];
    log.addLog(UserID, len, oper);
    if (account.getAccount().Privilege >= 3)
      log.addEmployeeLog(UserID, len, oper);
    return;
  }

  void Runsu(string s) {
    string S = s;
    stringstream ss;
    ss << s;
    getline(ss, s, ' ');
    if (s != "su") {
      Invalid();
      return;
    }
    string UserID, Password = "";
    if (!getline(ss, UserID, ' ')) {
      Invalid();
      return;
    }
    if (!getline(ss, Password, ' '))
      Password = "";
    if (getline(ss, s, ' ')) {
      Invalid();
      return;
    }
    if (!chk1(UserID) || !chk1(Password)) {
      Invalid();
      return;
    }
    if (!account.login(UserID, Password)) {
      Invalid();
      return;
    }
    book.select(-1);
    LOG(S, UserID);
    return;
  }
  void Runlogout(string s) {
    if (account.getAccount().Privilege < 1) {
      Invalid();
      return;
    }
    string S = s;
    stringstream ss;
    ss << s;
    getline(ss, s, ' ');
    if (s != "logout") {
      Invalid();
      return;
    }
    if (getline(ss, s, ' ')) {
      Invalid();
      return;
    }
    String30 UserID = account.getAccount().UserID;
    if (!account.logout()) {
      Invalid();
      return;
    }
    book.select(account.getSelect());
    LOG(S, UserID);
    return;
  }
  void Runregister(string s) {
    string S = s;
    stringstream ss;
    ss << s;
    getline(ss, s, ' ');
    if (s != "register") {
      Invalid();
      return;
    }
    string UserID, Password, Username;
    if (!getline(ss, UserID, ' ')) {
      Invalid();
      return;
    }
    if (!getline(ss, Password, ' ')) {
      Invalid();
      return;
    }
    if (!getline(ss, Username, ' ')) {
      Invalid();
      return;
    }
    if (getline(ss, s, ' ')) {
      Invalid();
      return;
    }
    if (!chk1(UserID) || !chk1(Password) || !chk2(Username)) {
      Invalid();
      return;
    }
    if (!account.signup(UserID, Password, Username)) {
      Invalid();
      return;
    }
    LOG(S, account.getAccount().UserID);
    return;
  }
  void Runpasswd(string s) {
    if (account.getAccount().Privilege < 1) {
      Invalid();
      return;
    }
    string S = s;
    stringstream ss;
    ss << s;
    getline(ss, s, ' ');
    if (s != "passwd") {
      Invalid();
      return;
    }
    string UserID, CurrentPassword, NowPassword;
    if (!getline(ss, UserID, ' ')) {
      Invalid();
      return;
    }
    if (!getline(ss, CurrentPassword, ' ')) {
      Invalid();
      return;
    }
    if (!getline(ss, NowPassword, ' ')) {
      NowPassword = CurrentPassword, CurrentPassword = "";
      if (account.getAccount().Privilege < 7) {
        Invalid();
        return;
      }
    }
    if (getline(ss, s, ' ')) {
      Invalid();
      return;
    }
    if (!chk1(UserID) || !chk1(CurrentPassword) || !chk1(NowPassword)) {
      Invalid();
      return;
    }
    if (!account.changePassword(UserID, CurrentPassword, NowPassword)) {
      Invalid();
      return;
    }
    LOG(S, account.getAccount().UserID);
    return;
  }
  void Runuseradd(string s) {
    if (account.getAccount().Privilege < 3) {
      Invalid();
      return;
    }
    string S = s;
    stringstream ss;
    ss << s;
    getline(ss, s, ' ');
    if (s != "useradd") {
      Invalid();
      return;
    }
    string UserID, Password, Privilege, Username;
    if (!getline(ss, UserID, ' ')) {
      Invalid();
      return;
    }
    if (!getline(ss, Password, ' ')) {
      Invalid();
      return;
    }
    if (!getline(ss, Privilege, ' ')) {
      Invalid();
      return;
    }
    if (!getline(ss, Username, ' ')) {
      Invalid();
      return;
    }
    if (getline(ss, s, ' ')) {
      Invalid();
      return;
    }
    if (!chk1(UserID) || !chk1(Password) || !chk3(Privilege) ||
        !chk2(Username)) {
      Invalid();
      return;
    }
    if (!account.useradd(UserID, Password, Privilege[0] - '0', Username)) {
      Invalid();
      return;
    }
    LOG(S, account.getAccount().UserID);
    return;
  }
  void Rundelete(string s) {
    if (account.getAccount().Privilege < 7) {
      Invalid();
      return;
    }
    string S = s;
    stringstream ss;
    ss << s;
    getline(ss, s, ' ');
    if (s != "delete") {
      Invalid();
      return;
    }
    string UserID;
    if (!getline(ss, UserID, ' ')) {
      Invalid();
      return;
    }
    if (getline(ss, s, ' ')) {
      Invalid();
      return;
    }
    if (!chk1(UserID)) {
      Invalid();
      return;
    }
    if (!account.deleteUser(UserID)) {
      Invalid();
      return;
    }
    LOG(s, account.getAccount().UserID);
    return;
  }

  bool chk4(string s) { // ISBN
    if (s.size() > 20 || !s.size())
      return 0;
    int len = s.size();
    for (int i = 0; i < len; i++)
      if (s[i] < 32 || s[i] > 126)
        return 0;
    return 1;
  }
  bool chk5(string s) { // BookName, Author
    if (s.size() > 60 || !s.size())
      return 0;
    int len = s.size();
    for (int i = 0; i < len; i++)
      if (!(s[i] >= 32 && s[i] <= 126 && s[i] != '\"'))
        return 0;
    return 1;
  }
  bool chkkey(string s) {
    if (s.size() > 60 || !s.size())
      return 0;
    int L = s.size();
    for (int i = 0; i < L; i++)
      if (!(s[i] >= 32 && s[i] <= 126 && s[i] != '\"'))
        return 0;
    if (s[0] == '|' || s[s.size() - 1] == '|')
      return 0;
    for (int i = 1; i < L - 1; i++)
      if (s[i] == '|' && s[i + 1] == '|')
        return 0;
    string aux = "", str[60];
    int len = 0;
    for (int i = 0; i < L; i++) {
      if (s[i] != '|')
        aux += s[i];
      else
        str[++len] = aux, aux = "";
    }
    str[++len] = aux, aux = "";
    sort(str + 1, str + len + 1);
    for (int i = 1; i < len; i++)
      if (str[i] == str[i + 1])
        return 0;
    return 1;
  }
  int chk6(string s) { // Quantity,Count
    if (s.size() > 10)
      return -1;
    long long num = 0;
    int len = s.size();
    if (len > 1 && s[0] == '0')
      return -1;
    for (int i = 0; i < len; i++)
      if (s[i] < '0' || s[i] > '9')
        return -1;
      else
        num = num * 10 + s[i] - '0';
    if (num > 2147483647ll)
      return -1;
    return num;
  }
  double chk7(string s) { // Price
    if (s.size() > 13)
      return -1;
    stringstream ss;
    ss << s;
    int len = s.size(), cnt = 0;
    for (int i = 0; i < len; i++)
      if (!(s[i] >= '0' && s[i] <= '9' || s[i] == '.'))
        return -1;
    if (len == 1) {
      if (s[0] == '.')
        return -1;
      return s[0] - '0';
    }
    if (s[0] == '0' && s[1] != '.')
      return -1;
    for (int i = 0; i < len; i++)
      if (s[i] == '.')
        cnt++;
    if (cnt > 1 || s[0] == '.' || s[len - 1] == '.')
      return -1;
    if (cnt == 1 && len >= 3 && s[len - 1] != '.' && s[len - 2] != '.' &&
        s[len - 3] != '.')
      return -1;
    double num;
    ss >> num;
    if (ss >> s)
      return -1;
    return num;
  }

  void Runshow(string s) {
    if (account.getAccount().Privilege < 1) {
      Invalid();
      return;
    }
    string S = s;
    stringstream ss;
    ss << s;
    getline(ss, s, ' ');
    if (s != "show") {
      Invalid();
      return;
    }
    if (!getline(ss, s, ' ')) {
      book.query();
      return;
    }
    string tmp;
    if (getline(ss, tmp, ' ')) {
      Invalid();
      return;
    }
    bool tag = 0;
    string str;
    if (s.size() >= 7 && s.substr(0, 6) == "-ISBN=") {
      tag = 1;
      str = s.substr(6, s.size() - 6);
      if (!chk4(str)) {
        Invalid();
        return;
      }
      book.queryISBN(str);
    } else if (s.size() >= 9 && s.substr(0, 7) == "-name=\"" &&
               s.back() == '\"') {
      tag = 1;
      str = s.substr(7, s.size() - 8);
      if (!chk5(str)) {
        Invalid();
        return;
      }
      book.queryBookName(str);
    } else if (s.size() >= 11 && s.substr(0, 9) == "-author=\"" &&
               s.back() == '\"') {
      tag = 1;
      str = s.substr(9, s.size() - 10);
      if (!chk5(str)) {
        Invalid();
        return;
      }
      book.queryAuthor(str);
    } else if (s.size() >= 12 && s.substr(0, 10) == "-keyword=\"" &&
               s.back() == '\"') {
      str = s.substr(10, s.size() - 11);
      if (!chkkey(str)) {
        Invalid();
        return;
      }
      tag = book.queryKeyword(str);
    }
    if (!tag) {
      Invalid();
      return;
    }
    LOG(S, account.getAccount().UserID);
    return;
  }
  void Runbuy(string s) {
    if (account.getAccount().Privilege < 1) {
      Invalid();
      return;
    }
    string oper = s;
    stringstream ss;
    ss << s;
    getline(ss, s, ' ');
    if (s != "buy") {
      Invalid();
      return;
    }
    string ISBN, Quantity;
    if (!getline(ss, ISBN, ' ')) {
      Invalid();
      return;
    }
    if (!getline(ss, Quantity, ' ')) {
      Invalid();
      return;
    }
    if (getline(ss, s, ' ')) {
      Invalid();
      return;
    }
    int num = chk6(Quantity);
    if (!chk4(ISBN) || num <= 0) {
      Invalid();
      return;
    }
    double v = book.buy(ISBN, num);
    if (v < 0) {
      Invalid();
      return;
    }
    array<char, operlen> tmp;
    int len = oper.size();
    for (int i = 0; i < len; i++)
      tmp[i] = oper[i];
    log.addFinance(v, 0, account.getAccount().UserID, oper.size(), tmp);
    LOG(oper, account.getAccount().UserID);
    return;
  }
  void Runselect(string s) {
    if (account.getAccount().Privilege < 3) {
      Invalid();
      return;
    }
    string S = s;
    stringstream ss;
    ss << s;
    getline(ss, s, ' ');
    if (s != "select") {
      Invalid();
      return;
    }
    string ISBN;
    if (!getline(ss, ISBN, ' ')) {
      Invalid();
      return;
    }
    if (getline(ss, s, ' ')) {
      Invalid();
      return;
    }
    account.select(book.select(ISBN));
    LOG(S, account.getAccount().UserID);
    return;
  }
  void Runmodify(string s) {
    if (account.getAccount().Privilege < 3) {
      Invalid();
      return;
    }
    string S = s;
    stringstream ss;
    ss << s;
    getline(ss, s, ' ');
    if (s != "modify") {
      Invalid();
      return;
    }
    bool vis[5] = {0};
    string str[5];
    double price;
    while (getline(ss, s, ' ')) {
      if (s.size() >= 7 && s.substr(0, 6) == "-ISBN=") {
        if (vis[0]) {
          Invalid();
          return;
        }
        vis[0] = 1, str[0] = s.substr(6, s.size() - 6);
        if (!chk4(str[0])) {
          Invalid();
          return;
        }
      } else if (s.size() >= 9 && s.substr(0, 7) == "-name=\"" &&
                 s.back() == '\"') {
        if (vis[1]) {
          Invalid();
          return;
        }
        vis[1] = 1, str[1] = s.substr(7, s.size() - 8);
        if (!chk5(str[1])) {
          Invalid();
          return;
        }
      } else if (s.size() >= 11 && s.substr(0, 9) == "-author=\"" &&
                 s.back() == '\"') {
        if (vis[2]) {
          Invalid();
          return;
        }
        vis[2] = 1, str[2] = s.substr(9, s.size() - 10);
        if (!chk5(str[2])) {
          Invalid();
          return;
        }
      } else if (s.size() >= 12 && s.substr(0, 10) == "-keyword=\"" &&
                 s.back() == '\"') {
        if (vis[3]) {
          Invalid();
          return;
        }
        vis[3] = 1, str[3] = s.substr(10, s.size() - 11);
        if (!chkkey(str[3])) {
          Invalid();
          return;
        }
      } else if (s.size() >= 8 && s.substr(0, 7) == "-price=") {
        if (vis[4]) {
          Invalid();
          return;
        }
        vis[4] = 1, str[4] = s.substr(7, s.size() - 7);
        price = chk7(str[4]);
        if (price < 0) {
          Invalid();
          return;
        }
      } else {
        Invalid();
        return;
      }
    }
    if (vis[0]) {
      if (!book.modifyISBN(str[0])) {
        Invalid();
        return;
      }
    }
    if (vis[1]) {
      if (!book.modifyBookName(str[1])) {
        Invalid();
        return;
      }
    }
    if (vis[2]) {
      if (!book.modifyAuthor(str[2])) {
        Invalid();
        return;
      }
    }
    if (vis[3]) {
      if (!book.modifyKeyword(str[3])) {
        Invalid();
        return;
      }
    }
    if (vis[4]) {
      if (!book.modifyPrice(price)) {
        Invalid();
        return;
      }
    }
    if (!vis[0] && !vis[1] && !vis[2] && !vis[3] && !vis[4]) {
      Invalid();
      return;
    }
    LOG(S, account.getAccount().UserID);
    return;
  }
  void Runimport(string s) {
    if (account.getAccount().Privilege < 3) {
      Invalid();
      return;
    }
    stringstream ss;
    ss << s;
    getline(ss, s, ' ');
    if (s != "import") {
      Invalid();
      return;
    }
    string Quantity, TotalCost;
    if (!getline(ss, Quantity, ' ')) {
      Invalid();
      return;
    }
    if (!getline(ss, TotalCost, ' ')) {
      Invalid();
      return;
    }
    if (getline(ss, s, ' ')) {
      Invalid();
      return;
    }
    int num = chk6(Quantity);
    double cost = chk7(TotalCost);
    if (num <= 0 || cost <= 0) {
      Invalid();
      return;
    }
    if (!book.import(num)) {
      Invalid();
      return;
    }
    string oper = "import ";
    String20 ISBN = book.querycur();
    array<char, 20> str = ISBN.getstr();
    int L = ISBN.getlen();
    for (int i = 0; i < L; i++)
      oper += str[i];
    oper += ' ';
    oper += Quantity + " " + TotalCost;
    array<char, operlen> tmp;
    int len = oper.size();
    for (int i = 0; i < len; i++)
      tmp[i] = oper[i];
    log.addFinance(0, cost, account.getAccount().UserID, len, tmp);
    LOG(oper, account.getAccount().UserID);
    return;
  }

  void Runshowfinance(string s) {
    if (account.getAccount().Privilege < 7) {
      Invalid();
      return;
    }
    stringstream ss;
    ss << s;
    getline(ss, s, ' ');
    if (s != "show") {
      Invalid();
      return;
    }
    getline(ss, s, ' ');
    if (s != "finance") {
      Invalid();
      return;
    }
    string Count;
    if (getline(ss, Count, ' ')) {
      int num = chk6(Count);
      if (num < 0) {
        Invalid();
        return;
      }
      if (!log.showFinance(num)) {
        Invalid();
        return;
      }
    } else
      log.showFinance();
    return;
  }
  void Runlog(string s) {
    if (account.getAccount().Privilege < 7) {
      Invalid();
      return;
    }
    stringstream ss;
    ss << s;
    getline(ss, s, ' ');
    if (s != "log") {
      Invalid();
      return;
    }
    if (getline(ss, s, ' ')) {
      Invalid();
      return;
    }
    log.showLog();
    return;
  }
  void Runreport(string s) {
    if (account.getAccount().Privilege < 7) {
      Invalid();
      return;
    }
    stringstream ss;
    ss << s;
    getline(ss, s, ' ');
    if (s != "report") {
      Invalid();
      return;
    }
    getline(ss, s, ' ');
    string tmp;
    if (getline(ss, tmp, ' ')) {
      Invalid();
      return;
    }
    if (s == "finance")
      log.showFinanceInfo();
    else if (s == "employee")
      log.showEmployeeLog();
    else
      Invalid();
    return;
  }
};
#endif