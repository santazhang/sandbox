#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "bigint.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int bigint_alloc_count();
extern void bigint_mul_by_trad(bigint* p_dst, bigint* p_src);

#ifdef __cplusplus
};
#endif

int test_bigint_init_release() {
  bigint bi, bi2, bi3;
  printf("==== TEST BIGINT INIT RELEASE ====\n");
  bigint_init(&bi);
  printf("Just initialized a bigint (bi)\n");
  bigint_init(&bi2);
  printf("Just initialized a bigint (bi2)\n");
  bigint_init(&bi3);
  printf("Just initialized a bigint (bi3)\n");
  bigint_release(&bi3);
  printf("Just released a bigint(bi3)\n");
  bigint_release(&bi2);
  printf("Just released a bigint(bi2)\n");
  bigint_release(&bi);
  printf("Just released a bigint(bi)\n");
  printf("\n");
  return 0;
}

int test_bigint_from_string() {
  char str[10000];
  bigint bi;
  int holder = 2147483647;
  printf("==== TEST BIGINT FROM STRING ==== \n");
  bigint_init(&bi);
  bigint_from_string(&bi, "-0.0055e2");
  bigint_to_string(&bi, str);
  printf("%s\n", str);
  bigint_from_string(&bi, "0");
  bigint_to_string(&bi, str);
  printf("%s\n", str);
  printf("*** %d\n", bigint_to_int(&bi, &holder));
  printf("*** holder = %d\n", holder);
  bigint_release(&bi);
  printf("\n");
  return 0;
}

int test_bigint_alloc_counter() {
  printf("==== TEST BIGINT ALLOC COUNTER ====\n");
  printf("Currently, alloc counter = %d\n", bigint_alloc_count());
  printf("\n");
  return 0;
}

int test_bigint_mul_by_pow_10() {
  bigint bi;
  char str[100];
  printf("==== TEST BIGINT MUL BY POW 10 ====\n");  
  bigint_init(&bi);
  bigint_from_int(&bi, 123);
  bigint_mul_by_pow_10(&bi, -300);
  bigint_to_string(&bi, str);
  printf("%s\n", str);
  bigint_release(&bi);
  printf("\n");
  return 0;
}

int test_bigint_div() {
  bigint bi;
  char str[2000];
  bigint_init(&bi);
  printf("==== TEST BIGINT DIV ====\n");
  bigint_from_string(&bi, "234328972139484");
  bigint_div_by_int(&bi, 123);
  bigint_to_string(&bi, str);
  printf("div result = %s\n", str);
  printf("\n");
  bigint_release(&bi);
  return 0;
}

int test_bigint_div_by_pow_10() {
  bigint bi;
  char str[10000];
  printf("==== TEST BIGINT DIV BY POW 10 ====\n");
  bigint_init(&bi);
  bigint_from_int(&bi, 123);
  bigint_div_by_pow_10(&bi, -1010);
  bigint_to_string(&bi, str);
  printf("%s\n", str);
  bigint_release(&bi);
  printf("\n");
  return 0;
}

int test_bigint_mod() {
  bigint bi;
  int v;
  char str[10000];
  printf("==== TEST BIGINT MOD ====\n");
  bigint_init(&bi);
  bigint_from_string(&bi, "1231231235325423425");
  bigint_mod_by_int(&bi, 13, &v);
  printf("v = %d\n", v);
  bigint_mod_by_pow_10(&bi, 3);
  bigint_to_string(&bi, str);
  printf("str = %s\n", str);
  bigint_release(&bi);
  printf("\n");
  return 0;
}

int test_bigint_mul_by() {
  bigint bi;
  bigint bi2;
  char* num1 = "123";
  char* num2 = "-23";
  char str[10000];
  bigint_init(&bi);
  bigint_init(&bi2);
  printf("==== TEST BIGINT MUL BY====\n");
  bigint_from_string(&bi, num1);
  bigint_from_string(&bi2, num2);
  bigint_mul_by_trad(&bi, &bi2);
  bigint_to_string(&bi, str);
  printf("%s * %s = \n%s = \n", num1, num2, str);
  bigint_from_string(&bi, num1);
  bigint_mul_by(&bi, &bi2);
  bigint_to_string(&bi, str);
  printf("%s\n", str);
  bigint_release(&bi);
  bigint_release(&bi2);
  printf("\n");
  return 0;
}

int test_bigint_mul_by_trad() {
  bigint bi;
  bigint bi2;
  char str[1000];
  bigint_init(&bi);
  bigint_init(&bi2);
  printf("==== TEST BIGINT MUL BY TRAD ====\n");
  bigint_from_int(&bi, 12312);
  bigint_from_int(&bi2, 2412233);
  bigint_mul_by_trad(&bi, &bi2);
  bigint_to_string(&bi, str);
  assert(strcmp(str, "29699412696") == 0);
  printf("*** %s ***\n", str);
  bigint_mul_by_int(&bi, 123123123);
  bigint_to_string(&bi, str);
  assert(strcmp(str, "3656684442397369608") == 0);
  printf("*** %s ***\n", str);
  printf("\n");
  bigint_release(&bi);
  bigint_release(&bi2);
  return 0;
}

void sterling_number_1st_kind(bigint* p_bigint, int n, int k) {
  if (n < k || k == 0) {
    bigint_set_zero(p_bigint);
  } else if (n == k || k == 1) {
    bigint_set_one(p_bigint);
  } else {
    bigint bi;
    bigint_init(&bi);
    sterling_number_1st_kind(p_bigint, n - 1, k - 1);
    sterling_number_1st_kind(&bi, n - 1, k);
    bigint_mul_by_int(&bi, k);
    bigint_add_by(p_bigint, &bi);
    bigint_release(&bi);
  }
}

int test_sterling_number_1st_kind(int n, int k) {
  bigint bi;
  char str[20000];
  bigint_init(&bi);
  printf("==== TEST STERLING NUMBER 1ST KIND ====\n");
  sterling_number_1st_kind(&bi, n, k);
  bigint_to_string(&bi, str);
  printf("sterling_1st(%d, %d) = %s\n", n, k, str);
  printf("\n");
  bigint_release(&bi);
  return 0;
}

int test_bigint_show_fibonacci(int nth) {
  bigint f1;
  bigint f2;
  bigint* p_f1 = &f1;
  bigint* p_f2 = &f2;
  bigint* p_swap;
  int i;
  char str[200000];
  printf("==== TEST BIGINT SHOW FIBONACCI ===\n");
  bigint_init(&f1);
  bigint_init(&f2);
  bigint_from_int(&f1, 0);
  bigint_from_int(&f2, 1);
  for (i = 0; i < nth; i++) {
    bigint_add_by(p_f1, p_f2);
    p_swap = p_f1;
    p_f1 = p_f2;
    p_f2 = p_swap;
  }
  bigint_to_string(p_f1, str);
  printf("%s\n---------------------\n", str);
  bigint_to_string(p_f2, str);
  printf("%s\n", str);
  printf("\n----------------------\n");
  bigint_mul_by(p_f1, p_f1);
  bigint_to_string(p_f1, str);
  printf("%s\n---------------------\n", str);
  bigint_mul_by_trad(p_f2, p_f2);
  bigint_to_string(p_f2, str);
  printf("%s\n", str);
  printf("++++++++++++++++++++++++\n");
  bigint_from_int(p_f2, 12345);
  bigint_pow_by_int(p_f2, 120);
  bigint_to_string(p_f2, str);
  printf("%s\n", str);
  bigint_release(&f1);
  bigint_release(&f2);
  return 0;
}

void profile_big_cal() {
  bigint f1;
  bigint f2;
  bigint* p_f1 = &f1;
  bigint* p_f2 = &f2;
  bigint_init(&f1);
  bigint_init(&f2);
  bigint_from_int(p_f1, 12345);
  bigint_from_int(p_f2, 12345);
  bigint_pow_by_int(p_f2, 20000);
  bigint_release(&f1);
  bigint_release(&f2);
}

// check the unit test results
void test(int return_code) {
  if (return_code != 0) {
    printf("*** OOPS, a test has failed\n");
    exit(1);
  }
}

void test_scientific() {
  bigint b;
  char str_val[100];
  int i;
  bigint_init(&b);
  for (i = 0; i < 10; i++) {
    double base;
    int expo;
    printf("%d of 10, input a bigint string val:\n", i + 1);
    scanf("%s", str_val);
    bigint_from_string(&b, str_val);
    bigint_to_scientific(&b, &base, &expo);
    printf("sci = %lf    E   %d\n", base, expo);
  }
  bigint_release(&b);
}

void print_bigint(bigint* b) {
  char* str = (char *) malloc(bigint_string_length(b) + 10);
  bigint_to_string(b, str);
  printf("%s", str);
  free(str);
}

void test_division() {
  bigint a, b, q, r;
  char str_val[100];
  bigint_init(&a);
  bigint_init(&b);
  bigint_init(&r);
  bigint_init(&q);
  printf("(divmod) please input a, b, separate by space:\n");
  scanf("%s", str_val);
  bigint_from_string(&a, str_val);
  scanf("%s", str_val);
  bigint_from_string(&b, str_val);
  print_bigint(&a);
  printf(" divmod ");
  print_bigint(&b);
  bigint_divmod(&a, &b, &q, &r);
  printf("\nq=");
  print_bigint(&q);
  printf("\nr=");
  print_bigint(&r);
  printf("\n");
  bigint_release(&a);
  bigint_release(&b);
  bigint_release(&q);
  bigint_release(&r);
}

int main(int argc, char* argv) {
  test(test_bigint_init_release());
  test(test_bigint_from_string());
  test(test_bigint_mul_by_pow_10());
  test(test_bigint_div());
  test(test_bigint_div_by_pow_10());
  test(test_bigint_mod());
  test(test_bigint_mul_by_trad());
  test(test_bigint_mul_by());
  test(test_sterling_number_1st_kind(14, 12));
  test(test_bigint_show_fibonacci(10000));
//  profile_big_cal();
//  test_scientific();
  test_division();
  test(test_bigint_alloc_counter());
  return 0;
}

