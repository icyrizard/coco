#include "types.h"
#include "link_functions.h"
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
    hashmap *fun_decs;
    hashmap *fun_calls;
};

static info *MakeInfo()
{
    info *result;

    result = MEMmalloc(sizeof(info));
    result->fun_decs = hashmap_create();
    result->fun_calls = hashmap_create();

    return result;
}

static info *FreeInfo( info *info)
{
    hashmap_free(info->fun_decs);
    hashmap_free(info->fun_calls);

    info = MEMfree( info);

    return info;
}

/******************   Help Functions   ******************/
void report_unlinked_functioncalls(info *arg_info)
{
    hashmap *tmp;

    while(!hashmap_empty(arg_info->fun_calls)) {
        tmp = hashmap_pop_last(arg_info->fun_calls);

        CTIerror(":%d: error: unknown identifier '%s'", NODE_LINE((node *)tmp->value),
                (char *)tmp->key);
    }
}


/*********************   Traverse   *********************/
node *LFUNfunheader(node *arg_node, info *arg_info)
{
    char *funname;
    node *funcall;

    DBUG_ENTER("LFUNfunheader");

    funname = VAR_NAME(FUNHEADER_ID(arg_node));
    VAR_DECL( FUNHEADER_ID(arg_node)) = arg_node;

    /* has their already been a function with same name*/
    if(hashmap_contains(arg_info->fun_decs, funname)) {
        CTIerror(":%d: error: '%s' has already been defined in this context",
                NODE_LINE(arg_node), funname);

        CTIerror(":%d: error: location of earlier definition",
                NODE_LINE((node *)hashmap_get(arg_info->fun_decs, funname)));
        DBUG_RETURN(arg_node);
    }

    /* add new function rule to info object */
    hashmap_add(arg_info->fun_decs, funname, arg_node);

    /* check previously saved funcalls */
    while(hashmap_contains(arg_info->fun_calls, funname)) {
        funcall = hashmap_get(arg_info->fun_calls, funname);
        VAR_DECL( FUNCALL_ID(funcall)) = arg_node;

        hashmap_remove(arg_info->fun_calls, funname);
    }
    DBUG_RETURN(arg_node);
}


node *LFUNfuncall(node *arg_node, info *arg_info)
{
    char *funname;
    node *fundec;

    DBUG_ENTER("LFUNfuncall");

    /* traverse aguments of function call */
    FUNCALL_ARGUMENTS(arg_node) = TRAVopt(FUNCALL_ARGUMENTS(arg_node), arg_info);

    funname = VAR_NAME(FUNCALL_ID( arg_node));

    /* search for funcion declaration with same name */
    fundec = hashmap_get(arg_info->fun_decs, funname);

    /* set declaration if function is found */
    if(fundec)
        VAR_DECL(FUNCALL_ID(arg_node)) = fundec;
    else
        hashmap_add(arg_info->fun_calls, funname, arg_node);

    DBUG_RETURN( arg_node);
}

node *CTPdoLinkFun(node *syntaxtree)
{
    info *info;

    DBUG_ENTER("CTPdoLinkFun");

    DBUG_ASSERT( ( syntaxtree != NULL), "CTPdoLink called with empty syntaxtree");

    info = MakeInfo();

    TRAVpush(TR_lfun);

    syntaxtree = TRAVdo( syntaxtree, info);

    report_unlinked_functioncalls(info);

    TRAVpop();

    info = FreeInfo( info);

    DBUG_RETURN( syntaxtree);
}
