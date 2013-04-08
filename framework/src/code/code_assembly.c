/***   Print tree phase for debugging purposes   ***/

#include <stdio.h>
#include "print.h"
#include "traverse.h"
#include "tree_basic.h"
#include "dbug.h"
#include "memory.h"
#include "globals.h"
#include "list_hash.h"
#include "str.h"


#define NELEMS(x)  (sizeof(x) / sizeof(x[0]))

char* typess[5] = { "bool", "int", "float", "void", "unknown" };

/* dit gaat trouens wel fout wss als we het meenemen nar een nieuwe phase of niet?
 * mmm/ nog geen idee hoe we dat uberhaupt moeten doen ja ok no problem dan
 * anders gooien we peepholing en printing ook gwn in deze uber file! */
int int_types[4] = { 0,1,2,3 };

/***********************   INFO   ***********************/
        //    als je hebt: export int main() { ...}
        //    dan wordt het:  .export  "main"  int  main
        //
typedef struct instr instruction;

struct INFO {
    int indent;
    list *instrs;  // zoiets
    list *imports;
    list *exports;
    list *localvars;
    list *constpool;
    node  *root; // root node for the instruction lists
};

static info *MakeInfo()
{
    info *result;

    result = MEMmalloc(sizeof(info));
    result->indent = 0;
    result->instrs  = list_create();
    result->imports = list_create();
    result->exports = list_create();
    result->localvars = list_create();
    result->constpool = list_create();

    return result;
}


static info *FreeInfo( info *info)
{
    list_free(info->instrs);
    list_free(info->imports);
    list_free(info->exports);
    list_free(info->localvars);
    list_free(info->constpool);

    info = MEMfree( info);

    return info;
}

/*****************   Helper Functions   *****************/
void print_indent( int n)
{
    int i = 0;

    for(; i < n; i++)
        printf("    ");
}

bool check_str(void *v1, void *v2)
{

    return STReq((char *)v1, (char *)v2);
}

bool check_const(void *v1, void *v2){
    char *value;
    node *arg, *tmp = (node *)v2;

    /* print args */
    arg = ASSEMBLYINSTR_ARGS(tmp);
    arg = ARGLIST_NEXT(arg);

    /* get last argument(always second)  */
    while(arg){
        value = ARG_INSTR(ARGLIST_HEAD(arg));
        arg = ARGLIST_NEXT(arg);
    }

    return STReq((char *)v1, (char *)value);
}

type get_type(node *decl)
{
    switch(NODE_TYPE(decl))
    {
        case N_globaldec:
            return GLOBALDEC_TYPE(decl);
        case N_globaldef:
            return GLOBALDEF_TYPE(decl);
        case N_vardec:
            return VARDEC_TYPE(decl);
        case N_param:
            return PARAM_TYPE(decl);
        default:
            printf("Unknown node type %d\n", NODE_TYPE(decl));
            return TYPE_unknown;
    }
}

/*********************   Traverse   *********************/
node *ASMprogram(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMprogram");

    PROGRAM_HEAD( arg_node) = TRAVdo( PROGRAM_HEAD( arg_node), arg_info);

    PROGRAM_NEXT( arg_node) = TRAVopt( PROGRAM_NEXT( arg_node), arg_info);

    DBUG_RETURN (arg_node);
}

node *ASMassign (node * arg_node, info * arg_info)
{
    char *tmp = NULL;
    int index;
    DBUG_ENTER ("ASMassign");

    if (ASSIGN_LET( arg_node) != NULL) {
        ASSIGN_LET( arg_node) = TRAVdo( ASSIGN_LET( arg_node), arg_info);
        printf( " = ");
    }

    ASSIGN_EXPR( arg_node) = TRAVdo( ASSIGN_EXPR( arg_node), arg_info);

    switch(get_type(VAR_DECL(ASSIGN_LET(arg_node))))
    {
        case TYPE_int:
            tmp = "istore";
            break;
        case TYPE_float:
            tmp = "fstore";
            break;
        case TYPE_bool:
            tmp = "bstore";
            break;
        default:
            printf("andere type\n");
    }

    index = list_get_index_fun(arg_info->localvars, VAR_NAME(ASSIGN_LET(arg_node)), check_str);
    list_addtoend(arg_info->instrs,
    TBmakeAssemblyinstr(STRcpy(tmp), TBmakeArglist(TBmakeArg( STRitoa(index) ), NULL)));


    DBUG_RETURN (arg_node);
}

node *ASMbinop (node * arg_node, info * arg_info)
{
    char *tmp;

    DBUG_ENTER ("ASMbinop");

    printf( "(");

    BINOP_LEFT( arg_node) = TRAVdo( BINOP_LEFT( arg_node), arg_info);

    switch (BINOP_OP( arg_node)) {
        case BO_add:
            tmp = "+";
            break;
        case BO_sub:
            tmp = "-";
            break;
        case BO_mul:
            tmp = "*";
            break;
        case BO_div:
            tmp = "/";
            break;
        case BO_mod:
            tmp = "%";
            break;
        case BO_lt:
            tmp = "<";
            break;
        case BO_le:
            tmp = "<=";
            break;
        case BO_gt:
            tmp = ">";
            break;
        case BO_ge:
            tmp = ">=";
            break;
        case BO_eq:
            tmp = "==";
            break;
        case BO_ne:
            tmp = "!=";
            break;
        case BO_or:
            tmp = "||";
            break;
        case BO_and:
            tmp = "&&";
            break;
        case BO_unknown:
            DBUG_ASSERT( 0, "unknown binop detected!");
    }

    printf( " %s ", tmp);

    BINOP_RIGHT( arg_node) = TRAVdo( BINOP_RIGHT( arg_node), arg_info);

    printf( ")");

    DBUG_RETURN (arg_node);
}

node *ASMfloat (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMfloat");

    char *instr, *arg, tmp[32];
    int index;
    float number;
    node *args = NULL, *arg_list = NULL, *new_instr = NULL;

    number = FLOAT_VALUE(arg_node);

    switch((int)number)
    {
        case 0:
            instr = STRcpy("floadc_0");
            arg = NULL;
            break;
        case 1:
            instr = STRcpy("floadc_1");
            arg = NULL;
            break;
        default:
            instr = STRcpy("floadc");
            /* copy the constant number in the buffer*/
            sprintf(tmp, "%f",  number);
            printf("tmp %s\n", tmp);

            /* try to find the constant in the buffer */
            index = list_get_index_fun(arg_info->constpool, tmp, check_const);

            /* constant found! add it to the constant list */
            if(index == -1) {
                /* set int as first arg, value as second*/
               arg_list = TBmakeArglist(TBmakeArg("float"), TBmakeArglist(TBmakeArg(STRcpy(tmp)), NULL));

                /* add new constant to constant pool */
                new_instr = TBmakeAssemblyinstr(".const", arg_list);
                list_addtoend(arg_info->constpool, new_instr);

                /* get index of constant in constant pool */
                index = list_length(arg_info->constpool) - 1;
            }

            /* set value of arg to the index where this constant can be found by the VM */
            arg =  STRitoa(index);
            break;
    }

    /* create argument node if needed */
    if(arg)
        args = TBmakeArglist(TBmakeArg(STRitoa(index)), NULL);

    /* add new instruction to list */
    list_addtoend(arg_info->instrs, TBmakeAssemblyinstr(instr, args)); //
    DBUG_RETURN (arg_node);

    DBUG_RETURN (arg_node);
}

node *ASMnum (node * arg_node, info * arg_info)
{
    char *instr, *arg, tmp[32];
    int index, number;
    node *args = NULL, *arg_list = NULL, *new_instr = NULL;

    DBUG_ENTER ("ASMnum");

    number = NUM_VALUE(arg_node);

    switch(number)
    {
        case -1:
            instr = STRcpy("iloadc_m1");
            arg = NULL;
            break;
        case 0:
            instr = STRcpy("iloadc_0");
            arg = NULL;
            break;
        case 1:
            instr = STRcpy("iloadc_1");
            arg = NULL;
            break;
        default:
            instr = STRcpy("iloadc");
            /* copy the constant number in the buffer */
            sprintf(tmp, "%d",  number);

            /* try to find the constant in the buffer */
            index = list_get_index_fun(arg_info->constpool, tmp, check_const);

            /* constant found! add it to the constant list */
            if(index == -1) {
                /* set int as first arg, value as second*/
                arg_list = TBmakeArglist(TBmakeArg("int"),
                        TBmakeArglist(TBmakeArg(STRcpy(tmp)), NULL));

                /* add new constant to constant pool */
                new_instr = TBmakeAssemblyinstr(".const", arg_list);
                list_addtoend(arg_info->constpool, new_instr);

                /* get index of constant in constant pool */
                index = list_length(arg_info->constpool) - 1;
            }

            /* set value of arg to the index where this constant can be found by the VM */
            arg =  STRitoa(index);
            break;
    }

    /* create argument node if needed */
    if(arg)
        args = TBmakeArglist(TBmakeArg(STRitoa(index)), NULL);

    /* add new instruction to list */
    list_addtoend(arg_info->instrs, TBmakeAssemblyinstr(instr, args)); //
    DBUG_RETURN (arg_node);
}

node *ASMbool (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMbool");

    if (BOOL_VALUE( arg_node)) {
        printf( "true");
    }
    else {
        printf( "false");
    }

    DBUG_RETURN (arg_node);
}
node *ASMassemblyinstr(node * arg_node, info * arg_info){
    DBUG_ENTER("ASMinstr");

    DBUG_RETURN(arg_node);
}

node *ASMassemblyinstrs(node * arg_node, info * arg_info){
    DBUG_ENTER("ASMinstrs");

    DBUG_RETURN(arg_node);
}

node *ASMvar (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMvar");

    /* print var as string */
    printf( "%s", VAR_NAME( arg_node));

    DBUG_RETURN (arg_node);
}

node *ASMerror (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMerror");
    DBUG_RETURN (arg_node);
}

node *
ASMmonop (node * arg_node, info * arg_info)
{
    char *tmp;

    DBUG_ENTER ("ASMmonop");

    printf("(");

    switch (MONOP_OP( arg_node)) {
        case MO_not:
            tmp = "!";
            break;
        case MO_neg:
            tmp = "-";
            break;
        case MO_unknown:
            DBUG_ASSERT( 0, "unknown minop detected!");
    }

    printf( " %s ", tmp);

    MONOP_RIGHT( arg_node) = TRAVdo( MONOP_RIGHT( arg_node), arg_info);

    printf(")");

    DBUG_RETURN (arg_node);
}


node *ASMfundec (node * arg_node, info * arg_info)
{
    type tmp_type;
    char *fun_name;
    node *header, *params, *args;

    DBUG_ENTER ("ASMfundec");

    header = FUNDEC_HEADER(arg_node);
    tmp_type = FUNHEADER_RETTYPE(header);
    fun_name = VAR_NAME(FUNHEADER_ID(header));

    /* set function name */
    node *arg_list = TBmakeArglist(TBmakeArg(fun_name), NULL);

    /* set return type */
    ARGLIST_NEXT(arg_list) = TBmakeArglist(TBmakeArg(STRcpy(typess[tmp_type])), NULL);

    /* create type list */
    params = FUNHEADER_PARAMS(header);

    /* create param list  */
    args = ARGLIST_NEXT(arg_list);
    while(params) {
        tmp_type = PARAM_TYPE(PARAMLIST_HEAD(params));
        ARGLIST_NEXT(args) = TBmakeArglist(TBmakeArg(typess[tmp_type]), NULL);
        args = ARGLIST_NEXT(args);
        params = PARAMLIST_NEXT(params);
    }

    /* set funname as last argument */
    ARGLIST_NEXT(args) = TBmakeArglist(TBmakeArg(fun_name), NULL);

    /* add to list */
    node *new_instr = TBmakeAssemblyinstr(".import", arg_list);
    list_addtoend(arg_info->imports, new_instr);

    printf("extern ");

    FUNDEC_HEADER( arg_node) = TRAVdo( FUNDEC_HEADER( arg_node), arg_info);

    printf(";\n");

    DBUG_RETURN (arg_node);
}

node *ASMglobaldec (node * arg_node, info * arg_info)
{
    char* tmp;

    DBUG_ENTER ("ASMglobaldec");

    printf("exern ");

    switch (GLOBALDEC_TYPE( arg_node)) {
      case TYPE_bool:
        tmp = "bool";
        break;
      case TYPE_int:
        tmp = "int";
        break;
      case TYPE_float:
        tmp = "float";
        break;
      case TYPE_void:
        tmp = "void";
        break;
      case TYPE_unknown:
        DBUG_ASSERT( 0, "unknown type detected!");
    }

    printf("%s ", tmp);

    GLOBALDEC_ID( arg_node) = TRAVdo( GLOBALDEC_ID( arg_node), arg_info);

    printf(";\n");

    DBUG_RETURN (arg_node);

}

node *ASMglobaldef (node * arg_node, info * arg_info)
{
    char* tmp;

    DBUG_ENTER ("ASMglobaldef");

    if (GLOBALDEF_EXPORT( arg_node))
        printf("export ");

    switch (GLOBALDEF_TYPE( arg_node)) {
      case TYPE_bool:
        tmp = "bool";
        break;
      case TYPE_int:
        tmp = "int";
        break;
      case TYPE_float:
        tmp = "float";
        break;
      case TYPE_void:
        tmp = "void";
        break;
      case TYPE_unknown:
        DBUG_ASSERT( 0, "unknown type detected!");
    }

    printf("%s ", tmp);

    GLOBALDEF_ID( arg_node) = TRAVdo( GLOBALDEF_ID( arg_node), arg_info);

    if(GLOBALDEF_EXPR( arg_node) != NULL) {
        printf(" = ");
        GLOBALDEF_EXPR( arg_node) = TRAVdo( GLOBALDEF_EXPR( arg_node), arg_info);
    }
    printf(";\n");

    DBUG_RETURN (arg_node);
}

node *ASMcast (node * arg_node, info * arg_info)
{
    char *tmp;

    DBUG_ENTER ("ASMcast");

    printf("((");

    switch (CAST_TYPE( arg_node)) {
      case TYPE_bool:
        tmp = "bool";
        break;
      case TYPE_int:
        tmp = "int";
        break;
      case TYPE_float:
        tmp = "float";
        break;
      case TYPE_void:
        tmp = "void";
        break;
      case TYPE_unknown:
        DBUG_ASSERT( 0, "unknown type detected!");
    }
    printf("%s)", tmp);

    CAST_RIGHT( arg_node) = TRAVdo( CAST_RIGHT( arg_node), arg_info);

    printf(")");

    DBUG_RETURN (arg_node);
}

node *ASMconditionif (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMconditionif");

    print_indent(arg_info->indent);
    printf("if(");

    CONDITIONIF_EXPR( arg_node) = TRAVdo( CONDITIONIF_EXPR( arg_node), arg_info);

    printf(")\n");
    arg_info->indent++;

    CONDITIONIF_BLOCK( arg_node) = TRAVdo( CONDITIONIF_BLOCK( arg_node), arg_info);

    arg_info->indent--;

    if(CONDITIONIF_ELSEBLOCK( arg_node) != NULL) {
        print_indent(arg_info->indent);
        printf("else\n");
        arg_info->indent++;

        CONDITIONIF_ELSEBLOCK( arg_node) = TRAVdo(CONDITIONIF_ELSEBLOCK(\
                    arg_node), arg_info);
        arg_info->indent--;
    }

    DBUG_RETURN (arg_node);
}

node *ASMwhileloop (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMwhileloop");

    printf("while(");

    WHILELOOP_EXPR( arg_node) = TRAVdo( WHILELOOP_EXPR( arg_node), arg_info);

    printf(")\n");

    if(WHILELOOP_BLOCK( arg_node) != NULL) {
        arg_info->indent++;

        WHILELOOP_BLOCK( arg_node) = TRAVdo( WHILELOOP_BLOCK( arg_node), arg_info);

        arg_info->indent--;
    }

    DBUG_RETURN (arg_node);
}

node *ASMdowhileloop (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMdowhileloop");

    printf("do\n");

    arg_info->indent++;
    DOWHILELOOP_BLOCK( arg_node) = TRAVdo( DOWHILELOOP_BLOCK( arg_node), arg_info);

    print_indent(--arg_info->indent);
    printf("while(");

    DOWHILELOOP_EXPR( arg_node) = TRAVdo( DOWHILELOOP_EXPR( arg_node), arg_info);

    printf(")");

    DBUG_RETURN (arg_node);

}

node *ASMforloop (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMforloop");

    printf("for( ");

    FORLOOP_STARTVALUE( arg_node) = TRAVdo( FORLOOP_STARTVALUE( arg_node),
            arg_info);

    printf(", ");

    FORLOOP_STOPVALUE( arg_node) = TRAVdo( FORLOOP_STOPVALUE( arg_node),
                arg_info);

    if(FORLOOP_STEPVALUE( arg_node) != NULL) {
        printf("; ");
        FORLOOP_STEPVALUE( arg_node) = TRAVdo( FORLOOP_STEPVALUE( arg_node),
                arg_info);
    }
    printf(")\n");
    print_indent(arg_info->indent++);
    printf("{\n");
    FORLOOP_BLOCK( arg_node) = TRAVdo( FORLOOP_BLOCK( arg_node), arg_info);
    print_indent(--arg_info->indent);
    printf("}\n");


    DBUG_RETURN (arg_node);
}

node *ASMconst (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMconst");

    printf(" %s ", CONST_NAME( arg_node));

    DBUG_RETURN (arg_node);
}

node *ASMfuncall (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMfuncall");

    FUNCALL_ID( arg_node) = TRAVdo( FUNCALL_ID( arg_node), arg_info);

    printf("(");

    FUNCALL_ARGUMENTS( arg_node) = TRAVopt( FUNCALL_ARGUMENTS( arg_node), arg_info);

    printf(")");


    DBUG_RETURN (arg_node);
}

node *ASMarg(node *arg_node, info *arg_info)
{
    DBUG_ENTER ("ASMarg");
    DBUG_RETURN (arg_node);

}

node *ASMarglist(node *arg_node, info *arg_info)
{
    DBUG_ENTER ("ASMarglist");
    DBUG_RETURN (arg_node);
}

node *ASMfundef (node * arg_node, info * arg_info)
{
    type tmp_type, ret_type;
    char *fun_name, *return_keyword, *tmp_var;
    node *header, *params, *var_decs, *args, *new_instr ;

    DBUG_ENTER ("ASMfundef");

    header = FUNDEF_HEADER(arg_node);
    ret_type = FUNHEADER_RETTYPE(header);
    fun_name = VAR_NAME(FUNHEADER_ID(header));

    /* add to export list */
    if(FUNDEF_EXPORT( arg_node)) {
        /* set function name */
        node *arg_list = TBmakeArglist(TBmakeArg(fun_name), NULL);

        /* set return type */
        ARGLIST_NEXT(arg_list) = TBmakeArglist(TBmakeArg(STRcpy(typess[ret_type])), NULL);

        /* create type list */
        params = FUNHEADER_PARAMS(header);

        /* create param list  */
        args = ARGLIST_NEXT(arg_list);
        while(params) {
            tmp_type = PARAM_TYPE(PARAMLIST_HEAD(params));
            ARGLIST_NEXT(args) = TBmakeArglist(TBmakeArg(typess[tmp_type]), NULL);
            args = ARGLIST_NEXT(args);
            params = PARAMLIST_NEXT(params);
        }

        /* set funname as last argument */
        ARGLIST_NEXT(args) = TBmakeArglist(TBmakeArg(fun_name), NULL);

        /* add to list */
        new_instr = TBmakeAssemblyinstr(".export", arg_list);
        list_addtoend(arg_info->exports, new_instr);
    }


    /* create esr list  */
    params = FUNHEADER_PARAMS(header);
    while(params) {
        tmp_var = VAR_NAME(PARAM_ID(PARAMLIST_HEAD(params)));
        list_addtoend(arg_info->localvars, (void*)tmp_var);
        params = PARAMLIST_NEXT(params);
    }

    var_decs = FUNBODY_VARS(FUNDEF_BODY(arg_node));
    while(var_decs) {
        tmp_var = VAR_NAME(VARDEC_ID(VARDECLIST_HEAD(var_decs)));
        list_addtoend(arg_info->localvars, (void*)tmp_var);
        var_decs = VARDECLIST_NEXT(var_decs);
    }


    list_print_str(arg_info->localvars);

    list_addtoend(arg_info->instrs, TBmakeAssemblyinstr(STRcat(fun_name, ":"), NULL));
    list_addtoend(arg_info->instrs, TBmakeAssemblyinstr(STRcpy("esr"), TBmakeArglist(TBmakeArg(STRitoa(list_length(arg_info->localvars))), NULL)));

    FUNDEF_HEADER( arg_node) = TRAVdo( FUNDEF_HEADER( arg_node), arg_info);

    printf("\n{\n");

    FUNDEF_BODY( arg_node) = TRAVopt( FUNDEF_BODY( arg_node), arg_info);

    switch(ret_type){
        case TYPE_float:
            return_keyword = "freturn";
            break;
        case TYPE_void:
            return_keyword = "return";
            break;
        case TYPE_int:
            return_keyword = "ireturn";
            break;
        case TYPE_bool:
            return_keyword = "breturn";
            break;
        case TYPE_unknown:
            DBUG_ASSERT(0, "Unkown return type");
    }

    list_addtoend(arg_info->instrs, TBmakeAssemblyinstr(STRcpy(return_keyword), NULL));

    printf("}\n\n");

    /* empty localvars list for re-use */
    list_empty(arg_info->localvars);

    DBUG_RETURN (arg_node);
}

node *ASMfunbody (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMfunbody");

    arg_info->indent++;

    if(FUNBODY_VARS( arg_node) != NULL) {
        FUNBODY_VARS( arg_node) = TRAVopt( FUNBODY_VARS( arg_node), arg_info);
        printf("\n");
    }

    if(FUNBODY_STATEMENTS( arg_node) != NULL) {
        FUNBODY_STATEMENTS( arg_node) = TRAVopt( FUNBODY_STATEMENTS( arg_node), arg_info);
    }

    if(FUNBODY_RETURN( arg_node) != NULL) {
        FUNBODY_RETURN( arg_node) = TRAVdo( FUNBODY_RETURN( arg_node), arg_info);

    }

    arg_info->indent--;

    DBUG_RETURN (arg_node);
}

node *ASMexprlist (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMexprlist");

    EXPRLIST_HEAD( arg_node) = TRAVopt( EXPRLIST_HEAD( arg_node), arg_info);

    if(EXPRLIST_NEXT( arg_node) != NULL){
        printf(", ");
        EXPRLIST_NEXT( arg_node) = TRAVdo( EXPRLIST_NEXT( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);
}


node *ASMvardeclist (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMvardeclist");

    VARDECLIST_HEAD( arg_node) = TRAVdo( VARDECLIST_HEAD( arg_node), arg_info);
    printf(";\n");

    VARDECLIST_NEXT( arg_node) = TRAVopt( VARDECLIST_NEXT( arg_node), arg_info);

    DBUG_RETURN (arg_node);
}

node *ASMvardec (node * arg_node, info * arg_info)
{
    char* tmp;

    DBUG_ENTER ("ASMvardec");

    switch (VARDEC_TYPE( arg_node)) {
        case TYPE_bool:
            tmp = "bool";
            break;
        case TYPE_int:
            tmp = "int";
            break;
        case TYPE_float:
            tmp = "float";
            break;
        case TYPE_void:
            tmp = "void";
            break;
        case TYPE_unknown:
            DBUG_ASSERT( 0, "no or unknown type defined");
    }

    print_indent(arg_info->indent);
    printf("%s ", tmp);

    VARDEC_ID( arg_node) = TRAVdo( VARDEC_ID( arg_node), arg_info);

    if(VARDEC_VALUE( arg_node) != NULL) {
        printf(" = ");

        VARDEC_VALUE( arg_node) = TRAVdo( VARDEC_VALUE( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);
}

node *ASMstatementlist (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMstatementlist");

    if(NODE_TYPE(STATEMENTLIST_HEAD(arg_node)) != N_conditionif)
        print_indent( arg_info->indent);
    STATEMENTLIST_HEAD( arg_node) = TRAVdo( STATEMENTLIST_HEAD( arg_node), arg_info);

    if((NODE_TYPE(STATEMENTLIST_HEAD(arg_node)) == N_funcall) || (NODE_TYPE(STATEMENTLIST_HEAD(arg_node)) == N_assign)
            || (NODE_TYPE(STATEMENTLIST_HEAD(arg_node)) == N_dowhileloop))
        printf(";\n");

    STATEMENTLIST_NEXT( arg_node) = TRAVopt( STATEMENTLIST_NEXT( arg_node), arg_info);

    DBUG_RETURN (arg_node);
}

node *ASMfunheader (node * arg_node, info * arg_info)
{
    char* tmp;

    DBUG_ENTER ("ASMfunheader");

    switch (FUNHEADER_RETTYPE( arg_node)) {
        case TYPE_bool:
            tmp = "bool";
            break;
        case TYPE_int:
            tmp = "int";
            break;
        case TYPE_float:
            tmp = "float";
            break;
        case TYPE_void:
            tmp = "void";
            break;
        case TYPE_unknown:
            DBUG_ASSERT( 0, "no or unknown type defined");
    }

    printf("%s ", tmp);

    FUNHEADER_ID( arg_node) = TRAVdo( FUNHEADER_ID( arg_node), arg_info);

    printf("(");

    FUNHEADER_PARAMS( arg_node) = TRAVopt( FUNHEADER_PARAMS( arg_node),
            arg_info);

    printf(")");

    DBUG_RETURN (arg_node);
}

node *ASMparamlist (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMparamlist");

    PARAMLIST_HEAD( arg_node) = TRAVdo( PARAMLIST_HEAD( arg_node), arg_info);

    if(PARAMLIST_NEXT( arg_node) != NULL) {
        printf(", ");
        PARAMLIST_NEXT( arg_node) = TRAVopt( PARAMLIST_NEXT( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);
}

node *ASMparam (node * arg_node, info * arg_info)
{
    char *tmp;

    DBUG_ENTER ("ASMparam");

    switch (PARAM_TYPE( arg_node)) {
        case TYPE_bool:
            tmp = "bool";
            break;
        case TYPE_int:
            tmp = "int";
            break;
        case TYPE_float:
            tmp = "float";
            break;
        case TYPE_void:
            tmp = "void";
            break;
        case TYPE_unknown:
            DBUG_ASSERT( 0, "no or unknown type defined");
    }

    printf("%s ", tmp);
    PARAM_ID( arg_node) = TRAVdo( PARAM_ID( arg_node), arg_info);

    DBUG_RETURN (arg_node);
}

void print_const(list *consts){
    int index = 0;
    node *instr, *arg;
    instr = list_get_elem(consts, index++);

    while(instr){
        printf("%s ", ASSEMBLYINSTR_INSTR(instr));

        /* print args */
        arg = ASSEMBLYINSTR_ARGS(instr);
        printf("%s ", ARG_INSTR(ARGLIST_HEAD(arg)));
        arg = ARGLIST_NEXT(arg);

        while(arg){
          printf("%s ", ARG_INSTR(ARGLIST_HEAD(arg)));
          arg = ARGLIST_NEXT(arg);
        }
        printf("\n");

        instr = list_get_elem(consts, index++);
    }
}

void print_export(list *exports){
    int index = 0;
    node *instr, *arg;
    instr = list_get_elem(exports, index++);

    while(instr){
        printf("%s ", ASSEMBLYINSTR_INSTR(instr));

        /* print args */
        arg = ASSEMBLYINSTR_ARGS(instr);
        printf("\"%s\" ", ARG_INSTR(ARGLIST_HEAD(arg)));
        arg = ARGLIST_NEXT(arg);

        while(arg){
          printf("%s ", ARG_INSTR(ARGLIST_HEAD(arg)));
          arg = ARGLIST_NEXT(arg);
        }
        printf("\n");

        instr = list_get_elem(exports, index++);
    }
}

void print_imports(list *imports){
    int index = 0;
    node *instr, *arg;
    instr = list_get_elem(imports, index++);

    while(instr){
        printf("%s ", ASSEMBLYINSTR_INSTR(instr));

        /* print args */
        arg = ASSEMBLYINSTR_ARGS(instr);
        printf("\"%s\" ", ARG_INSTR(ARGLIST_HEAD(arg)));
        arg = ARGLIST_NEXT(arg);

        while(arg){
          printf("%s ", ARG_INSTR(ARGLIST_HEAD(arg)));
          arg = ARGLIST_NEXT(arg);
        }
        printf("\n");

        instr = list_get_elem(imports, index++);
    }
}

void print_instrs(list *instrs){
    int index = 0;
    node *instr, *arg;
    instr = list_get_elem(instrs, index++);
    char *instr_name;

    while(instr){
        instr_name = ASSEMBLYINSTR_INSTR(instr);
        if(instr_name[STRlen(instr_name) - 1] != ':')
            printf("        ");
        printf("%s ", instr_name);

        /* print args */
        arg = ASSEMBLYINSTR_ARGS(instr);

        while(arg){
            printf("%s ", ARG_INSTR(ARGLIST_HEAD(arg)));
            arg = ARGLIST_NEXT(arg);
        }
        printf("\n");

        instr = list_get_elem(instrs, index++);
    }
    printf("\n");
}

/****************** DEBUG PRINTING **********************/
void print_assembly(info *arg_info)
{
    print_instrs(arg_info->instrs);
    print_imports(arg_info->imports);
    print_export(arg_info->exports);
    print_const(arg_info->constpool);
}

/******************   START of phase   ******************/
node *CODEdoAssembly( node *syntaxtree)
{
    info *info;

    DBUG_ENTER("ASMdoPrint");

    DBUG_ASSERT( (syntaxtree!= NULL), "ASMdoPrint called with empty syntaxtree");

    fprintf(stderr, "\n\n------------------------------\n\n");

    info = MakeInfo();

    TRAVpush( TR_asm);

    syntaxtree = TRAVdo( syntaxtree, info);

    /* DEBUG printing of all instructions */
    printf("\n\nAssembly:\n\n");
    print_assembly(info);

    TRAVpop();

    info = FreeInfo(info);

    fprintf(stderr, "\n\n------------------------------\n\n");

    DBUG_RETURN( syntaxtree);
}
