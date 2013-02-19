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
<xsl:import href="common-c-code.xsl"/>

<xsl:output method="text" indent="no"/>
<xsl:strip-space elements="*"/>

<!--
     This file is imported by various xsl transformers to generate
     tree traversals. 

     templates:

     travfun-head           generates a c function head for traversal
                            functions
     travfun-comment        generates a doxygen comment for a traversal
                            function
     travfun-file           generates a doxygen comment for a file
     travfun-group-begin    generates the start tag for a doxygen group
     travfun-group-end      generates the end tag for a doxygen group

     traversals:

-->

<!--
     template travfun-head

     generates a traversal function head

     param prefix   the prefix of the phase (e.g. Free)
     param name     the name of the node (e.g. Ap)

     example:

     node *FreeAp( node *arg_node, info *arg_info )
-->
<xsl:template name="travfun-head">
  <xsl:param name="prefix" />
  <xsl:param name="name" />
  <xsl:value-of select="'node *'"/>
  <xsl:call-template name="travfun-name">
    <xsl:with-param name="prefix">
      <xsl:value-of select="$prefix" />
    </xsl:with-param>
    <xsl:with-param name="name">
      <xsl:value-of select="$name" />
    </xsl:with-param>
  </xsl:call-template>
  <xsl:value-of select="'( node *arg_node, info *arg_info )'"/>
</xsl:template>

<xsl:template name="travfun-name">
  <xsl:param name="prefix" />
  <xsl:param name="name" />
  <xsl:value-of select="$prefix"/>
  <xsl:call-template name="lowercase">
    <xsl:with-param name="string" >
      <xsl:value-of select="$name"/>
    </xsl:with-param>
  </xsl:call-template>
</xsl:template>

<!--
     template travfun-comment

     generates a doxygen comment for a traversal function

     param prefix   the preifx of the phase (e.g. FREE)
     param name     the name of the node (e.g. Ap)
     param text     the brief description (e.g. a traversal function)

     example:

     /** &lt;!&minus;&minus;******************************************************************&minus;&minus;&gt;
      *
      * @fn FREEap
      *
      * @brief Frees the node and its sons/attributes
      *
      * @param arg_node Ap node to process
      * @param arg_info pointer to info structure
      *
      * @return processed node
      *
      ***************************************************************************/
-->
<xsl:template name="travfun-comment">
  <xsl:param name="prefix" />
  <xsl:param name="name" />
  <xsl:param name="text" />
  <xsl:text>
/** &lt;!--******************************************************************--&gt;
 *
 * @fn </xsl:text>
  <xsl:value-of select="$prefix"/>
  <xsl:call-template name="lowercase" >
    <xsl:with-param name="string" >
      <xsl:value-of select="$name"/>
    </xsl:with-param>
  </xsl:call-template>
  <xsl:text>
 *
 * @brief </xsl:text><xsl:value-of select="$text"/><xsl:text>
 *
 * @param arg_node </xsl:text><xsl:value-of select="$name"/><xsl:text> node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/
  </xsl:text>
</xsl:template>

<!--
     template travfun-file

     generates a doxugen file description comment

     param file     the file name (e.g. free_node.c)
     param desc     a description (e.g. free tree traversal)
     param xslt     the generating xslt script (e.g. ast2free_node_c.xsl)

     example:

     /**
      * @file free_node.c
      *
      * free tree traversal
      *
      * THIS FILE HAS BEEN GENERATED USING 
      * ast2free_node_c.xsl.
      * DO NOT EDIT THIS FILE. EDIT THE AST SPEC IN ast.xml INSTEAD!
      *
      * ALL CHANGES MADE TO THIS FILE WILL BE OVERWRITTEN!
      *
      */
-->
<xsl:template name="travfun-file">
  <xsl:param name="file"/>
  <xsl:param name="desc"/>
  <xsl:param name="xslt"/>
  <xsl:text>
/**
 * @file </xsl:text><xsl:value-of select="$file"/><xsl:text>
 *
 * </xsl:text><xsl:value-of select="$desc"/><xsl:text>
 * 
 * THIS FILE HAS BEEN GENERATED USING 
 * </xsl:text><xsl:value-of select="$xslt"/><xsl:text>.
 * DO NOT EDIT THIS FILE AS MIGHT BE CHANGED IN A LATER VERSION.
 *
 * ALL CHANGES MADE TO THIS FILE WILL BE OVERWRITTEN!
 *
 */
  </xsl:text>
</xsl:template>

<!--
     template travfun-group-begin

     generates a doxygen function group

     param group    group identifier (e.g. free)
     param name     group name (e.g. Free Traversal)
     param desc     a description (e.g. files for free tree traversal)

     example:

     /**
      * @defgroup free Free Traversal
      *
      * files for free tree traversal
      *
      * @{
      */
-->
<xsl:template name="travfun-group-begin">
  <xsl:param name="group"/>
  <xsl:param name="name"/>
  <xsl:param name="desc"/>
  <xsl:text>
/**
 * @defgroup </xsl:text><xsl:value-of select="$group"/><xsl:text> </xsl:text><xsl:value-of select="$name"/><xsl:text>
 *
 * </xsl:text><xsl:value-of select="$desc"/><xsl:text>
 *
 * @{
 */
  </xsl:text>
</xsl:template>

<!--
     template travfun-group-end

     generates a doxygen group end tag

     example:

     /* @} */
-->
<xsl:template name="travfun-group-end">
  <xsl:text>
/**
 * @}
 */
  </xsl:text>
</xsl:template>

</xsl:stylesheet>
