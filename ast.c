#include "ast.h"
#include "dbg.h"

struct ast_node *
ast_new_node(int count_children, enum NODE_TYPE, enum,
             struct ast_node *child0, struct ast_node *child1, struct ast_node *child2)
{
    struct ast_node *node = malloc(sizeof *node);
    check_mem(node);

    node->child[0] = child0;
    node->child[1] = child1;
    node->child[2] = child2;
    node->child[3] = child3;

    node->next = NULL;
    node->tail = node;

    node->lineno = yyget_lineno();
    node->offset  = -1;

    return node;
error:
    return NULL;
}

struct Syntree_node *syntree_insert_last(struct Syntree_node *dest, struct Syntree_node *src)
{
    check(dest && dest->tail && dest->tail->next);
    check(src && src->tail);

    dest->tail->next = src;
    dest->tail = src->tail;

    return dest;

error:
    return NULL;
}
