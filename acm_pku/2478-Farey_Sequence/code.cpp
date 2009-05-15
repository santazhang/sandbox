#include <vector>
#include <iostream>

using namespace std;

int gcd(int a, int b) {
  for (;;) {
    int r = a % b;
    if (r == 0)
      break;
    a = b;
    b = r;
  }
  return b;
}

int phi_count(int n) {
  int c = 1;
  for (int i = 2; i < n; i++) {
    if (gcd(i, n) == 1)
      c++;
  }
  return c;
}

int main()
{
  vector<int> f;  // farey count
  vector<int> phi;  // number of co-primes

  f.push_back(0); // f[0] = f[1] = 0
  f.push_back(0);
  phi.push_back(0);

  int n;
  while (true) {
    cin >> n;
    if (n == 0)
      break;
    if (n >= phi.size()) {
      for (int i = phi.size(); i <= n; i++) {
        phi.push_back(phi_count(i));
      }
    }
    /*for (int i = 0; i < phi.size(); i++) {
      cout << phi[i] << ' ';
    }*/
    if (n >= f.size()) {
      for (int i = f.size(); i <= n; i++) {
        f.push_back(f[i - 1] + phi[i]);
      }
    }
    cout << f[n] << endl;
  }
  return 0;
}

