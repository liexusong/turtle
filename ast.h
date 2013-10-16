#ifndef AST_H_
#define AST_H_

#define MAX_COUNT_CHILDREN 4

enum NODE_TYPE {
    N_PROG,
    N_VAR_DECL,
    N_VAR_DEF,
    N_FUNC_DEF,
    N_STMT,
    N_EXPR,
    N_COMP,
    N_IDENT,
};

enum STMT_TYPE {
    STMT_UP,
    STMT_DOWN,
    STMT_MOVETO,
    STMT_READ,
    STMT_ASSIGN,
    STMT_IFT,
    STMT_IFTE,
    STMT_WHILE,
    STMT_RET,
    STMT_FUNC_CALL,
};

enum EXPR_TYPE {
    EXPR_OP_PLUS,
    EXPR_OP_MINUS,
    EXPR_OP_MULTIPLY,
    EXPR_OP_NEG,
    EXPR_IDENT,
    EXPR_FUNC_CALL,
    EXPR_NUM_LITERAL,
};

enum FUNC_TYPE {
    FUNC_PARAM,
};

enum COMP_TYPE {
    COMP_EQ,
    COMP_LT,
};

struct ast_node {
    enum NODE_TYPE type;
    union {
        enum STMT_TYPE stmt_type;
        enum EXPR_TYPE expr_type;
    } stmt_expr_type;
    struct ast_node *child[MAX_COUNT_CHILDREN];
    struct ast_node *next;
    struct ast_node *tail;
    int lineno;
    int offset;
    void *arbit;
};

typedef struct ast_node *AST;

struct ast_node *ast_insert(struct ast_node *dest, struct ast_node *src);

struct ast_node *ast_new_comp_node(enum COMP_TYPE type, struct ast_node *l,
                                   struct ast_node *r);

struct ast_node *ast_new_arith_node(enum EXPR_TYPE type, struct ast_node *l,
                                    struct ast_node *r);

struct ast_node *ast_new_int_literal_node(int value);

struct ast_node * ast_new_node(enum NODE_TYPE node_type, int type,
                               struct ast_node *child0, struct ast_node *child1,
                               struct ast_node *child2, struct ast_node *child3);

struct ast_node *ast_new_ident_node(char *text);

#define ast_new_node0(e, t)                 ast_new_node(e, t, NULL, NULL, NULL, NULL)
#define ast_new_node1(e, t, a)              ast_new_node(e, t, a, NULL, NULL, NULL)
#define ast_new_node2(e, t, a, b)           ast_new_node(e, t, a, b, NULL, NULL)
#define ast_new_node3(e, t, a, b, c)        ast_new_node(e, t, a, b, c, NULL)
#define ast_new_node4(e, t, a, b, c, d)     ast_new_node(e, t, a, b, c, d)

#endif /* end of include guard: AST_H_ */
