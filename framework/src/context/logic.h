extern node *CTPdoLogic(node *syntaxtree);

extern node *LOGICfunbody(node *arg_node, info *arg_info);
extern node *LOGICstatementlist(node *arg_node, info *arg_info);
extern node *LOGICconditionif(node *arg_node, info *arg_info);
extern node *LOGICwhileloop(node *arg_node, info *arg_info);
extern node *LOGICassign(node *arg_node, info *arg_info);
extern node *LOGICbinop(node *arg_node, info *arg_info);
extern node *LOGICfuncall(node *arg_node, info *arg_info);
