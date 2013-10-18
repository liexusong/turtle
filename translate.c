void
do_translate(struct ast_node *node)
{
    switch (node->type) {
        case N_PROG:
            translateGVDefs(node->child[1]);
            translateFuncDefs(node->child[2]);
            translateBody(node->child[3]);
            break;
        case N_VAR_DEF:
            break;
        case N_FUNC_DEF:
            break;
        case N_STMT:
            break;
        case N_EXPR:
            break;
        case N_COMP:
            break;
        case N_IDENT:
            break;
    }
}

void
translate_global_var_defs(struct ast_node *node)
{
    while (node != NULL) {
        translate_global_var_def(node);
        node = node->next;
    }
}

void
translate_global_var_def(struct ast_node *node)
{
    symtbl_new_global_var(node);
}


void
translate_func_defs(struct ast_node *node)
{
    while (node != NULL) {
        node = node->next;
    }
}


void
translate_func_def(struct ast_node *node)
{
    symtbl_new_func(node);
}

void
translate_compound_statements(struct ast_node *node)
{
    while (node != NULL) {
        translate_statement(node);
        node = node->next;
    }
}

void
translate_statement(struct ast_node *node)
{
    switch (node->stmt_expr_type->stmt_type) {
        case STMT_UP:
            break;
        case STMT_DOWN:
            break;
    }
}

void
translate_expression(struct ast_node *node)
{
    switch (node->stmt_expr_type->expr_type) {
    }
}
