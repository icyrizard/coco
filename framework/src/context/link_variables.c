#include "types.h"
#include "link_variables.h"
#include "memory.h"
#include "dbug.h"
#include "tree_basic.h"
#include "traverse.h"
#include "globals.h"
#include "str.h"
#include "ctinfo.h"
#include "list_hash.h"



/***********************   INFO   ***********************/
struct INFO {
    int nest_level;

    hashmap *local;
    hashmap *global;
};

static info *MakeInfo()
{
    info *result;

    result = MEMmalloc(sizeof(info));

    result->nest_level = 0;
    result->local = hashmap_create();
    result->global = hashmap_create();

    return result;
}

static info *FreeInfo( info *info)
{
    hashmap_free(info->global);
    hashmap_free(info->local);

    info = MEMfree( info);

    return info;
}


/******************   Help Functions   ******************/



/*********************   Traverse   *********************/
node *LVARglobaldec( node *arg_node, info *arg_info)
{
    char *var_name;

    DBUG_ENTER("LVARglobaldec");

    var_name = VAR_NAME(GLOBALDEC_ID( arg_node));

    /* set declaration of this global variable */
    VAR_DECL(GLOBALDEC_ID( arg_node) ) = arg_node;

    /* check for duplicated variable declaration */
    if(hashmap_contains(arg_info->global, var_name)) {
        CTIerror(":%d: error: '%s' has already been defined in this context",
                NODE_LINE(arg_node), var_name);

        CTIerror(":%d: error: location of earlier definition",
                NODE_LINE((node *)hashmap_get(arg_info->global, var_name)));
        DBUG_RETURN(arg_node);
    }

    /* add global variabele declaration to hashmap */
    hashmap_add(arg_info->global, var_name, arg_node);

    DBUG_RETURN( arg_node);
}

node *LVARglobaldef( node *arg_node, info *arg_info)
{
    char *var_name;

    DBUG_ENTER("LVARglobaldef");

    var_name = VAR_NAME(GLOBALDEF_ID( arg_node));

    /* set declaration of this global variable */
    VAR_DECL(GLOBALDEF_ID( arg_node) ) = arg_node;

    /* check for duplicated variable declaration */
    if(hashmap_contains(arg_info->global, var_name)) {
        CTIerror(":%d: error: '%s' has already been defined in this context",
                NODE_LINE(arg_node), var_name);

        CTIerror(":%d: error: location of earlier definition",
                NODE_LINE((node *)hashmap_get(arg_info->global, var_name)));
        DBUG_RETURN(arg_node);
    }

    /* add global variabele declaration to hashmap */
    hashmap_add(arg_info->global, var_name, arg_node);

    DBUG_RETURN( arg_node);
}

node *LVARfundef( node *arg_node, info *arg_info)
{
    DBUG_ENTER("LVARfundef");

    arg_info->nest_level++;

    FUNDEF_HEADER(arg_node)  = TRAVopt( FUNDEF_HEADER( arg_node), arg_info);

    FUNDEF_BODY(arg_node)  = TRAVopt( FUNDEF_BODY( arg_node), arg_info);

    /* empty hashmap of local variables */
    hashmap_empty(arg_info->local);

    arg_info->nest_level--;

    DBUG_RETURN( arg_node);
}

node *LVARfunheader(node *arg_node, info *arg_info)
{
    DBUG_ENTER("LVARfunheader");

    FUNHEADER_PARAMS(arg_node) = TRAVopt(FUNHEADER_PARAMS(arg_node), arg_info);

    DBUG_RETURN( arg_node);
}

node *LVARparam( node *arg_node, info *arg_info)
{
    char *var_name;

    DBUG_ENTER("LVARparam");

    VAR_DECL( PARAM_ID( arg_node)) = arg_node;

    var_name = VAR_NAME( PARAM_ID( arg_node));

    /* check for duplicated variable declaration */
    if(hashmap_contains(arg_info->local, var_name)) {
        CTIerror(":%d: error: '%s' has already been defined in this context",
                NODE_LINE(arg_node), var_name);

        CTIerror(":%d: error: location of earlier definition",
                NODE_LINE((node *)hashmap_get(arg_info->local, var_name)));
        DBUG_RETURN(arg_node);
    }

    /* add local variabele declaration to hashmap */
    hashmap_add(arg_info->local, var_name, arg_node);

    DBUG_RETURN(arg_node);
}

node *LVARfuncall( node *arg_node, info *arg_info)
{
    DBUG_ENTER("LVARfuncall");

    /* traverse aguments of function call */
    FUNCALL_ARGUMENTS(arg_node) = TRAVopt(FUNCALL_ARGUMENTS(arg_node), arg_info);

    DBUG_RETURN( arg_node);
}

node *LVARvardec( node *arg_node, info *arg_info)
{
    char *var_name;

    DBUG_ENTER("LVARvardec");

    VAR_DECL( VARDEC_ID( arg_node)) = arg_node;

    var_name = VAR_NAME(VARDEC_ID( arg_node));

    /* check for duplicated variable declaration */
    if(hashmap_contains(arg_info->local, var_name)) {
        CTIerror(":%d: error: '%s' has already been defined in this context",
                NODE_LINE(arg_node), var_name);

        CTIerror(":%d: error: location of earlier definition",
                NODE_LINE((node *)hashmap_get(arg_info->local, var_name)));
        DBUG_RETURN(arg_node);
    }

    /* add local variabele declaration to hashmap */
    hashmap_add(arg_info->local, var_name, arg_node);

    DBUG_RETURN( arg_node);
}

node *LVARvar( node *arg_node, info *arg_info)
{
    char *var_name;
    node *var_dec;

    DBUG_ENTER("LVARvar");

    var_name = VAR_NAME(arg_node);

    /* check for local variable declaration */
    var_dec = hashmap_get(arg_info->local, var_name);

    /* check for global variable declaration */
    if(!var_dec)
        var_dec = hashmap_get(arg_info->global, var_name);

    if(var_dec)
        VAR_DECL(arg_node) = var_dec;
    else
        CTIerror(":%d: error: unknown identifier '%s'", NODE_LINE(arg_node), var_name);

    DBUG_RETURN( arg_node);
}



node *CTPdoLinkVar(node *syntaxtree)
{
    info *info;

    DBUG_ENTER("CTPdoLinkVar");

    DBUG_ASSERT( ( syntaxtree != NULL), "CTPdoLink called with empty syntaxtree");

    info = MakeInfo();

    TRAVpush(TR_lvar);

    syntaxtree = TRAVdo( syntaxtree, info);

    //report_unlinked_identifiers(info);

    TRAVpop();

    info = FreeInfo( info);

    DBUG_RETURN( syntaxtree);
}
