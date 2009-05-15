#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <cstdlib>
#include <cmath>

using namespace std;

class Term {
  
public:

  Term() : coef(0), exp(0) {
  }

  Term(string str) : coef(1), exp(1) {
    istringstream trim(str);
    trim >> str;  // trim white space

    bool neg_coef = false;

    if (str[0] == '-') {
      neg_coef = true;
      str = str.substr(1);
    }

    istringstream istr(str);
    int num;
    if (str[0] != 'x') {
      istr >> num;

      if (num != 0) {
        coef = num;
      }
    }

    if (neg_coef) {
      coef = -coef;
    }

    char ch;
    istr >> ch; // should be 'x'
    if (!istr) {
      // no 'x' found, this is const term
      exp = 0;
    } else {

      istr >> ch; // test if there is '^'
      if (istr && ch == '^') {
        istr >> num;
        exp = num;
      }
    }
  }

  Term(const Term& t) : coef(t.coef), exp(t.exp) {
  }

  Term d() {
    Term t;
    if (exp == 0) {
      t.coef = 0;
      t.exp = 0;
    } else {
      t.coef = coef * exp;
      t.exp = exp - 1;
    }
    return t; 
  }

  void merge_by(const Term& t) {
    if (t.exp != exp) {
      //printf("Error: different exp!\n");
      exit(1);
    } else {
      coef += t.coef;
      if (coef == 0) {
        exp = 0;
      }
    }
  }

  double eval(double x) {
    double p = 1.0;
    for (int i = 0; i < exp; i++) {
      p *= x;
    }
    return coef * p;
  }

  string to_str() {
    ostringstream ostr;
    if (coef == 0) {
      ostr << 0;
    } else if (coef == 1 || coef == -1) {
      if (exp == 0) {
        ostr << '1';
      } else {
        if (coef == -1)
          cout << '-';
        ostr << 'x';
        if (exp != 1) {
          ostr << '^' << exp;
        }
      }
    } else {
      ostr << coef;
      if (exp != 0) {
        ostr << 'x';
        if (exp != 1) {
          ostr << '^' << exp;
        }
      }
    }
    return ostr.str();
  }

  int coef;
  int exp;

};

class Function {

public:

  void add_term(const Term& t) {
    if (terms.find(t.exp) != terms.end()) {
      Term term = terms[t.exp];
      term.merge_by(t);
      if (term.exp == 0 && term.coef == 0) {
        // cancelled iterms
        terms.erase(terms.find(t.exp));
      } else {
        terms[t.exp] = term;
      }
    } else {
      terms[t.exp] = t;
    }
  }

  Function d() {
    Function f;
    for (map<int, Term>::iterator it = terms.begin(); it != terms.end(); ++it) {
      f.add_term(it->second.d());
    }
    return f;
  }

  double eval(double x) {
    double v = 0;
    for (map<int, Term>::iterator it = terms.begin(); it != terms.end(); ++it) {
      v += it->second.eval(x);
    }
    return v;
  }

  string to_str() {
    ostringstream ostr;
    if (terms.empty() == false) {
      for (map<int, Term>::iterator it = terms.begin(); it != terms.end(); ++it) {
        if (it->second.coef >= 0 && it != terms.begin())
          ostr << '+';
        ostr << it->second.to_str() << " ";
      }
    } else {
      ostr << "0";
    }
    return ostr.str();
  }

  map<int, Term> terms;
};

int main()
{
  string line;
  while (getline(cin, line)) {
    istringstream istr(line);
    string frag;
    Function f;
    Function fprime;
    while (istr >> frag) {
      if (frag[0] == '=') {
        break;
      }
      //cout << frag << endl;
      Term t(frag);
      //cout << t.to_str() << endl;
      f.add_term(t);
    }
    //cout << "** " << f.to_str() << endl;
    fprime = f.d();
    //cout << "'' " << fprime.to_str() << endl;

    double x;
    getline(cin, line);
    istringstream istr2(line);
    istr2 >> x;
    
    int iter = 0;
    while (iter <= 1000) {
      //string str;
      //ostringstream ostr(str);
      double fx = f.eval(x);
      //cout << "x:" << x << endl;
      //cout << "fx: " << fx << endl;
      //ostr << f.to_str() << " x:" << x << " eval:" << fx << endl;
      if (abs(fx) < 0.000001) {
        break;
      }
      double fprime_x = fprime.eval(x);
      //cout << "f'x: " << fprime_x << endl;
      //ostr << fprime.to_str() << " x:" << x << " eval:" << fprime_x << endl;
      double new_x = x - (f.eval(x) / fprime.eval(x));
      //cout << "new_x: " << new_x << endl;
      if (abs(new_x - x) < 0.000001) {
        break;
      }
      x = new_x;
      //cout << endl;
      iter++;
    }
    if (iter > 1000) {
      cout << "Bad x0 or bad equation!" << endl;
    } else {
      cout << iter << endl;
    }
    //cout << x << " : " << f.eval(x) << endl;

  }
  return 0;
}

