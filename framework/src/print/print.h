
/**
 * @file print.h
 *
 * Functions to print node structures
 *
 */

#ifndef _SAC_PRT_NODE_H_
#define _SAC_PRT_NODE_H_

#include "types.h"

extern node *PRTprogram (node * arg_node, info * arg_info);
extern node *PRTassign (node * arg_node, info * arg_info);
extern node *PRTvar (node * arg_node, info * arg_info);
extern node *PRTvarlet (node * arg_node, info * arg_info);
extern node *PRTbinop (node * arg_node, info * arg_info);
extern node *PRTfloat (node * arg_node, info * arg_info);
extern node *PRTnum (node * arg_node, info * arg_info);
extern node *PRTbool (node * arg_node, info * arg_info);
extern node *PRTerror (node * arg_node, info * arg_info);

extern node *PRTdoPrint( node *syntaxtree);

extern node *PRTmonop(node * arg_node, info * arg_info);
extern node *PRTfundec (node * arg_node, info * arg_info);

extern node *PRTglobaldec (node * arg_node, info * arg_info);
extern node *PRTglobaldef (node * arg_node, info * arg_info);
extern node *PRTcast (node * arg_node, info * arg_info);
extern node *PRTconditionif (node * arg_node, info * arg_info);
extern node *PRTwhileloop (node * arg_node, info * arg_info);
extern node *PRTdowhileloop (node * arg_node, info * arg_info);
extern node *PRTforloop (node * arg_node, info * arg_info);
extern node *PRTconst (node * arg_node, info * arg_info);
extern node *PRTfuncall (node * arg_node, info * arg_info);
extern node *PRTfundef (node * arg_node, info * arg_info);
extern node *PRTfunbody (node * arg_node, info * arg_info);
extern node *PRTexprlist (node * arg_node, info * arg_info);
extern node *PRTvardeclist (node * arg_node, info * arg_info);
extern node *PRTvardec (node * arg_node, info * arg_info);
extern node *PRTstatementlist (node * arg_node, info * arg_info);
extern node *PRTstatement (node * arg_node, info * arg_info);
extern node *PRTfunheader (node * arg_node, info * arg_info);
extern node *PRTparamlist (node * arg_node, info * arg_info);
extern node *PRTparam (node * arg_node, info * arg_info);

#endif /* _SAC_PRT_NODE_H_ */
