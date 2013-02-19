<?xml version="1.0"?>


<!--

****************************************************************************
* 
* SAC Compiler Construction Framework
* 
****************************************************************************
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
****************************************************************************
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
****************************************************************************
 
 -->




<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
version="1.0">

<xsl:import href="common-key-tables.xsl"/>
<xsl:import href="common-travfun.xsl"/>
<xsl:import href="common-node-access.xsl"/>
<xsl:import href="common-c-code.xsl"/>

<xsl:output method="text" indent="no"/>
<xsl:strip-space elements="*"/>

<xsl:template match="/">
  <!-- generate file header and doxygen group -->
  <xsl:call-template name="travfun-file">
    <xsl:with-param name="file">
      <xsl:value-of select="'copy_node.c'"/>
    </xsl:with-param>
    <xsl:with-param name="desc">
      <xsl:value-of select="'Functions needed by copy traversal.'"/>
    </xsl:with-param>
    <xsl:with-param name="xslt">
      <xsl:value-of select="'copy_node.c.xsl'"/>
    </xsl:with-param>
  </xsl:call-template>
  <xsl:call-template name="travfun-group-begin">
    <xsl:with-param name="group">
      <xsl:value-of select="'copy'"/>
    </xsl:with-param>
    <xsl:with-param name="name">
      <xsl:value-of select="'Copy Tree Functions.'"/>
    </xsl:with-param>
    <xsl:with-param name="desc">
      <xsl:value-of select="'Functions needed by copy traversal.'"/>
    </xsl:with-param>
  </xsl:call-template>
  <!-- includes -->
  <xsl:text>

#include "copy_node.h"
#include "copy_info.h"
#include "traverse.h"
#include "dbug.h"
#include "tree_basic.h"
#include "str.h"
#include "lookup_table.h"


#define COPYTRAV( node, info) (node != NULL) ? TRAVdo( node, info) : node

  </xsl:text>
  <!-- functions -->
  <xsl:apply-templates select="//syntaxtree/node">
    <xsl:sort select="@name"/>
  </xsl:apply-templates>
  <!-- end of doxygen group -->
  <xsl:call-template name="travfun-group-end"/>
</xsl:template>


<!-- generate copy functions -->
<xsl:template match="node">
  <!-- generate head and comment -->
  <xsl:apply-templates select="@name"/>
  <!-- start of body -->
  <xsl:value-of select="'{'"/>
  <!-- variable and new empty node for result -->    
  <xsl:value-of select="'node *result = '"/>
  <xsl:apply-templates select="." mode="make-function-call"/>
  <xsl:value-of select="';'"/>
  <!-- give hint we start to copy now -->
  <xsl:value-of select="'DBUG_ENTER(&quot;COPY'"/>
  <xsl:call-template name="lowercase" >
    <xsl:with-param name="string" >
      <xsl:value-of select="@name" />
    </xsl:with-param>
  </xsl:call-template>
  <xsl:value-of select="'&quot;);'"/>
  <xsl:value-of select="'LUTinsertIntoLutP( INFO_LUT( arg_info), arg_node, result);'"/>
  <!-- call copy for attributes -->   
  <xsl:if test="count(attributes/attribute) > 0">
    <xsl:text>
    /* Copy attributes */
    </xsl:text>
  </xsl:if>
  <xsl:apply-templates select="attributes/attribute"/> 
  <!-- call copy for all sons --> 
  <xsl:if test="count(sons/son[@name]) > 0">
    <xsl:text>
    /* Copy sons */
    </xsl:text>
  </xsl:if>
  <xsl:apply-templates select="sons/son[@name]"/>
  <!-- return value -->
  <xsl:text>
  /* Return value */
  </xsl:text>
  <xsl:value-of select="'DBUG_RETURN( result);'"/>
  <!-- end of body -->
  <xsl:value-of select="'}'"/>
</xsl:template>


<!-- generate a comment and a function header -->
<xsl:template match="@name">
  <xsl:call-template name="travfun-comment">
    <xsl:with-param name="prefix">COPY</xsl:with-param>
    <xsl:with-param name="name"><xsl:value-of select="." /></xsl:with-param>
    <xsl:with-param name="text">Copies the node and its sons/attributes</xsl:with-param>
  </xsl:call-template>  
  <xsl:call-template name="travfun-head">
    <xsl:with-param name="prefix">COPY</xsl:with-param>
    <xsl:with-param name="name"><xsl:value-of select="." /></xsl:with-param>
  </xsl:call-template>
</xsl:template>


<!-- generate copy calls for sons-->
<xsl:template match="son">
  <xsl:call-template name="node-access">
    <xsl:with-param name="node">result</xsl:with-param>
    <xsl:with-param name="nodetype">
      <xsl:value-of select="../../@name"/>
    </xsl:with-param>
    <xsl:with-param name="field">
      <xsl:value-of select="@name"/>
    </xsl:with-param>
  </xsl:call-template>
  <xsl:value-of select="' = COPYTRAV( '"/>
  <xsl:call-template name="node-access">
    <xsl:with-param name="node">arg_node</xsl:with-param>
    <xsl:with-param name="nodetype">
      <xsl:value-of select="../../@name"/>
    </xsl:with-param>
    <xsl:with-param name="field">
      <xsl:value-of select="@name"/>
    </xsl:with-param>
  </xsl:call-template>
  <xsl:value-of select="', arg_info);'"/>
</xsl:template>


<!-- generate copying attributes -->
<xsl:template match="attribute">
  <!-- left side of assignment -->
  <xsl:call-template name="node-access">
    <xsl:with-param name="node">
      <xsl:value-of select="'result'" />
    </xsl:with-param>
    <xsl:with-param name="nodetype">
      <xsl:value-of select="../../@name" />
    </xsl:with-param>
    <xsl:with-param name="field">
      <xsl:value-of select="@name" />
    </xsl:with-param>
    </xsl:call-template>
    <xsl:value-of select="' = '" />
  <!-- right side of assignment -->
  <xsl:choose>
    <!-- literal attributes are just copied (assigment) -->
    <xsl:when test="key(&quot;types&quot;, ./type/@name)[@copy = &quot;literal&quot;]">
      <xsl:call-template name="node-access">
        <xsl:with-param name="node">
          <xsl:value-of select="'arg_node'" />
        </xsl:with-param>
        <xsl:with-param name="nodetype">
          <xsl:value-of select="../../@name" />
        </xsl:with-param>
        <xsl:with-param name="field">
          <xsl:value-of select="@name" />
        </xsl:with-param>
      </xsl:call-template>
      <xsl:value-of select="';'" />
    </xsl:when>
    <!-- literal attributes are just copied (assigment) -->
    <xsl:when test="key(&quot;types&quot;, ./type/@name)[@copy = &quot;lookup&quot;]">
       <xsl:value-of select="'LUTsearchInLutPp( INFO_LUT( arg_info), '"/>
      <xsl:call-template name="node-access">
        <xsl:with-param name="node">
          <xsl:value-of select="'arg_node'" />
        </xsl:with-param>
        <xsl:with-param name="nodetype">
          <xsl:value-of select="../../@name" />
        </xsl:with-param>
        <xsl:with-param name="field">
          <xsl:value-of select="@name" />
        </xsl:with-param>
      </xsl:call-template>
      <xsl:value-of select="');'" />
    </xsl:when>
    <!-- string attributes are deep copied -->
    <xsl:otherwise>
       <xsl:value-of select="'STRcpy('"/>
       <xsl:call-template name="node-access">
         <xsl:with-param name="node">
           <xsl:value-of select="'arg_node'" />
         </xsl:with-param>
         <xsl:with-param name="nodetype">
           <xsl:value-of select="../../@name" />
         </xsl:with-param>
         <xsl:with-param name="field">
           <xsl:value-of select="@name" />
         </xsl:with-param> 
       </xsl:call-template>
       <xsl:value-of select="');'"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<!-- generate a make function call -->
<xsl:template match="node" mode="make-function-call">
  <!-- function name -->
  <xsl:value-of select="'TBmake'"/>
  <xsl:call-template name="uppercase" >
    <xsl:with-param name="string" >
      <xsl:value-of select="substring( @name, 1, 1)" />
    </xsl:with-param>
  </xsl:call-template>
  <xsl:call-template name="lowercase" >
    <xsl:with-param name="string" >
      <xsl:value-of select="substring( @name, 2, 30)" />
    </xsl:with-param>
  </xsl:call-template>
  <xsl:value-of select="'( '"/>
  <!-- permanent attributes without default value first --> 
  <xsl:apply-templates select="attributes/attribute[type/targets/target/phases/all][not(@default)][type/targets/target/@mandatory = &quot;yes&quot;]" mode="make-function-call"/>
  <!-- add a , if needed -->
  <xsl:if test="attributes/attribute[type/targets/target/phases/all][not(@default)][type/targets/target/@mandatory = &quot;yes&quot;]">
    <xsl:if test="sons/son[ not( @default)]">
      <xsl:value-of select="' ,'"/>
    </xsl:if>
  </xsl:if>
  <!-- sons without default value are last parameters -->
  <xsl:apply-templates select="sons/son[ not( @default)]" mode="make-function-call"/>
  <xsl:value-of select="')'"/>
</xsl:template>


<!-- generate default value for node argument (NULL) -->
<xsl:template match="son" mode="make-function-call">
  <xsl:if test="position() != 1">
    <xsl:value-of select="' ,'"/>
  </xsl:if>
  <!-- all sons are NULL by default -->
  <xsl:value-of select="'NULL'"/>
</xsl:template>


<!-- generate an attribute argument -->
<xsl:template match="attribute" mode="make-function-call">
  <xsl:if test="position() != 1">
    <xsl:value-of select="' ,'"/>  
  </xsl:if>
  <xsl:apply-templates select="@name" mode="make-function-call" />
</xsl:template>


<!-- generate default values for non-node arguments -->
<xsl:template match="@name" mode="make-function-call">
  <xsl:value-of select="key(&quot;types&quot;, ../type/@name)/@init"/>
</xsl:template>

</xsl:stylesheet>
