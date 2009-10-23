#include <map>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

using namespace std;

int tele_no(const string& str) {
  int v = 0;
  for (int i = 0; i < str.length(); i++) {
    char ch = str[i];
    if (ch == '-')
      continue;
    
    v *= 10;
    
    if ('0' <= ch && ch <= '9') {
      v += ch - '0';
    } else if ('A' <= ch && ch <= 'C') {
      v += 2;
    } else if ('D' <= ch && ch <= 'F') {
      v += 3;
    } else if ('G' <= ch && ch <= 'I') {
      v += 4;
    } else if ('J' <= ch && ch <= 'L') {
      v += 5;
    } else if ('M' <= ch && ch <= 'O') {
      v += 6;
    } else if ('P' <= ch && ch <= 'S') {
      v += 7;
    } else if ('T' <= ch && ch <= 'V') {
      v += 8;
    } else if ('W' <= ch && ch <= 'Z') {
      v += 9;
    }
  }
  return v;
}

int main() {
  map<int, int> dir;
  vector<int> order;
  int n;
  cin >> n;
  order.reserve(100000);
  string str;
  for (int i = 0; i < n; i++) {
    
    cin >> str;
    int tel = tele_no(str);
    order.push_back(tel);
    dir[tel]++;
  }
  //sort(order.begin(), order.end());
  /*int last_tel = -1;
  for (int i = 0; i < order.size(); i++) {
    if (order[i] != last_tel) {
      last_tel = order[i];
      if (dir[order[i]] > 1)
        cout << std_form(order[i]) << " " << dir[order[i]] << endl;
    }
  }*/
  str = "123-4567";
  int has_out = 0;
  for (map<int, int>::iterator it = dir.begin(); it != dir.end(); ++it) {
    if (it->second > 1) {
      
      int w = 1000000;
      int v = it->first;
      for (int i = 0; i < 8; i++) {
        int a = v / w;
        char c = (char)(a + '0');
        if (i == 3) {
          str[i] = '-';
          i++;
        }
        str[i] = c;
        v %= w;
        w /= 10;
      }
      cout << str << " " << it->second << endl;
      has_out = 1;
    }
  }
  if (has_out == 0) {
    cout << "No duplicates." << endl;
  }
  return 0;
}
