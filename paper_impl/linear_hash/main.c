#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#include "linear_hash.h"

size_t hash_func(void *key)
{
  return (size_t)key;
}

int hash_eq(void *key1, void *key2)
{
  return key1 == key2;
}

void hash_free(void *key, void *value)
{
  printf("*** FREE pair (%d %d)\n", (int)key, (int)value);
  // int-int hash, no need to free
}


void print_lhash(lhash_t *lh)
{
  size_t i;
  size_t actual_size;
  hash_entry_t *p;
  printf("entry_count=%d, extend_ptr=%d, extend_level=%d, base_size=%d\n",
    lh->entry_count, lh->extend_ptr, lh->extend_level, lh->base_size);
  actual_size = (lh->base_size << lh->extend_level) + lh->extend_ptr;
  printf("actuall_slots=%d\n", actual_size);
  for (i = 0; i < actual_size; i++) {
    p = lh->slot[i];
    printf("%d:", i);
    while (p != NULL) {
      printf(" (%d,%d)", (int)(p->key), (int)(p->value));
      p = p->next;
    }
    printf("\n");
  }
  printf("\n");
}

int main()
{
  // we make a int-int hash
  lhash_t hash;
  size_t i;

  lhash_init(&hash, hash_func, hash_eq);
  for (i = 0; i < 100; i++) {
    lhash_put(&hash, (void *)i, (void *)(i + 1));
    print_lhash(&hash);
  }
  for (i = 0; i < 100; i++) {
    int val = (int) lhash_get(&hash, (void *)i);
    printf("%d %d\n", val, i);
    if (val != 0)
      assert(val == i + 1);
    print_lhash(&hash);
  }
  for (i = 0; i < 100; i++) {
    lhash_remove(&hash, (void *)i, hash_free);
    print_lhash(&hash);
  }
  assert(hash.entry_count == 0);
  lhash_release(&hash, hash_free);
  
  return 0;
}

