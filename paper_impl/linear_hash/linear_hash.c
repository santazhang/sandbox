#include <malloc.h>
#include <stdio.h>

#include "linear_hash.h"

#define ALLOC(ty, n) ((ty *)malloc(sizeof(ty) * (n)))
#define REALLOC(ty, ptr, n) (ty *)realloc(ptr, sizeof(ty) * (n))

void lhash_init(lhash_t *lh, hash_func_t hash, hash_eq_t eq)
{
  size_t i;
  lh->extend_ptr = 0;
  lh->extend_level = 0;
  lh->base_size = LINEAR_HASH_INIT_SLOT_COUNT;
  printf("*************** %d\n", lh->base_size);
  lh->entry_count = 0;
  lh->hash = hash;
  lh->eq = eq;
  lh->slot = ALLOC(hash_entry_t *, lh->base_size);
  for (i = 0; i < lh->base_size; i++) {
    lh->slot[i] = NULL;
  }
}

void lhash_release(lhash_t *lh, hash_free_t hfree)
{
  size_t i;
  size_t actual_size = (lh->base_size << lh->extend_level) + lh->extend_ptr;
  hash_entry_t *p, *q;
  
  for (i = 0; i < actual_size; i++) {
    p = lh->slot[i];
    while (p != NULL) {
      q = p->next;
      hfree(p->key, p->value);
      free(p);
      p = q;
    }
  }
}

// calculates the actuall slot id of a key
static size_t _lhash_slot_id(lhash_t *lh, void *key)
{
  size_t hcode = lh->hash(key);
  if (hcode % (lh->base_size << lh->extend_level) < lh->extend_ptr) {
    // already extended part
    return hcode % (lh->base_size << (1 + lh->extend_level));
  } else {
    // no the yet-not-extended part
    return hcode % (lh->base_size << lh->extend_level);
  }
}


// extend the hash table if necessary
static void _lhash_try_extend(lhash_t *lh)
{
  if (lh->entry_count > LINEAR_HASH_THRESHOLD * (lh->base_size << lh->extend_level)) {
    hash_entry_t *p, *q;
    size_t actual_size = (lh->base_size << lh->extend_level) + lh->extend_ptr;

    printf("*** SHOULD EXPEND\n");

    lh->slot = REALLOC(hash_entry_t *, lh->slot, actual_size + 1);
    p = lh->slot[lh->extend_ptr];
    lh->slot[lh->extend_ptr] = NULL;
    lh->slot[actual_size] = NULL;
    while (p != NULL) {
      size_t hcode = lh->hash(p->key);
      size_t slot_id = hcode % (lh->base_size << (1 + lh->extend_level));
      q = p->next;
      p->next = lh->slot[slot_id];
      lh->slot[slot_id] = p;
      p = q;
    }

    lh->extend_ptr++;
    if (lh->extend_ptr == (lh->base_size << lh->extend_level)) {
      lh->extend_ptr = 0;
      lh->extend_level++;
    }
  }

}

void lhash_put(lhash_t *lh, void *key, void *value)
{
  size_t slot_id;
  hash_entry_t *entry = ALLOC(hash_entry_t, 1);

  // first of all, test if need to expand
  _lhash_try_extend(lh);

  slot_id = _lhash_slot_id(lh, key);

  printf("*** PUT slot id = %d\n", slot_id);

  entry->next = lh->slot[slot_id];
  entry->key = key;
  entry->value = value;
  lh->slot[slot_id] = entry;

  lh->entry_count++;
}



void *lhash_get(lhash_t *lh, void *key)
{
  size_t slot_id = _lhash_slot_id(lh, key);

  printf("*** GET slot id = %d\n", slot_id);

  hash_entry_t *p, *q;
  p = lh->slot[slot_id];
  while (p != NULL) {
    q = p->next;
    if (lh->eq(key, p->key))
      return p->value;
    p = q;
  }
  return NULL;
}

// shrink the hash table if necessary
// table size is shrinked to 1/2
static void _lhash_try_shrink(lhash_t *lh)
{
  if (lh->extend_level == 0)
    return;

  if (lh->entry_count * LINEAR_HASH_THRESHOLD < (lh->base_size << lh->extend_level)) {
    size_t i;
    size_t actual_size = (lh->base_size << lh->extend_level) + lh->extend_ptr;
    size_t new_size = lh->base_size << (lh->extend_level - 1);
    printf("Should shrink");

    for (i = new_size; i < actual_size; i++) {
      size_t slot_id = i % new_size;
      //merge(&lh->slot[i % new_size], lh->slot[i]);
      if (lh->slot[slot_id] == NULL) {
        lh->slot[slot_id] = lh->slot[i];
      } else {
        hash_entry_t *p = lh->slot[slot_id];
        while (p->next != NULL)
          p = p->next;
        p->next = lh->slot[i];
      }
    }

    lh->extend_level--;
    lh->extend_ptr = 0;

    lh->slot = REALLOC(hash_entry_t *, lh->slot, lh->base_size << lh->extend_level);
  }
}

int lhash_remove(lhash_t *lh, void *key, hash_free_t hfree)
{
  size_t slot_id;
  hash_entry_t *p, *q;
  _lhash_try_shrink(lh);
  slot_id = _lhash_slot_id(lh, key);
  p = lh->slot[slot_id];
  if (p == NULL) {
    // head is null, so no element found
    return -1;
  } else if (lh->eq(key, p->key)) {
    // head is object
    lh->slot[slot_id] = lh->slot[slot_id]->next;
    hfree(p->key, p->value);
    free(p);
    lh->entry_count--;
    return 0;
  } else {
    // head is not object
    q = p->next;
    for (;;) {
      // q is the item to be checked
      q = p->next;
      if (q == NULL)
        return -1;
      
      if (lh->eq(key, q->key)) {
        // q is object
        p->next = q->next;
        hfree(q->key, q->value);
        free(q);
        lh->entry_count--;
        return 0;
      }
      p = q;
    }
  }
  return -1;
}

