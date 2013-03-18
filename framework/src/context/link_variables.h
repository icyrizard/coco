extern node *CTPdoLinkVar(node *syntaxtree);

extern node *LVARglobaldec( node *arg_node, info *arg_info);
extern node *LVARglobaldef( node *arg_node, info *arg_info);
extern node *LVARfundef( node *arg_node, info *arg_info);
extern node *LVARfunheader( node *arg_node, info *arg_info);
extern node *LVARparam( node *arg_node, info *arg_info);
extern node *LVARfuncall( node *arg_node, info *arg_info);
extern node *LVARvardec( node *arg_node, info *arg_info);
extern node *LVARvar( node *arg_node, info *arg_info);
