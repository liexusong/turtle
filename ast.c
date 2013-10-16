#include "global.h"
#include "ast.h"
#include "dbg.h"

struct ast_node *
ast_new_node(enum NODE_TYPE node_type, int type, struct ast_node *child0,
             struct ast_node *child1, struct ast_node *child2,
             struct ast_node *child3)
{
    struct ast_node *node = malloc(sizeof *node);
    check_mem(node);

    node->type = node_type;
    node->stmt_expr_type = type;

    node->child[0] = child0;
    node->child[1] = child1;
    node->child[2] = child2;
    node->child[3] = child3;

    node->next = NULL;
    node->tail = node;

    node->lineno = yyget_lineno();
    node->offset  = -1;
    node->arbit = NULL;

    return node;

error:
    return NULL;
}

struct ast_node *ast_insert_last(struct ast_node *dest, struct ast_node *src)
{
    check(dest && dest->tail && dest->tail->next, "Check your argument");
    check(src && src->tail, "Check your argument");

    dest->tail->next = src;
    dest->tail = src->tail;

    return dest;

error:
    return NULL;
}


struct ast_node *
ast_new_comp_node(enum COMP_TYPE type, struct ast_node *l, struct ast_node *r)
{
    return ast_new_node(N_COMP, type, l, r, NULL, NULL);
}


struct ast_node *
ast_new_arith_node(enum EXPR_TYPE type, struct ast_node *l, struct ast_node *r)
{
    check(type >= 0 && type <= EXPR_OP_NEG, "incorrect type");
    return ast_new_node2(N_EXPR, type, l, r);

error:
    return NULL;
}


struct ast_node *
ast_new_int_literal_node(int value)
{
    struct ast_node *ast = ast_new_node0(N_EXPR, expr_num_literal);
    check(ast, "error");

    ast->arbit = malloc(sizeof value);
    check_mem(ast->arbit);
    *((int *)ast->arbit) = value;

    return ast;

error:
    return NULL;
}

struct ast_node *
ast_new_ident_node(char *text)
{
    struct ast_node *ast = ast_new_node0(N_IDENT, EXPR_NUM_LITERAL);
    check(ast, "error");

    ast->arbit = strdup(text);

    return ast;

error:
    return NULL;
}

