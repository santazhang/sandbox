#ifndef LINEAR_HASH_H_
#define LINEAR_HASH_H_

#define LINEAR_HASH_INIT_SLOT_COUNT 4
#define LINEAR_HASH_THRESHOLD 5

typedef size_t (*hash_func_t)(void *key);
typedef int (*hash_eq_t)(void *key1, void *key2);
typedef void (*hash_free_t)(void *key, void *value);

typedef struct hash_entry_t {
  void *key;
  void *value;

  struct hash_entry_t *next;

} hash_entry_t;

typedef struct lhash_t {

  hash_entry_t **slot;
  
  size_t entry_count; // for calculating load average
  size_t extend_ptr;  // index of next element to be expanded
  size_t extend_level; // how many times the table get expanded
  size_t base_size; // the size of the hash table in the begining
    // current hash table's "appearing" size is base_size * 2 ^ level
    // actuall size could be calculated by: extend_ptr + base_size * 2 ^ level

  hash_func_t hash;
  hash_eq_t eq;

} lhash_t;


void lhash_init(lhash_t *lh, hash_func_t hash, hash_eq_t eq);


void lhash_release(lhash_t *lh, hash_free_t hfree);


void lhash_put(lhash_t *lh, void *key, void *value);


void *lhash_get(lhash_t *lh, void *key);


int lhash_remove(lhash_t *lh, void *key, hash_free_t hfree);


#endif

