#ifndef HASH_H_
#define HASH_H_

struct hash_table;

void hash_insert(struct hash_table *t, void *key, void *value);

void *hash_lookup(struct hash_table *t, void *key);

void *hash_pop(struct hash_table *t);

struct hash_table *hash_table_empty(void);

#endif /* end of include guard: HASH_H_ */

