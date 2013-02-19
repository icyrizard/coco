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


<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<!-- Some nice templates for accessing fields of the node structure
     by using the access macros -->

<xsl:template name="node-access">
  <!-- name of the variable pointing to the node -->
  <xsl:param name="node" />
  <!-- name of node type -->
  <xsl:param name="nodetype" />
  <!-- field to select -->
  <xsl:param name="field" />
  <!-- index for fields beeing an array, otherwise do not set -->
  <xsl:param name="index" />

  <!-- generate macro selector ala ARRAY_NEXT(n) -->
  <xsl:call-template name="uppercase">
    <xsl:with-param name="string"><xsl:value-of select="$nodetype"/></xsl:with-param>
  </xsl:call-template>
  <xsl:value-of select="'_'"/>
  <xsl:call-template name="uppercase">
    <xsl:with-param name="string"><xsl:value-of select="$field"/></xsl:with-param>
  </xsl:call-template>
  <xsl:value-of select="'( '" />
  <xsl:value-of select="$node" />
  <xsl:if test="$index != &quot;&quot;">
    <xsl:value-of select="', '" />
    <xsl:value-of select="$index" />
  </xsl:if>
  <xsl:value-of select="') '" />
</xsl:template>

</xsl:stylesheet>
