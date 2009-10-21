#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
typedef struct {
  int* p_data;
  int sign;
  int data_len;
  int mem_size;
} bigint;
typedef enum {
  BIGINT_NOERR = 0,
  BIGINT_ILLEGAL_PARAM = 1,
  BIGINT_OVERFLOW = 2,
} bigint_errno;
void bigint_init(bigint* p_bigint);
void bigint_release(bigint* p_bigint);
void bigint_from_int(bigint* p_bigint, int value);
bigint_errno bigint_from_double(bigint* p_bigint, double value);
bigint_errno bigint_from_string(bigint* p_bigint, char* str);
int bigint_digit_count(bigint* p_bigint);
int bigint_string_length(bigint* p_bigint);
void bigint_to_string(bigint* p_bigint, char* str);
bigint_errno bigint_to_double(bigint* p_bigint, double* p_double);
bigint_errno bigint_to_int(bigint* p_bigint, int* p_int);
void bigint_copy(bigint* p_dst, bigint* p_src);
void bigint_change_sign(bigint* p_bigint);
int bigint_is_positive(bigint* p_bigint);
int bigint_is_negative(bigint* p_bigint);
int bigint_is_zero(bigint* p_bigint);
void bigint_set_zero(bigint* p_bigint);
void bigint_set_one(bigint* p_bigint);
void bigint_add_by(bigint* p_dst, bigint* p_src);
void bigint_add_by_int(bigint* p_dst, int value);
void bigint_sub_by(bigint* p_dst, bigint* p_src);
void bigint_sub_by_int(bigint* p_dst, int value);
void bigint_mul_by(bigint* p_dst, bigint* p_src);
void bigint_mul_by_int(bigint* p_dst, int value);
void bigint_mul_by_pow_10(bigint* p_bigint, int pow);
bigint_errno bigint_pow_by_int(bigint* p_bigint, int pow);
bigint_errno bigint_div_by(bigint* p_dst, bigint* p_src);
bigint_errno bigint_div_by_int(bigint* p_bigint, int div);
void bigint_div_by_pow_10(bigint* p_bigint, int pow);
bigint_errno bigint_square_root_ceiling(bigint* p_bigint);
bigint_errno bigint_square_root_floor(bigint* p_bigint);
bigint_errno bigint_root_n_Ceiling(bigint* p_bigint, int n);
bigint_errno bigint_root_n_floor(bigint* p_bigint, int n);
void bigint_mod_by(bigint* p_dst, bigint* p_src);
bigint_errno bigint_mod_int(bigint* p_bigint, int value, int* p_result);
bigint_errno bigint_mod_by_pow_10(bigint* p_bigint, int pow);
int bigint_compare(bigint* p_bigint1, bigint* p_bigint2);
int bigint_equal(bigint* p_bigint1, bigint* p_bigint2);
int bigint_nth_digit(bigint* p_bigint, int nth);
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#define BIGINT_INT_MAX_DIGITS 10
#define BIGINT_INIT_MEM_SIZE 4
#define BIGINT_DOUBLE_PRECISION 16
#define BIGINT_MUL_THRESHOLD 100
#define BIGINT_RADIX 1000000000
#define BIGINT_RADIX_LOG10 9
#define BIGINT_ALLOC(size) bigint_alloc(size)
#define BIGINT_FREE(ptr) bigint_free(ptr)
static int bigint_alloc_counter = 0;
static void* bigint_alloc(int size) {
  bigint_alloc_counter++;
  return malloc(size);
}
static void bigint_free(void* ptr) {
  bigint_alloc_counter--;
  assert(bigint_alloc_counter >= 0);
  free(ptr);
}
int bigint_alloc_count() {
  return bigint_alloc_counter;
}
static void bigint_set_memory_size(bigint* p_bigint, int new_mem_size) {
  int* old_ptr = p_bigint->p_data;
  int* new_ptr = BIGINT_ALLOC(sizeof(int) * new_mem_size);
  memcpy(new_ptr, old_ptr, sizeof(int) * p_bigint->data_len);
  p_bigint->mem_size = new_mem_size;
  p_bigint->p_data = new_ptr;
  BIGINT_FREE(old_ptr);
}
static void bigint_assure_memory(bigint* p_bigint, int min_size) {
  assert(min_size > 0);
  if (min_size > p_bigint->mem_size) {
    bigint_set_memory_size(p_bigint, min_size * 2);
  }
}
static void bigint_pack_memory(bigint* p_bigint) {
  int index = p_bigint->data_len - 1;
  while (index > 0 && p_bigint->p_data[index] == 0) {
    p_bigint->data_len--;
    index--;
  }
  if (p_bigint->data_len * 4 < p_bigint->mem_size) {
    bigint_set_memory_size(p_bigint, p_bigint->data_len * 2);
  }
}
void bigint_init(bigint* p_bigint) {
  p_bigint->mem_size = BIGINT_INIT_MEM_SIZE;
  p_bigint->p_data = BIGINT_ALLOC(sizeof(int) * p_bigint->mem_size);
  p_bigint->data_len = 1;
  p_bigint->sign = 0;
  p_bigint->p_data[0] = 0;
}
void bigint_release(bigint* p_bigint) {
  BIGINT_FREE(p_bigint->p_data);
  p_bigint->p_data = NULL;
}
void bigint_from_int(bigint* p_bigint, int value) {
  if (value < 0) {
    p_bigint->sign = -1;
  } else if (value > 0) {
    p_bigint->sign = 1;
  } else {
    p_bigint->sign = 0;
  }
  if (value < 0) {
    value = -value;
  }
  if (value == 0) {
    p_bigint->data_len = 1;
    p_bigint->p_data[0] = 0;
  } else {
    int index = 0;
    p_bigint->data_len = 0;
    while (value > 0) {
      int lower_value = value % BIGINT_RADIX;
      value = value / BIGINT_RADIX;
      bigint_assure_memory(p_bigint, p_bigint->data_len + 1);
      p_bigint->p_data[index] = lower_value;
      p_bigint->data_len++;
      index++;
    }
  }
  bigint_pack_memory(p_bigint);
}
bigint_errno bigint_from_string(bigint* p_bigint, char* str) {
  int approx_len = 0;
  int has_neg_sign = 0;
  int mantissa_value = 0;
  int fixed_begin = -1;
  int fixed_end = -1;
  int fixed_len = 0;
  int fraction_begin = -1;
  int fraction_end = -1;
  int fraction_len = 0;
  int mantissa_begin = -1;
  int mantissa_end = -1;
  int state = 0;
  int index = 0;
  while (str[index] != '\0') {
    char ch = str[index];
    switch (state) {
      case 0:
        if (ch == '+') {
          state = 1;
        } else if (ch == '-') {
          has_neg_sign = 1;
          state = 1;
        } else if ('0' <= ch && ch <= '9') {
          fixed_begin = index;
          state = 2;
        } else {
          return -BIGINT_ILLEGAL_PARAM;
        }
        break;
      case 1:
        if ('0' <= ch && ch <= '9') {
          fixed_begin = index;
          state = 2;
        } else {
          return -BIGINT_ILLEGAL_PARAM;
        }
        break;
      case 2:
        fixed_end = index;
        if ('0' <= ch && ch <= '9') {
        } else if (ch == '.') {
          state = 3;
        } else if (ch == 'E' || ch == 'e') {
          mantissa_begin = index;
          state = 5;
        } else {
          return -BIGINT_ILLEGAL_PARAM;
        }
        break;
      case 3:
        if ('0' <= ch && ch <= '9') {
          fraction_begin = index;
          state = 4;
        } else {
          return -BIGINT_ILLEGAL_PARAM;
        }
        break;
      case 4:
        fraction_end = index;
        if ('0' <= ch && ch <= '9') {
        } else if (ch == 'E' || ch == 'e') {
          state = 5;
        } else {
          return -BIGINT_ILLEGAL_PARAM;
        }
        break;
      case 5:
        mantissa_begin = index;
        if ('0' <= ch && ch <= '9') {
          state = 7;
        } else if (ch == '+' || ch == '-') {
          state = 6;
        } else {
          return -BIGINT_ILLEGAL_PARAM;
        }
        break;
      case 6:
        if ('0' <= ch && ch <= '9') {
          mantissa_end = index + 1;
          state = 7;
        } else {
          return -BIGINT_ILLEGAL_PARAM;
        }
        break;
      case 7:
        if ('0' <= ch && ch <= '9') {
          mantissa_end = index + 1;
        } else {
          return -BIGINT_ILLEGAL_PARAM;
        }
        break;
    }
    index++;
  }
  if (state == 2) {
    fixed_end = index;
  } else if (state == 4) {
    fraction_end = index;
  } else if (state == 7) {
    mantissa_end = index;
  } else {
    return -BIGINT_ILLEGAL_PARAM;
  }
  if (mantissa_begin != -1) {
    mantissa_value = atoi(str + mantissa_begin);
  } else {
    mantissa_value = 0;
  }
  fixed_len = fixed_end - fixed_begin;
  fraction_len = fraction_end - fraction_begin;
  approx_len = mantissa_value + fixed_len;
  if (approx_len < 0) {
    bigint_set_zero(p_bigint);
  } else if (approx_len == 0) {
    if (str[fixed_begin] >= '5') {
      bigint_set_one(p_bigint);
      if (has_neg_sign) {
        bigint_change_sign(p_bigint);
      }
    } else {
      bigint_set_zero(p_bigint);
    }
  } else {
    int i;
    int pos = 0;
    int weight = 1;
    int is_zero = 1;
    int carry = 0;
    bigint_assure_memory(p_bigint, approx_len / BIGINT_RADIX_LOG10 + 2);
    for (i = 0; i < p_bigint->mem_size; i++) {
      p_bigint->p_data[i] = 0;
    }
    mantissa_value -= fraction_len;
    if (fraction_len > 0) {
      pos = fraction_end - 1;
    } else {
      pos = fixed_end - 1;
    }
    if (mantissa_value < 0) {
      int drop = -mantissa_value;
      while (drop > 0) {
        if (str[pos] == '.')
          pos--;
        if (str[pos] >= '5') {
          carry = 1;
        } else {
          carry = 0;
        }
        pos--;
        drop--;
      }
      mantissa_value = 0;
    }
    index = mantissa_value / BIGINT_RADIX_LOG10;
    i = 0;
    while (index * BIGINT_RADIX_LOG10 + i < mantissa_value) {
      weight *= 10;
      i++;
    }
    while (pos >= fixed_begin) {
      if (str[pos] == '.')
        pos--;
      if ('1' <= str[pos] && str[pos] <= '9' || carry != 0) {
        is_zero = 0;
      }
      p_bigint->p_data[index] += weight * (str[pos] - '0' + carry);
      carry = 0;
      weight *= 10;
      if (weight >= BIGINT_RADIX) {
        weight = 1;
        if (p_bigint->p_data[index] > BIGINT_RADIX) {
          p_bigint->p_data[index] %= BIGINT_RADIX;
          carry = 1;
        }
        index++;
        p_bigint->p_data[index] = 0;
      }
      pos--;
    }
    if (is_zero) {
      bigint_set_zero(p_bigint);
    } else if (has_neg_sign) {
      p_bigint->sign = -1;
      p_bigint->data_len = index + 1;
      bigint_pack_memory(p_bigint);
    } else {
      p_bigint->sign = 1;
      p_bigint->data_len = index + 1;
      bigint_pack_memory(p_bigint);
    }
  }
  return -BIGINT_NOERR;
}
void bigint_to_string(bigint* p_bigint, char* str) {
  int index = p_bigint->data_len - 1;
  int value;
  int first_seg_length = 0;
  int i;
  if (p_bigint->sign < 0) {
    *str = '-';
    str++;
  }
  if (bigint_is_zero(p_bigint)) {
    *str = '0';
    str++;
  } else {
    first_seg_length = 0;
    value = p_bigint->p_data[index];
    while (value > 0) {
      first_seg_length++;
      value /= 10;    
    }
    value = p_bigint->p_data[index];
    i = 0;
    while (value > 0) {
      int r = value % 10;
      value /= 10;
      str[first_seg_length - i - 1] = (char) (r + '0');
      i++;
    }
    str += first_seg_length;
    index--;
    while (index >= 0) {
      int counter = BIGINT_RADIX_LOG10;
      value = p_bigint->p_data[index];
      i = 0;
      while (counter > 0) {
        int r = value % 10;
        value /= 10;
        str[BIGINT_RADIX_LOG10 - i - 1] = (char) (r + '0');
        i++;
        counter--;
      }
      str += BIGINT_RADIX_LOG10;
      index--;
    }
  }
  *str = '\0';
}
void bigint_copy(bigint* p_dst, bigint* p_src) {
  bigint_assure_memory(p_dst, p_src->data_len);
  memcpy(p_dst->p_data, p_src->p_data, sizeof(int) * p_src->data_len);
  p_dst->data_len = p_src->data_len;
  p_dst->sign = p_src->sign;
}
void bigint_change_sign(bigint* p_bigint) {
  p_bigint->sign = -p_bigint->sign;
}
int bigint_is_positive(bigint* p_bigint) {
  return p_bigint->sign > 0;
}
int bigint_is_negative(bigint* p_bigint) {
  return p_bigint->sign < 0;
}
int bigint_is_zero(bigint* p_bigint) {
  return p_bigint->sign == 0;
}
void bigint_set_zero(bigint* p_bigint) {
  p_bigint->data_len = 1;
  p_bigint->p_data[0] = 0;
  p_bigint->sign = 0;
  bigint_pack_memory(p_bigint);
}
void bigint_set_one(bigint* p_bigint) {
  p_bigint->data_len = 1;
  p_bigint->p_data[0] = 1;
  p_bigint->sign = 1;
  bigint_pack_memory(p_bigint);
}
void bigint_add_by(bigint* p_dst, bigint* p_src) {
  if (bigint_is_zero(p_dst)) {
    bigint_copy(p_dst, p_src);
  } else if (bigint_is_zero(p_src)) {
  } else if (p_src->data_len == 1) {
    assert(p_src->sign != 0);
    if (p_src->sign < 0) {
      bigint_add_by_int(p_dst, -p_src->p_data[0]);
    } else if (p_src->sign > 0) {
      bigint_add_by_int(p_dst, p_src->p_data[0]);
    }
  } else if (p_dst == p_src) {
    bigint_mul_by_int(p_dst, 2);
  } else {
    int index;
    int result_mem_size_bound = max(p_dst->data_len, p_src->data_len) + 1;
    bigint_assure_memory(p_dst, result_mem_size_bound);
    for (index = p_dst->data_len; index < result_mem_size_bound; index++) {
      p_dst->p_data[index] = 0;
    }
    p_dst->data_len = result_mem_size_bound;
    if (p_dst->sign < 0) {
      for (index = 0; index < p_dst->data_len; index++) {
        p_dst->p_data[index] = -p_dst->p_data[index];
      }
    }
    if (p_src->sign < 0) {
      for (index = 0; index < p_src->data_len; index++) {
        p_dst->p_data[index] -= p_src->p_data[index];
      }
    } else {
      for (index = 0; index < p_src->data_len; index++) {
        p_dst->p_data[index] += p_src->p_data[index];
      }
    }
    p_dst->sign = 0;
    for (index = p_dst->data_len - 1; index >= 0; index--) {
      if (p_dst->p_data[index] < 0) {
        p_dst->sign = -1;
        break;
      } else if (p_dst->p_data[index] > 0) {
        p_dst->sign = 1;
        break;
      }
    }
    if (p_dst->sign < 0) {
      for (index = 0; index < p_dst->data_len; index++) {
        p_dst->p_data[index] = -p_dst->p_data[index];
      }
    }
    for (index = 0; index < p_dst->data_len; index++) {
      if (p_dst->p_data[index] < 0) {
        p_dst->p_data[index] += BIGINT_RADIX;
        p_dst->p_data[index + 1]--;
      } else if (p_dst->p_data[index] >= BIGINT_RADIX) {
        p_dst->p_data[index] -= BIGINT_RADIX;
        p_dst->p_data[index + 1]++;
      }
    }
    bigint_pack_memory(p_dst);
  }
}
void bigint_add_by_int(bigint* p_dst, int value) {
  if (bigint_is_zero(p_dst)) {
    bigint_from_int(p_dst, value);
  } else if (value == 0) {
  } else if (-BIGINT_RADIX < value && value < BIGINT_RADIX) {
    if (p_dst->data_len == 1) {
      if (p_dst->sign < 0) {
        bigint_from_int(p_dst, -p_dst->p_data[0] + value);
      } else {
        bigint_from_int(p_dst, p_dst->p_data[0] + value);
      }
    } else {
      int value_sign;
      int index = 0;
      int max_segment_incr = (BIGINT_INT_MAX_DIGITS / BIGINT_RADIX_LOG10) + 1;
      if (value < 0) {
        value_sign = -1;
        value = -value;
      } else {
        value_sign = 1;
      }
      bigint_assure_memory(p_dst, p_dst->data_len + max_segment_incr);
      p_dst->p_data[p_dst->data_len] = 0;
      p_dst->data_len++;
      if (value_sign == p_dst->sign) {
        while (value != 0) {
          p_dst->p_data[index] += value;
          if (p_dst->p_data[index] > BIGINT_RADIX) {
            value = 1;
            p_dst->p_data[index] -= BIGINT_RADIX;
          } else {
            value = 0;
          }
          index++;
        }
      } else {
        while (value != 0) {
          p_dst->p_data[index] -= value;
          if (p_dst->p_data[index] < 0) {
            value = 1;
            p_dst->p_data[index] += BIGINT_RADIX;
          } else {
            value = 0;
          }
          index++;
        }
      }
      bigint_pack_memory(p_dst);
    }
  } else {
    bigint bi;
    bigint_init(&bi);
    bigint_from_int(&bi, value);
    bigint_add_by(p_dst, &bi);
    bigint_release(&bi);
  }
}
void bigint_mul_by_trad(bigint* p_dst, bigint* p_src) {
  if (p_dst == p_src) {
    bigint bi;
    bigint_init(&bi);
    bigint_copy(&bi, p_src);
    bigint_mul_by_trad(p_dst, &bi);
    bigint_release(&bi);
  } else {
    bigint bi;
    bigint bi_add;
    int i;
    bigint_init(&bi);
    bigint_init(&bi_add);
    bigint_set_zero(&bi);
    if (p_src->sign < 0) {
      bigint_change_sign(p_dst);
    }
    for (i = 0; i < p_src->data_len; i++) {
      bigint_copy(&bi_add, p_dst);
      bigint_mul_by_int(&bi_add, p_src->p_data[i]);
      bigint_mul_by_pow_10(&bi_add, i * BIGINT_RADIX_LOG10);
      bigint_add_by(&bi, &bi_add);
    }
    bigint_copy(p_dst, &bi);
    bigint_release(&bi);
    bigint_release(&bi_add);
  }
}
void bigint_mul_by(bigint* p_dst, bigint* p_src) {
  if (p_dst == p_src) {
    bigint bi;
    bigint_init(&bi);
    bigint_copy(&bi, p_src);
    bigint_mul_by(p_dst, &bi);
    bigint_release(&bi);
  } else {
    bigint_mul_by_trad(p_dst, p_src);
  }
}
void bigint_mul_by_int(bigint* p_bigint, int value) {
  if (value == 0) {
    bigint_set_zero(p_bigint);
  } else if (value == 1) {
  } else if (value == -1) {
    bigint_change_sign(p_bigint);
  } else {
    int max_segment_incr = (BIGINT_INT_MAX_DIGITS / BIGINT_RADIX_LOG10) + 1;
    long long prod;
    int index;
    int carry = 0;
    bigint_assure_memory(p_bigint, p_bigint->data_len + max_segment_incr);
    if (value < 0) {
      bigint_change_sign(p_bigint);
      value = -value;
    }
    for (index = 0; index < p_bigint->data_len || carry != 0; index++) {
      if (index >= p_bigint->data_len) {
        p_bigint->p_data[index] = 0;
      }
      prod = (p_bigint->p_data[index] * (long long) value + carry);
      carry = (int) (prod / BIGINT_RADIX);
      p_bigint->p_data[index] = prod % BIGINT_RADIX;
    }
    p_bigint->data_len = index;
  }
}
void bigint_div_by_pow_10(bigint* p_bigint, int pow) {
  if (pow < 0) {
    bigint_mul_by_pow_10(p_bigint, -pow);
  } else if (pow > 0) {
    int throw_segments = pow / BIGINT_RADIX_LOG10;
    int throw_offset = pow % BIGINT_RADIX_LOG10;
    int approx_len = p_bigint->data_len - throw_segments + 1;
    if (p_bigint->data_len <= throw_segments) {
      bigint_set_zero(p_bigint);
    } else {
      int index;
      int prod = 1;
      int trailing = 0;
      int* p_new_data = (int *) BIGINT_ALLOC(sizeof(int) * approx_len);
      if (throw_offset != 0) {
        int i;
        trailing = p_bigint->p_data[throw_segments];
        prod = BIGINT_RADIX;
        for (i = 0; i < throw_offset; i++) {
          trailing /= 10;
          prod /= 10;
        }
        for (index = 0; index + throw_segments + 1 < p_bigint->data_len; index++) {
          p_new_data[index] = p_bigint->p_data[index + throw_segments + 1];
        }
        BIGINT_FREE(p_bigint->p_data);
        p_bigint->data_len -= throw_segments + 1;
        p_bigint->p_data = p_new_data;
        p_bigint->mem_size = approx_len;
        bigint_mul_by_int(p_bigint, prod);
        bigint_add_by_int(p_bigint, trailing);
      } else {
        BIGINT_FREE(p_bigint->p_data);
        p_bigint->data_len -= throw_segments;
        p_bigint->p_data = p_new_data;
        p_bigint->mem_size = approx_len;
      }
    }
  }
}
int bigint_digit_count(bigint* p_bigint) {
  int len;
  if (bigint_is_zero(p_bigint)) {
    assert(p_bigint->data_len == 1 &&
      p_bigint->sign == 0 &&
      p_bigint->p_data[0] == 0);
    len = 1;
  } else {
    int value;
    len = 0;
    len += BIGINT_RADIX_LOG10 * (p_bigint->data_len - 1);
    value = p_bigint->p_data[p_bigint->data_len - 1];
    while (value > 0) {
      len++;
      value /= 10;
    }
  }
  return len;
}
void bigint_mul_by_pow_10(bigint* p_bigint, int pow) {
  if (pow < 0) {
    bigint_div_by_pow_10(p_bigint, -pow);
  } else if (pow > 0) {
    int approx_segments = pow / BIGINT_RADIX_LOG10 + 2 + p_bigint->data_len;
    int* p_new_data = BIGINT_ALLOC(sizeof(int) * (approx_segments));
    int prod;
    int index;
    int helper_var;
    helper_var = pow / BIGINT_RADIX_LOG10;
    for (index = 0; index < helper_var; index++) {
      p_new_data[index] = 0;
    }
    for (index = 0; index < p_bigint->data_len; index++) {
      p_new_data[index + helper_var] = p_bigint->p_data[index];
    }
    p_bigint->data_len += helper_var;
    BIGINT_FREE(p_bigint->p_data);
    p_bigint->p_data = p_new_data;
    p_bigint->mem_size = approx_segments;
    helper_var = pow % BIGINT_RADIX_LOG10;
    prod = 1;
    while (helper_var > 0) {
      prod *= 10;
      helper_var--;
    }
    bigint_mul_by_int(p_bigint, prod);
  }
}
bigint_errno bigint_pow_by_int(bigint* p_bigint, int pow) {
  if (pow < 0) {
    return -BIGINT_ILLEGAL_PARAM;
  } else if (pow == 0) {
    bigint_set_one(p_bigint);
  } else if (pow > 1) {
    if (pow % 2 == 1) {
      bigint bi;
      bigint_init(&bi);
      bigint_copy(&bi, p_bigint);
      bigint_pow_by_int(p_bigint, pow / 2);
      bigint_mul_by(p_bigint, p_bigint);
      bigint_mul_by(p_bigint, &bi);
      bigint_release(&bi);
    } else {
      bigint_pow_by_int(p_bigint, pow / 2);
      bigint_mul_by(p_bigint, p_bigint);
    }
  }
  return BIGINT_NOERR;
}
typedef struct {
  bigint* p_bi;
  int pow;
} bigdec;
void bigdec_init(bigdec* p_bdec) {
  p_bdec->p_bi = (bigint *) malloc(sizeof(bigint));
  bigint_init(p_bdec->p_bi);
  p_bdec->pow = 0;
}
void bigdec_release(bigdec* p_bdec) {
  bigint_release(p_bdec->p_bi);
}
void bigdec_pow(bigdec* p_bdec, int pow) {
  p_bdec->pow *= pow;
  bigint_pow_by_int(p_bdec->p_bi, pow);
}
void bigdec_assign_from_cstr(bigdec* p_bdec, char* cstr) {
  int cstr_len = strlen(cstr);
  char* cstr_cpy = strdup(cstr);
  int dot_idx = cstr_len;
  int init_pow = 0;
  int i;
  for (i = 0; cstr[i] != '\0'; i++) {
    if (cstr[i] == '.') {
      dot_idx = i;
      break;
    }
  }
  for (; cstr_cpy[i] != '\0'; i++) {
    cstr_cpy[i] = cstr_cpy[i + 1];
  }
  init_pow = dot_idx - cstr_len + 1;
  p_bdec->pow = init_pow;
  bigint_from_string(p_bdec->p_bi, cstr_cpy);
}
void kill_trailing(char* cstr) {
  int i;
  int has_dot = 0;
  int end_pos = strlen(cstr);
  for (i = 0; cstr[i] != '\0'; i++) {
    if (has_dot) {
      if (cstr[i] != '0') {
        end_pos = i + 1;
      }
    } else {
      if (cstr[i] == '.') {
        has_dot = 1;
        end_pos = i;
      }
    }
  }
  cstr[end_pos] = '\0';
}
void shift_dot_left(char* cstr) {
  int dot_pos = 0;
  int i;
  int cstr_len = strlen(cstr);
  for (i = 0; cstr[i] != '\0'; i++) {
    if (cstr[i] == '.') {
      dot_pos = i;
      break;
    }
  }
  cstr[dot_pos] = cstr[dot_pos - 1];
  cstr[dot_pos - 1] = '.';
  if (cstr[0] == '.') {
    for (i = cstr_len + 1; i > 0; i--) {
      cstr[i] = cstr[i - 1];
    }
    cstr[0] = '0';
  }
}
void kill_head(char *cstr) {
  int i;
  if (cstr[0] == '0' && cstr[1] == '.') {
    for (i = 0; cstr[i] != '\0'; i++) {
      cstr[i] = cstr[i + 1];
    }
  }
}
void bigdec_convert_to_cstr(bigdec* p_bdec, char* cstr, int len) {
  int idx;
  int i;
  bigint_to_string(p_bdec->p_bi, cstr);
  idx = strlen(cstr);
  cstr[idx] = '.';
  cstr[idx + 1] = 0;
  for (i = 0; i < -p_bdec->pow; i++) {
    shift_dot_left(cstr);
  }
  kill_trailing(cstr);
  kill_head(cstr);
}
int main(int argc, char* argv) {
  char buf[1000];
  char s[10];
  int n;
  bigdec bdec;
  while(scanf("%s%d",s,&n) == 2) {
    bigdec_init(&bdec);
    bigdec_assign_from_cstr(&bdec, s);
    bigdec_pow(&bdec, n);
    bigdec_convert_to_cstr(&bdec, buf, 1000);
    printf("%s\n", buf);
    bigdec_release(&bdec);
  }
  return 0;
}
