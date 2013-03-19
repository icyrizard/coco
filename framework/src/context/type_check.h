node *CTPdoType(node *syntaxtree);

node *TYPEfundef(node *arg_node, info *arg_info);
node *TYPEfunheader(node *arg_node, info *arg_info);
node *TYPEfunbody(node *arg_node, info *arg_info);
node *TYPEfuncall(node *arg_node, info *arg_info);
node *TYPEbinop(node *arg_node, info *arg_info);
node *TYPEassign(node *arg_node, info *arg_info);
node *TYPEvar(node *arg_node, info *arg_info);
node *TYPEfloat(node *arg_node, info *arg_info);
node *TYPEnum(node *arg_node, info *arg_info);
node *TYPEbool(node *arg_node, info *arg_info);
node *TYPEconditionif(node *arg_node, info *arg_info);
