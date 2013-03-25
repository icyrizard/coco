#ifndef _SAC_ASM_NODE_H_
#define _SAC_ASM_NODE_H_

#include "types.h"

extern node *ASMprogram (node * arg_node, info * arg_info);
extern node *ASMassign (node * arg_node, info * arg_info);
extern node *ASMvar (node * arg_node, info * arg_info);
extern node *ASMbinop (node * arg_node, info * arg_info);
extern node *ASMfloat (node * arg_node, info * arg_info);
extern node *ASMnum (node * arg_node, info * arg_info);
extern node *ASMbool (node * arg_node, info * arg_info);
extern node *ASMerror(node * arg_node, info * arg_info);

extern node *CODEdoAssembly( node *syntaxtree);

extern node *ASMmonop(node * arg_node, info * arg_info);
extern node *ASMfundec (node * arg_node, info * arg_info);
extern node *ASMglobaldec (node * arg_node, info * arg_info);
extern node *ASMglobaldef (node * arg_node, info * arg_info);
extern node *ASMcast (node * arg_node, info * arg_info);
extern node *ASMconditionif (node * arg_node, info * arg_info);
extern node *ASMwhileloop (node * arg_node, info * arg_info);
extern node *ASMdowhileloop (node * arg_node, info * arg_info);
extern node *ASMforloop (node * arg_node, info * arg_info);
extern node *ASMconst (node * arg_node, info * arg_info);
extern node *ASMfuncall (node * arg_node, info * arg_info);
extern node *ASMfundef (node * arg_node, info * arg_info);
extern node *ASMfunbody (node * arg_node, info * arg_info);
extern node *ASMexprlist (node * arg_node, info * arg_info);
extern node *ASMvardeclist (node * arg_node, info * arg_info);
extern node *ASMvardec (node * arg_node, info * arg_info);
extern node *ASMstatementlist (node * arg_node, info * arg_info);
extern node *ASMfunheader (node * arg_node, info * arg_info);
extern node *ASMparamlist (node * arg_node, info * arg_info);
extern node *ASMparam (node * arg_node, info * arg_info);
extern node *ASMassemblyinstr(node * arg_node, info * arg_info);
extern node *ASMassemblyinstrs(node * arg_node, info * arg_info);
extern node *ASMarglist(node * arg_node, info * arg_info);
extern node *ASMarg(node * arg_node, info * arg_info);

#endif /* _SAC_ASM_NODE_H_ */
