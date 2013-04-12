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

void print_instrs(list *instrs);
void print_global(list *globals);

#define NELEMS(x)  (sizeof(x) / sizeof(x[0]))

char* typess[5] = { "bool", "int", "float", "void", "unknown" };
char* typesc[5] = { "b", "i", "f", "v", "u" };

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
    list *instrs;  // zoiets
    list *imports;
    list *exports;
    list *globalvars;
    list *localvars;
    list *constpool;
    node  *root; // root node for the instruction lists
    type t;
    int label;
};

static info *MakeInfo()
{
    info *result;

    result = MEMmalloc(sizeof(info));
    result->instrs = list_create();
    result->imports = list_create();
    result->exports = list_create();
    result->globalvars = list_create();
    result->localvars = list_create();
    result->constpool = list_create();
    result->label = 1;

    return result;
}

static info *FreeInfo( info *info)
{
    list_free(info->instrs);
    list_free(info->imports);
    list_free(info->exports);
    list_free(info->localvars);
    list_free(info->constpool);
    list_free(info->globalvars);

    info = MEMfree( info);

    return info;
}

/*****************   Helper Functions   *****************/
bool check_str(void *v1, void *v2)
{
    return STReq((char *)v1, (char *)v2);
}

bool check_const(void *v1, void *v2){
    char *value;
    node *arg, *tmp = (node *)v2;

    /* get last argument(always second)  */
    arg = ASSEMBLYINSTR_ARGS(tmp);
    arg = ARGLIST_NEXT(arg);
    while(arg){
        value = ARG_INSTR(ARGLIST_HEAD(arg));
        arg = ARGLIST_NEXT(arg);
    }

    return STReq((char *)v1, (char *)value);
}

bool check_imports(void *v1, void *v2){
    char *value;
    node *arg, *tmp = (node *)v2;


    /* get last argument(always second)  */
    arg = ASSEMBLYINSTR_ARGS(tmp);
    arg = ARGLIST_NEXT(arg);
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
        case N_funheader:
            return FUNHEADER_RETTYPE(decl);
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
    char *tmp = NULL, *var_name;
    int index = 0;
    type var_type;

    DBUG_ENTER ("ASMassign");

    //if (ASSIGN_LET( arg_node) != NULL)
    //    ASSIGN_LET( arg_node) = TRAVdo( ASSIGN_LET( arg_node), arg_info);

    ASSIGN_EXPR( arg_node) = TRAVdo( ASSIGN_EXPR( arg_node), arg_info);

    var_name = VAR_NAME(ASSIGN_LET(arg_node));
    var_type = get_type(VAR_DECL(ASSIGN_LET(arg_node)));



    if((index = list_get_index_fun(arg_info->localvars, var_name, check_str)) >= 0) {
        switch(var_type)
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
                DBUG_RETURN(arg_node);
        }
    }
    else if((index = list_get_index_fun(arg_info->globalvars, var_name, check_const)) >= 0) {
        /* change according to global stores*/
        switch(var_type)
        {
            case TYPE_int:
                tmp = "istoreg";
                break;
            case TYPE_float:
                tmp = "fstoreg";
                break;
            case TYPE_bool:
                tmp = "bstoreg";
                break;
            default:
                DBUG_RETURN(arg_node);
        }
    }

    list_addtoend(arg_info->instrs,
    TBmakeAssemblyinstr(STRcpy(tmp), TBmakeArglist(TBmakeArg( STRitoa(index) ), NULL)));

    DBUG_RETURN (arg_node);
}

node *ASMbinop (node * arg_node, info * arg_info)
{
    char *first_char, *tmp;
    node *new_instr = NULL;
    type left, right;
    int bool_add_mul = 0;

    DBUG_ENTER ("ASMbinop");

    /* two exprs on top of stack */
    BINOP_LEFT( arg_node) = TRAVdo( BINOP_LEFT( arg_node), arg_info);
    left = arg_info->t;

    BINOP_RIGHT( arg_node) = TRAVdo( BINOP_RIGHT( arg_node), arg_info);
    right = arg_info->t;

    /* get the 'i', 'f', 'b' character used for some assembly instructions */
    first_char = typesc[right];

    switch (BINOP_OP( arg_node)) {
        case BO_add:                /* TODO: 'adding' two bools!!! */
            if(left == TYPE_bool) {
                bool_add_mul = 1;

                new_instr = TBmakeAssemblyinstr(STRcpy("bloadc_t"), NULL);
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcpy("beq"), NULL);
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcpy("branch_f"), TBmakeArglist(TBmakeArg(STRitoa(arg_info->label)), NULL));
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcpy("bpop"), NULL);
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcpy("bloadc_t"), NULL);
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcat(STRitoa(arg_info->label++), ":"), NULL);
                list_addtoend(arg_info->instrs, new_instr);
            } else
                tmp = STRcat(first_char, "add");
            break;
        case BO_sub:
            tmp = STRcat(first_char, "sub");
            break;
        case BO_mul:                /* TODO: 'multiplying' two bools!!! */
            if(left == TYPE_bool) {
                bool_add_mul = 1;

                new_instr = TBmakeAssemblyinstr(STRcpy("bloadc_t"), NULL);
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcpy("beq"), NULL);
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcpy("branch_t"), TBmakeArglist(TBmakeArg(STRitoa(arg_info->label)), NULL));
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcpy("bpop"), NULL);
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcpy("bloadc_f"), NULL);
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcat(STRitoa(arg_info->label++), ":"), NULL);
                list_addtoend(arg_info->instrs, new_instr);
            } else
                tmp = STRcat(first_char, "mul");
            break;
        case BO_div:
            tmp = STRcat(first_char, "div");
            break;
        case BO_mod:
            tmp = STRcat(first_char, "rem");
            break;
        case BO_lt:
            tmp = STRcat(first_char, "lt");
            break;
        case BO_le:
            tmp = STRcat(first_char, "le");
            break;
        case BO_gt:
            tmp = STRcat(first_char, "gt");
            break;
        case BO_ge:
            tmp = STRcat(first_char, "ge");
            break;
        case BO_eq:
            tmp = STRcat(first_char, "eq");
            break;
        case BO_ne:
            tmp = STRcat(first_char, "ne");
            break;
        case BO_or:         /* 'or' and 'and' should not exist anymore */
        case BO_and:
        case BO_unknown:
            DBUG_ASSERT( 0, "unknown binop detected!");
    }

    /* add new instruction to list */
    if(!bool_add_mul) {
        new_instr = TBmakeAssemblyinstr(tmp, NULL);
        list_addtoend(arg_info->instrs, new_instr);
    }

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
    arg_info->t = TYPE_float;

    if(number == 0.0) {
        instr = STRcpy("floadc_0");
        arg = NULL;
    } else if(number == 1.0) {
        instr = STRcpy("floadc_1");
        arg = NULL;
    } else {
        instr = STRcpy("floadc");
        /* copy the constant number in the buffer*/
        sprintf(tmp, "%f",  number);
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
    }

    /* create argument node if needed */
    if(arg) {
        args = TBmakeArglist(TBmakeArg(STRitoa(index)), NULL);
    }

    /* add new instruction to list */
    list_addtoend(arg_info->instrs, TBmakeAssemblyinstr(instr, args)); //
    DBUG_RETURN (arg_node);
}

node *ASMnum (node * arg_node, info * arg_info)
{
    char *instr, *arg, tmp[32];
    int index, number;
    node *args = NULL, *arg_list = NULL, *new_instr = NULL;

    DBUG_ENTER ("ASMnum");

    number = NUM_VALUE(arg_node);
    arg_info->t = TYPE_int;

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
                /* set int as first arg, value as second */
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
    list_addtoend(arg_info->instrs, TBmakeAssemblyinstr(instr, args));
    DBUG_RETURN (arg_node);
}

node *ASMbool (node * arg_node, info * arg_info)
{
    node *new_instr;
    DBUG_ENTER ("ASMbool");

    arg_info->t = TYPE_bool;

    if (BOOL_VALUE( arg_node))
        new_instr = TBmakeAssemblyinstr("bloadc_t", NULL);
    else
        new_instr = TBmakeAssemblyinstr("bloadc_f", NULL);

    list_addtoend(arg_info->instrs, new_instr);
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
    int index;
    char *tmp = NULL, *var_name = NULL;
    type var_type;

    var_name = VAR_NAME(arg_node);
    var_type = get_type(VAR_DECL(arg_node));
    arg_info->t  = var_type;

    switch(var_type)
    {
        case TYPE_int:
            tmp = "iload";
            break;
        case TYPE_float:
            tmp = "fload";
            break;
        case TYPE_bool:
            tmp = "bload";
            break;
        default:
            DBUG_RETURN(arg_node);
    }

    if((index = list_get_index_fun(arg_info->localvars, var_name, check_str)) >= 0) {

    /* een variabele vinden in de constant pool lijkt mij niet heel nuttig */
    //} else if((index = list_get_index_fun(arg_info->constpool, var_name, check_const)) >= 0) {

    } else if((index = list_get_index_fun(arg_info->globalvars, var_name, check_const)) >= 0) {

        /* change according to global loads*/
        switch(var_type)
        {
            case TYPE_int:
                tmp = "iloadg";
                break;
            case TYPE_float:
                tmp = "floadg";
                break;
            case TYPE_bool:
                tmp = "bloadg";
                break;
            default:
                DBUG_RETURN(arg_node);
        }
    }

    //if (index != -1){
    list_addtoend(arg_info->instrs,
    TBmakeAssemblyinstr(STRcpy(tmp), TBmakeArglist(TBmakeArg( STRitoa(index) ), NULL)));
    //}

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
    node *new_instr;

    DBUG_ENTER ("ASMmonop");

    MONOP_RIGHT( arg_node) = TRAVdo( MONOP_RIGHT( arg_node), arg_info);

    switch (MONOP_OP( arg_node)) {
        case MO_not:
            tmp = STRcpy("bnot");
            break;
        case MO_neg:
            tmp = STRcat(typesc[arg_info->t], "neg");
            break;
        case MO_unknown:
            DBUG_ASSERT( 0, "unknown minop detected!");
    }

    /* add new instruction to list */
    new_instr = TBmakeAssemblyinstr(tmp, NULL);
    list_addtoend(arg_info->instrs, new_instr);

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


    FUNDEC_HEADER( arg_node) = TRAVdo( FUNDEC_HEADER( arg_node), arg_info);

    DBUG_RETURN (arg_node);
}

node *ASMglobaldec (node * arg_node, info * arg_info)
{
    int index;
    char* tmp, *var;
    node *new_instr, *arg_list;

    DBUG_ENTER ("ASMglobaldec");

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

    /* get index of constant in constant pool */
    index = list_length(arg_info->globalvars) - 1;

    //GLOBALDEC_ID( arg_node) = TRAVdo( GLOBALDEC_ID( arg_node), arg_info);
    var = VAR_NAME(GLOBALDEC_ID( arg_node));

    /* create argument list */
    arg_list = TBmakeArglist(TBmakeArg(tmp), TBmakeArglist(TBmakeArg(STRcpy(var)), NULL));

    /* add new constant to constant pool */
    new_instr = TBmakeAssemblyinstr(".global", arg_list);
    list_addtoend(arg_info->globalvars, new_instr);

    DBUG_RETURN (arg_node);
}

node *ASMglobaldef (node * arg_node, info * arg_info)
{
    int index;
    char* tmp, *var = NULL;
    node *new_instr, *arg_list;

    DBUG_ENTER ("ASMglobaldef");

    //if (GLOBALDEF_EXPORT( arg_node))
    //

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

    /* no need to traverse the ID */
    //GLOBALDEF_ID( arg_node) = TRAVdo( GLOBALDEF_ID( arg_node), arg_info);

    /* copy varname to globaldef */
    var = STRcpy(VAR_NAME(GLOBALDEF_ID( arg_node)));
    index = list_length(arg_info->globalvars) - 1;

    /* create argument list */
    arg_list = TBmakeArglist(TBmakeArg(tmp), TBmakeArglist(TBmakeArg(STRcpy(var)), NULL));

    /* add new constant to constant pool */
    new_instr = TBmakeAssemblyinstr(".global", arg_list);
    list_addtoend(arg_info->globalvars, new_instr);

    if(GLOBALDEF_EXPR( arg_node) != NULL)
        GLOBALDEF_EXPR( arg_node) = TRAVdo( GLOBALDEF_EXPR( arg_node), arg_info);

    DBUG_RETURN (arg_node);
}

node *ASMcast (node * arg_node, info * arg_info)
{
    node *new_instr;
    type cast;

    DBUG_ENTER ("ASMcast");

    cast = CAST_TYPE(arg_node);

    TRAVdo( CAST_RIGHT( arg_node), arg_info);

    switch(cast) {
        /* Casting to int */
        case TYPE_int:
            if(arg_info->t == TYPE_float) {
                new_instr = TBmakeAssemblyinstr(STRcpy("f2i"), NULL);
                list_addtoend(arg_info->instrs, new_instr);
            } else if(arg_info->t == TYPE_bool) {
                /* casting a bool to int */

                /*  (bool) b1
                 *  [false
                 *
                 */
                new_instr = TBmakeAssemblyinstr(STRcpy("bloadc_t"), NULL);
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcpy("beq"), NULL);
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcpy("branch_f"), TBmakeArglist(TBmakeArg(STRitoa(arg_info->label)), NULL));
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcpy("iloadc_1"), NULL);
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcpy("jump"), TBmakeArglist(TBmakeArg(STRitoa(arg_info->label + 1)), NULL));
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcat(STRitoa(arg_info->label++), ":"), NULL);
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcpy("iloadc_0"), NULL);
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcat(STRitoa(arg_info->label++), ":"), NULL);
                list_addtoend(arg_info->instrs, new_instr);
            }
            break;

        /* Casting to float */
        case TYPE_float:
            if(arg_info->t == TYPE_int) {
                new_instr = TBmakeAssemblyinstr(STRcpy("i2f"), NULL);
                list_addtoend(arg_info->instrs, new_instr);
            } else if(arg_info->t == TYPE_bool) {
                /* casting a bool to float */
                new_instr = TBmakeAssemblyinstr(STRcpy("bloadc_t"), NULL);
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcpy("beq"), NULL);
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcpy("branch_f"), TBmakeArglist(TBmakeArg(STRitoa(arg_info->label)), NULL));
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcpy("floadc_1"), NULL);
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcpy("jump"), TBmakeArglist(TBmakeArg(STRitoa(arg_info->label+1)), NULL));
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcat(STRitoa(arg_info->label++), ":"), NULL);
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcpy("floadc_0"), NULL);
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcat(STRitoa(arg_info->label++), ":"), NULL);
                list_addtoend(arg_info->instrs, new_instr);
            }
            break;

        /* Casting to bool */
        case TYPE_bool:
            if(arg_info->t == TYPE_int) {
                /* casting a int to bool */
                new_instr = TBmakeAssemblyinstr(STRcpy("iloadc_0"), NULL);
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcpy("ineq"), NULL);
                list_addtoend(arg_info->instrs, new_instr);
            } else if(arg_info->t == TYPE_float) {
                /* casting a float to bool */
                new_instr = TBmakeAssemblyinstr(STRcpy("floadc_0"), NULL);
                list_addtoend(arg_info->instrs, new_instr);

                new_instr = TBmakeAssemblyinstr(STRcpy("fneq"), NULL);
                list_addtoend(arg_info->instrs, new_instr);
            }
            break;
        default:
            break;

    }

    /* the type of the result of the expression is the cast type */
    arg_info->t = cast;

    DBUG_RETURN (arg_node);
}

node *ASMconditionif (node * arg_node, info * arg_info)
{
    node *new_instr;
    int old, old2;

    DBUG_ENTER ("ASMconditionif");

    CONDITIONIF_EXPR( arg_node) = TRAVdo( CONDITIONIF_EXPR( arg_node), arg_info);
    old = arg_info->label;

    new_instr = TBmakeAssemblyinstr(STRcpy("branch_f"), TBmakeArglist(TBmakeArg(STRitoa(arg_info->label++)), NULL));
    list_addtoend(arg_info->instrs, new_instr);

    CONDITIONIF_BLOCK( arg_node) = TRAVdo( CONDITIONIF_BLOCK( arg_node), arg_info);

    old2 = arg_info->label;
    new_instr = TBmakeAssemblyinstr(STRcpy("jump"), TBmakeArglist(TBmakeArg(STRitoa(arg_info->label++)), NULL));
    list_addtoend(arg_info->instrs, new_instr);

    new_instr = TBmakeAssemblyinstr(STRcat(STRitoa(old), ":"), NULL);
    list_addtoend(arg_info->instrs, new_instr);

    if(CONDITIONIF_ELSEBLOCK( arg_node) != NULL) {
        CONDITIONIF_ELSEBLOCK( arg_node) = TRAVdo(CONDITIONIF_ELSEBLOCK(\
                    arg_node), arg_info);
    }

    new_instr = TBmakeAssemblyinstr(STRcat(STRitoa(old2), ":"), NULL);
    list_addtoend(arg_info->instrs, new_instr);

    DBUG_RETURN (arg_node);
}

node *ASMwhileloop (node * arg_node, info * arg_info)
{
    node *new_instr;
    int label_before, label_after;

    DBUG_ENTER ("ASMwhileloop");


    label_before = arg_info->label;
    new_instr = TBmakeAssemblyinstr(STRcat(STRitoa(arg_info->label++), ":"), NULL);
    list_addtoend(arg_info->instrs, new_instr);


    WHILELOOP_EXPR( arg_node) = TRAVdo( WHILELOOP_EXPR( arg_node), arg_info);


    label_after = arg_info->label;
    new_instr = TBmakeAssemblyinstr(STRcpy("branch_f"), TBmakeArglist(TBmakeArg(STRitoa(arg_info->label++)), NULL));
    list_addtoend(arg_info->instrs, new_instr);

    if(WHILELOOP_BLOCK( arg_node) != NULL) {
        WHILELOOP_BLOCK( arg_node) = TRAVdo( WHILELOOP_BLOCK( arg_node), arg_info);
    }

    new_instr = TBmakeAssemblyinstr(STRcpy("jump"), TBmakeArglist(TBmakeArg(STRitoa(label_before)), NULL));
    list_addtoend(arg_info->instrs, new_instr);

    new_instr = TBmakeAssemblyinstr(STRcat(STRitoa(label_after), ":"), NULL);
    list_addtoend(arg_info->instrs, new_instr);

    DBUG_RETURN (arg_node);
}

node *ASMdowhileloop (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMdowhileloop");


    DOWHILELOOP_BLOCK( arg_node) = TRAVdo( DOWHILELOOP_BLOCK( arg_node), arg_info);


    DOWHILELOOP_EXPR( arg_node) = TRAVdo( DOWHILELOOP_EXPR( arg_node), arg_info);


    DBUG_RETURN (arg_node);

}

node *ASMforloop (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMforloop");


    FORLOOP_STARTVALUE( arg_node) = TRAVdo( FORLOOP_STARTVALUE( arg_node),
            arg_info);


    FORLOOP_STOPVALUE( arg_node) = TRAVdo( FORLOOP_STOPVALUE( arg_node),
                arg_info);

    if(FORLOOP_STEPVALUE( arg_node) != NULL)
        FORLOOP_STEPVALUE( arg_node) = TRAVdo( FORLOOP_STEPVALUE( arg_node), arg_info);

    FORLOOP_BLOCK( arg_node) = TRAVdo( FORLOOP_BLOCK( arg_node), arg_info);


    DBUG_RETURN (arg_node);
}

node *ASMconst (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMconst");

    DBUG_RETURN (arg_node);
}

node *ASMfuncall (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMfuncall");
    char *fun_name = NULL;
    int num_args = 0, index = 0;
    node *arg_list, *new_instr, *params;

    /* initiate function call*/
    fun_name = STRcpy(VAR_NAME(FUNCALL_ID(arg_node)));

    /* arg_list with num of argumenst and fun name */
    new_instr = TBmakeAssemblyinstr("isrg", NULL);
    list_addtoend(arg_info->instrs, new_instr);

    params = FUNCALL_ARGUMENTS( arg_node) = TRAVopt( FUNCALL_ARGUMENTS( arg_node), arg_info);

    /* create param list */
    while(params) {
        params = EXPRLIST_NEXT(params);
        num_args++;
    }

    if((index = list_get_index_fun(arg_info->imports, fun_name, check_const)) >= 0){
        arg_list = TBmakeArglist(TBmakeArg(STRitoa(index)), NULL);
        new_instr = TBmakeAssemblyinstr("jsre", arg_list);
    } else {
        arg_list = TBmakeArglist(TBmakeArg(STRitoa(num_args)),
                        TBmakeArglist(TBmakeArg(fun_name), NULL));
        new_instr = TBmakeAssemblyinstr("jsr", arg_list);
    }

    /* add new instruction to list */
    list_addtoend(arg_info->instrs, new_instr);

    /* set type in info to return type */
    arg_info->t = FUNHEADER_RETTYPE(VAR_DECL(FUNCALL_ID(arg_node)));

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
        list_addtoend(arg_info->localvars, tmp_var);
        params = PARAMLIST_NEXT(params);
    }

    if(FUNDEF_BODY(arg_node) != NULL) {
        var_decs = FUNBODY_VARS(FUNDEF_BODY(arg_node));

        while(var_decs) {
            tmp_var = VAR_NAME(VARDEC_ID(VARDECLIST_HEAD(var_decs)));
            list_addtoend(arg_info->localvars, tmp_var);
            var_decs = VARDECLIST_NEXT(var_decs);
        }
    }

    list_addtoend(arg_info->instrs, TBmakeAssemblyinstr(STRcat(fun_name, ":"), NULL));
    list_addtoend(arg_info->instrs, TBmakeAssemblyinstr(STRcpy("esr"), TBmakeArglist(TBmakeArg(STRitoa(list_length(arg_info->localvars))), NULL)));

    FUNDEF_HEADER( arg_node) = TRAVdo( FUNDEF_HEADER( arg_node), arg_info);


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

    /* empty localvars list for re-use */
    list_empty(arg_info->localvars);

    DBUG_RETURN (arg_node);
}

node *ASMfunbody (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMfunbody");


    if(FUNBODY_VARS( arg_node) != NULL)
        FUNBODY_VARS( arg_node) = TRAVopt( FUNBODY_VARS( arg_node), arg_info);

    if(FUNBODY_STATEMENTS( arg_node) != NULL) {
        FUNBODY_STATEMENTS( arg_node) = TRAVopt( FUNBODY_STATEMENTS( arg_node), arg_info);
    }

    if(FUNBODY_RETURN( arg_node) != NULL) {
        FUNBODY_RETURN( arg_node) = TRAVdo( FUNBODY_RETURN( arg_node), arg_info);

    }


    DBUG_RETURN (arg_node);
}

node *ASMexprlist (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMexprlist");

    EXPRLIST_HEAD( arg_node) = TRAVopt( EXPRLIST_HEAD( arg_node), arg_info);

    if(EXPRLIST_NEXT( arg_node) != NULL){
        EXPRLIST_NEXT( arg_node) = TRAVdo( EXPRLIST_NEXT( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);
}


node *ASMvardeclist (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMvardeclist");

    VARDECLIST_HEAD( arg_node) = TRAVdo( VARDECLIST_HEAD( arg_node), arg_info);

    VARDECLIST_NEXT( arg_node) = TRAVopt( VARDECLIST_NEXT( arg_node), arg_info);

    DBUG_RETURN (arg_node);
}

node *ASMvardec (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMvardec");
  // hmm do we need this? don't think so
  //  VARDEC_ID( arg_node) = TRAVdo( VARDEC_ID( arg_node), arg_info);

  //  if(VARDEC_VALUE( arg_node) != NULL) {
    //    VARDEC_VALUE( arg_node) = TRAVdo( VARDEC_VALUE( arg_node), arg_info);
  //  }

    DBUG_RETURN (arg_node);
}

node *ASMstatementlist (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMstatementlist");

    STATEMENTLIST_HEAD( arg_node) = TRAVdo( STATEMENTLIST_HEAD( arg_node), arg_info);

    if((NODE_TYPE(STATEMENTLIST_HEAD(arg_node)) == N_funcall) || (NODE_TYPE(STATEMENTLIST_HEAD(arg_node)) == N_assign)
            || (NODE_TYPE(STATEMENTLIST_HEAD(arg_node)) == N_dowhileloop))

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

//  FUNHEADER_ID( arg_node) = TRAVdo( FUNHEADER_ID( arg_node), arg_info);


 //  FUNHEADER_PARAMS( arg_node) = TRAVopt( FUNHEADER_PARAMS( arg_node),
            //arg_info);


    DBUG_RETURN (arg_node);
}

node *ASMparamlist (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMparamlist");

    PARAMLIST_HEAD( arg_node) = TRAVdo( PARAMLIST_HEAD( arg_node), arg_info);

    if(PARAMLIST_NEXT( arg_node) != NULL)
        PARAMLIST_NEXT( arg_node) = TRAVopt( PARAMLIST_NEXT( arg_node), arg_info);

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

    PARAM_ID( arg_node) = TRAVdo( PARAM_ID( arg_node), arg_info);

    DBUG_RETURN (arg_node);
}

void print_const(list *consts){
    int index = 0;
    node *instr, *arg;
    instr = list_get_elem(consts, index++);

    while(instr) {
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


/****************** PEEPHOLING **********************/
list *peephole(list *instrs)
{
    list *prev, *curr;
    node *first, *second, *args1, *args2;
    char *instr1, *instr2;
    int change = 1;

    /* continue peeholing as long as something changes */
    while(change) {
        change = 0;

        /* initialize prev and curr to the first 2 instrutions */
        prev = instrs->next;
        curr = prev->next;

        /* go though the whole list of instructions */
        while(curr) {
            /* get the instruction nodes */
            first = (node *)prev->value;
            second = (node *)curr->value;

            /* get instruction strings */
            instr1 = ASSEMBLYINSTR_INSTR(first);
            instr2 = ASSEMBLYINSTR_INSTR(second);

            /* get argument nodes */
            args1 = ASSEMBLYINSTR_ARGS(first);
            args2 = ASSEMBLYINSTR_ARGS(second);

            /* [.., load 0, store 0, ..] -> [.., ..] */
            if(STReq(instr1 + 1, "load") &&
                    STReq(instr2 + 1, "store") &&
                    STReq(ARG_INSTR(ARGLIST_HEAD(args1)), ARG_INSTR(ARGLIST_HEAD(args2)))) {
                change = 1;

                /* this is perfectly find as there will always be a init
                 * function after this */
                prev = curr->next;
                curr = prev->next;

                /* remove the insructions from the list */
                list_remove(instrs, first);
                list_remove(instrs, second);

                continue;
            } else if(STReq(instr1 + 1, "loadg") &&
                    STReq(instr2 + 1, "storeg") &&
                    STReq(ARG_INSTR(ARGLIST_HEAD(args1)), ARG_INSTR(ARGLIST_HEAD(args2)))) {
                change = 1;

                /* this is perfectly fine as there will alwasy be a return
                 * instruction after istoreg instructions in the __init
                 * function */
                prev = curr->next;
                curr = prev->next;

                /* remove the insructions from the list */
                list_remove(instrs, first);
                list_remove(instrs, second);

                continue;
            }
            prev = curr;
            curr = curr->next;
        }
    }



    return instrs;
}


/****************** ASSEMBLY PRINTING **********************/
void print_export(list *exports){
    int index = 0;
    node *instr, *arg;
    instr = list_get_elem(exports, index++);

    while(instr) {
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

        while(arg) {
          printf("%s ", ARG_INSTR(ARGLIST_HEAD(arg)));
          arg = ARGLIST_NEXT(arg);
        }
        printf("\n");

        instr = list_get_elem(imports, index++);
    }
}

void print_global(list *globals){
    int index = 0;
    node *instr, *arg;
    instr = list_get_elem(globals, index++);

    while(instr) {
        printf("%s ", ASSEMBLYINSTR_INSTR(instr));

        /* print args */
        arg = ASSEMBLYINSTR_ARGS(instr);
        printf("%s ", ARG_INSTR(ARGLIST_HEAD(arg)));
        printf("\n");
        instr = list_get_elem(globals, index++);
    }
    printf("\n");
}

void print_instrs(list *instrs){
    int index = 0;
    char *instr_name;
    node *instr, *arg;
    instr = list_get_elem(instrs, index++);

    while(instr){
        instr_name = ASSEMBLYINSTR_INSTR(instr);
        if(instr_name[STRlen(instr_name) - 1] != ':')
            printf("        ");
        //else
        //    printf("    ");
        printf("%s ", instr_name);

        if(STReq(instr_name, "return") || STReq(instr_name+1, "return"))
            printf("\n");

        /* print args */
        arg = ASSEMBLYINSTR_ARGS(instr);
        while(arg) {
            printf("%s ", ARG_INSTR(ARGLIST_HEAD(arg)));
            arg = ARGLIST_NEXT(arg);
        }
        printf("\n");

        instr = list_get_elem(instrs, index++);
    }
    printf("\n");
}

void print_assembly(info *arg_info)
{
    print_instrs(arg_info->instrs);
    print_imports(arg_info->imports);
    print_export(arg_info->exports);
    print_global(arg_info->globalvars);
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

    /* generate assembly code */
    syntaxtree = TRAVdo( syntaxtree, info);

    /* peephole the assembly code */
    info->instrs = peephole(info->instrs);


    /* print assembly code */
    print_assembly(info);

    TRAVpop();

    info = FreeInfo(info);

    fprintf(stderr, "\n\n------------------------------\n\n");

    DBUG_RETURN( syntaxtree);
}
