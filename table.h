/*-
 * Copyright (c) 1998, Andrew W. Appel with Maia Ginsburg
 * All rights reserved.
 */

/**
 * Implements hash tables using a void pointer as the key and another pointer as
 * the value.
 *
 * For more information, please refer to the tiger book.
 */

#ifndef HASH_H_
#define HASH_H_

struct table;

/**
 * Inserts an entry with @key and @value into the table @t
 */
void table_insert(struct table *t, void *key, void *value);

/**
 * @return the key associated with @key in the able @t if there is any
 */
void *table_find(struct table *t, void *key);

/**
 * @return the last entry in the hashtable @t
 */
void *table_pop(struct table *t);

/**
 * @return an empty hashtable
 */
struct table *table_new_empty(void);

/**
 * Releases all dynamic memory
 */
void table_release(void);

#endif /* end of include guard: HASH_H_ */

