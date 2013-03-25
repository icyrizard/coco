/***   Print tree phase for debugging purposes   ***/

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
struct export {
    char *fun_name; // die sowieso 
    list *args; 
};

struct instr {
    char *name;
    int args[3];
    int num_args;
};

typedef struct instr instruction;

struct INFO {
    int indent;
    list *instrs;  // zoiets
    list *imports;
    list *exports;
};



static info *MakeInfo()
{
    info *result;

    result = MEMmalloc(sizeof(info));
    result->indent = 0;
    result->instrs = list_create();
    result->imports = list_create();
    result->exports = list_create();

    return result;
}


static info *FreeInfo( info *info)
{
    list_free(info->instrs);
    list_free(info->imports);
    list_free(info->exports);

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
    DBUG_ENTER ("ASMassign");

    if (ASSIGN_LET( arg_node) != NULL) {
        ASSIGN_LET( arg_node) = TRAVdo( ASSIGN_LET( arg_node), arg_info);
        printf( " = ");
    }

    ASSIGN_EXPR( arg_node) = TRAVdo( ASSIGN_EXPR( arg_node), arg_info);

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

    printf( "%f", FLOAT_VALUE( arg_node));

    DBUG_RETURN (arg_node);
}

node *ASMnum (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ASMnum");

    printf("%d", NUM_VALUE( arg_node));

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
    DBUG_ENTER ("ASMfundec");

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

// hier was ik begonnnen
node *ASMfundef (node * arg_node, info * arg_info)
{
    type tmp_type;
    char *fun_name;
    node *header, *params;
    instruction *new_instr;

    DBUG_ENTER ("ASMfundef");

    header = FUNDEF_HEADER(arg_node);
    tmp_type = FUNHEADER_RETTYPE(header);
    fun_name = VAR_NAME(FUNHEADER_ID(header));

    /* add to export list */
    if(FUNDEF_EXPORT( arg_node)) {
        struct export *new_export = MEMmalloc(sizeof(struct export));

        /* set function name */
        new_export->fun_name = VAR_NAME(FUNHEADER_ID(header));

        /* create type list */
        new_export->args = list_create();
        list_addtoend(new_export->args, &int_types[FUNHEADER_RETTYPE(header)]);

        params = FUNHEADER_PARAMS(header);
        while(params) {
            tmp_type = PARAM_TYPE(PARAMLIST_HEAD(params));
            list_addtoend(new_export->args,  &int_types[tmp_type]);

            params = PARAMLIST_NEXT(params);
        }
    }

    /* add function label to instrunction list */
    new_instr = MEMmalloc(sizeof(instruction));
    new_instr->name = STRcat(VAR_NAME(FUNHEADER_ID(header)), ":");

    list_addtoend(arg_info->instrs, new_instr);

    /* add esr to instruction list */
    new_instr = MEMmalloc(sizeof(instruction));
    new_instr->name = STRcpy("    esr");   // TODO die spaties moeten weg
    new_instr->num_args = 1;
    new_instr->args[0] = 0;         // TODO num args + num local vars
    list_addtoend(arg_info->instrs, new_instr);
    

    FUNDEF_HEADER( arg_node) = TRAVdo( FUNDEF_HEADER( arg_node), arg_info);


    printf("\n{\n");

    FUNDEF_BODY( arg_node) = TRAVopt( FUNDEF_BODY( arg_node), arg_info);

    printf("}\n\n");

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
        printf("\n");
        print_indent(arg_info->indent);
        printf("return ");

        FUNBODY_RETURN( arg_node) = TRAVdo( FUNBODY_RETURN( arg_node), arg_info);

        printf(";\n");
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

    if((NODE_TYPE(STATEMENTLIST_HEAD(arg_node)) == N_funcall) || (NODE_TYPE(STATEMENTLIST_HEAD(arg_node)) == N_assign) || (NODE_TYPE(STATEMENTLIST_HEAD(arg_node)) == N_dowhileloop))
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


/****************** DEBUG PRINTING **********************/
void print_assembly(info *arg_info)
{
    int i, index = 0;
    instruction *instr;

    instr = list_get_elem(arg_info->instrs, index++);

    while(instr){
        printf("%s ", instr->name);

        /* print args */
        for(i = 0; i < instr->num_args; i++)
            printf("%d ", instr->args[i]);
        printf("\n");
        
        instr = list_get_elem(arg_info->instrs, index++);
    }

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
