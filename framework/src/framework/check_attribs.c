/* ---------------------------------------------------------------------------
 * 
 * SAC Compiler Construction Framework
 * 
 * ---------------------------------------------------------------------------
 * 
 * SAC COPYRIGHT NOTICE, LICENSE, AND DISCLAIMER
 * 
 * (c) Copyright 1994 - 2011 by
 * 
 *   SAC Development Team
 *   SAC Research Foundation
 * 
 *   http://www.sac-home.org
 *   email:info@sac-home.org
 * 
 *   All rights reserved
 * 
 * ---------------------------------------------------------------------------
 * 
 * The SAC compiler construction framework, all accompanying 
 * software and documentation (in the following named this software)
 * is developed by the SAC Development Team (in the following named
 * the developer) which reserves all rights on this software.
 * 
 * Permission to use this software is hereby granted free of charge
 * exclusively for the duration and purpose of the course 
 *   "Compilers and Operating Systems" 
 * of the MSc programme Grid Computing at the University of Amsterdam.
 * Redistribution of the software or any parts thereof as well as any
 * alteration  of the software or any parts thereof other than those 
 * required to use the compiler construction framework for the purpose
 * of the above mentioned course are not permitted.
 * 
 * The developer disclaims all warranties with regard to this software,
 * including all implied warranties of merchantability and fitness.  In no
 * event shall the developer be liable for any special, indirect or
 * consequential damages or any damages whatsoever resulting from loss of
 * use, data, or profits, whether in an action of contract, negligence, or
 * other tortuous action, arising out of or in connection with the use or
 * performance of this software. The entire risk as to the quality and
 * performance of this software is with you. Should this software prove
 * defective, you assume the cost of all servicing, repair, or correction.
 * 
 * ---------------------------------------------------------------------------
 */ 


#include "check_attribs.h"

#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"


/** <!--******************************************************************-->
 *
 * @fn CHKMattribString
 *
 * @brief Touch String attribute
 *
 * @param attr String node to process
 * @param arg_info arg_info structure
 *
 * @return the string
 *
 ***************************************************************************/
char *
CHKMattribString( char *attr, info * arg_info)
{
  DBUG_ENTER( "CHKMattribString");

  /* do nothing */

  DBUG_RETURN (attr);
}


/** <!--******************************************************************-->
 *
 * @fn CHKMattribSharedString
 *
 * @brief Touch String attribute
 *
 * @param attr String node to process
 * @param arg_info arg_info structure
 *
 * @return NULL
 *
 ***************************************************************************/
char *
CHKMattribSharedString( char *attr, info * arg_info)
{
  DBUG_ENTER( "CHKMattribSharedString");

  /* do nothing */

  DBUG_RETURN( attr);
}


/** <!--******************************************************************-->
 *
 * @fn CHKMattribNode
 *
 * @brief Touch Node attribute
 *
 * @param attr Node node to process
 * @param arg_info arg_info structure
 *
 * @return the attribute
 *
 ***************************************************************************/
node *
CHKMattribNode( node * attr, info * arg_info)
{
  DBUG_ENTER( "CHKMattribNode");

  if( attr != NULL) {
    attr = TRAVdo( attr, arg_info);
  }

  DBUG_RETURN( attr);
}



/** <!--******************************************************************-->
 *
 * @fn CHKMattribLink
 *
 * @brief Touch Link attribute
 *
 * @param attr Link node to process
 * @param arg_info arg_info structure
 *
 * @return entire attribute
 *
 ***************************************************************************/
node *
CHKMattribLink( node * attr, info * arg_info)
{
  DBUG_ENTER( "CHKMattribLink");

  /*
   * NEVER do anything with this kind of attribute
   * as you cannot make sure the node you reference
   * here really exists!
   */

  DBUG_RETURN( attr);
}

/** <!--******************************************************************-->
 *
 * @fn CHKMattribExtLink
 *
 * @brief Touch Link attribute
 *
 * @param attr Link node to process
 * @param arg_info arg_info structure
 *
 * @return entire attribute
 *
 ***************************************************************************/
node *
CHKMattribExtLink( node * attr, info * arg_info)
{
  DBUG_ENTER( "CHKMattribExtLink");

  DBUG_RETURN( attr);
}

