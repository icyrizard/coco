#include "types.h"
#include "memory.h"
#include "dbug.h"
#include "tree_basic.h"
#include "traverse.h"
#include "globals.h"
#include "str.h"
#include "ctinfo.h"
#include "list_hash.h"
#include "logic.h"

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

extern node *CTPdoLogic(node *syntaxtree)
{
    info *info;

    DBUG_ENTER("CTPdoLinkFun");

    DBUG_ASSERT( ( syntaxtree != NULL), "CTPdoLink called with empty syntaxtree");

    info = MakeInfo();

    TRAVpush(TR_lfun);

    syntaxtree = TRAVdo( syntaxtree, info);

    TRAVpop();

    info = FreeInfo( info);

    DBUG_RETURN( syntaxtree);
}
