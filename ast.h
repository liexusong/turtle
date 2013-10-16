#ifndef AST_H_
#define AST_H_

#define MAX_COUNT_CHILDREN 4

enum NODE_TYPE {
    K_FUNC,
    K_DEF,
    K_STMT,
    K_EXPR,
};

enum STMT_TYPE {
    K_IFELSE,
    K_WHILE,
    K_RET,
};

enum EXPR_TYPE {
    K_INT,
    K_SYM,
    K_OPR,
    K_CALL,
    K_CALL_STMT,
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
};

#endif /* end of include guard: AST_H_ */
