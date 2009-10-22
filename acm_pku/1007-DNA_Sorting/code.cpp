#include <iostream>
#include <string>
#include <vector>

using namespace std;

int basic_rev_count_method(string& str) {
  int count = 0;
  char ch;
  for (int i = 0; i < str.length(); i++) {
    for (int j = i + 1; j < str.length(); j++) {
      if (str[i] > str[j]) {
        count++;
        
        ch = str[i];
        str[i] = str[j];
        str[j] = ch;
      }
    }
  }
  return count;
}

int rev_count_merge(string& str, int start, int end) {
  
  //if (str.length() < 5000) {
  return basic_rev_count_method(str);
  //}
  /*
  int mid = (end + start) / 2;
  int l_rev = rev_count_merge(str, start, mid);
  int r_rev = rev_count_merge(str, mid, end);
  
  int cross_count = 0;
  
  string new_str;
  
  int l_ptr = 0;
  int r_ptr = mid;
  
  while (l_ptr < mid && r_ptr < end) {
    if (str[l_ptr] <= str[r_ptr]) {
      new_str += str[l_ptr];
      l_ptr++;
      
    } else {
      new_str += str[r_ptr];
      r_ptr++;
      
      cross_count++;
    }
  }
  
  
  str = new_str;
  
  return l_rev + r_rev + cross_count;
  */
}

int real_rev_count(const string& str) {
  string str_cpy = str;
  return rev_count_merge(str_cpy, 0, str_cpy.length());
}

class DNA {
public:

  DNA(const string& _str, int orig_pos) : str(_str), pos(orig_pos){
    my_rev_count = real_rev_count(str);
  }
  
  DNA(const DNA& dna) {
    this->str = dna.str;
    this->my_rev_count = dna.my_rev_count;
  }
  
  bool operator< (const DNA& another) const {
    if (this->my_rev_count < another.my_rev_count) {
      return true;
    } else if (this->my_rev_count == another.my_rev_count){
      return this->pos < another.pos;
    } else {
      return false;
    }
  }
  
  bool operator> (const DNA& another) const {
    if (this->my_rev_count > another.my_rev_count) {
      return true;
    } else if (this->my_rev_count == another.my_rev_count){
      return this->pos > another.pos;
    } else {
      return false;
    }
  }
  
  bool operator== (const DNA& another) const {
    return this->my_rev_count == another.my_rev_count && this->pos == another.pos;
  }
  
  int get_rev() {
    return my_rev_count;
  }
  
  const string& get_str() {
    return str;
  }
  
private:
  int my_rev_count;
  int pos;
  string str;
};

void my_sort(vector<DNA>& vec) {
  for (int i = 0; i < vec.size(); i++) {
    int min_pos = i;
    for (int j = i + 1; j < vec.size(); j++) {
      if (vec[min_pos] > vec[j]) {
        min_pos = j;
      }
    }
    if (min_pos != i) {
      DNA swap = vec[min_pos];
      vec[min_pos] = vec[i];
      vec[i] = swap;
    }
  }
}

int main() {
  int n, m;
  string str;
  vector<DNA> dnas;
  cin >> n >> m;
  for (int i = 0; i < m; i++) {
    cin >> str;
    DNA dna(str, i);
    //cout << "REVCOUNT = " << dna.get_rev() << endl;
    dnas.push_back(dna);
  }
  my_sort(dnas);
  for (int i = 0; i < dnas.size(); i++) {
    cout << dnas[i].get_str() << endl;
  }
  return 0;
}
