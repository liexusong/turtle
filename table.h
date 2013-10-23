#ifndef HASH_H_
#define HASH_H_

struct table;

void table_insert(struct table *t, void *key, void *value);

void *table_find(struct table *t, void *key);

void *table_pop(struct table *t);

struct table *table_new_empty(void);

#endif /* end of include guard: HASH_H_ */

