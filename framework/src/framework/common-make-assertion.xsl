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
<xsl:import href="common-name-to-nodeenum.xsl"/>

<xsl:output method="text" indent="no"/>
<xsl:strip-space elements="*"/>

<xsl:template match="node" mode="make-assertion">
  <xsl:apply-templates select="sons/son" mode="make-assertion" />
</xsl:template>

<!-- templates for generating assertions for a correct node type -->
<xsl:template match="son" mode="make-assertion">
  <xsl:param name="self"/>
  <xsl:apply-templates select="." mode="make-assertion-nonnull">
    <xsl:with-param name="self"><xsl:value-of select="$self"/></xsl:with-param>
  </xsl:apply-templates>
  <xsl:apply-templates select="." mode="make-assertion-target">
    <xsl:with-param name="self"><xsl:value-of select="$self"/></xsl:with-param>
  </xsl:apply-templates>
</xsl:template>

<!-- check for mandatory sons to be set to a value other than NULL -->
<xsl:template match="son[@mandatory = 'yes']" mode="make-assertion-nonnull">
  <xsl:param name="self"/>
  <xsl:value-of select="'DBUG_ASSERT( '" />
  <xsl:call-template name="node-access">
    <xsl:with-param name="node">
      <xsl:value-of select="$self" />
    </xsl:with-param>
    <xsl:with-param name="nodetype">
      <xsl:value-of select="../../@name" />
    </xsl:with-param>
    <xsl:with-param name="field" >
      <xsl:value-of select="@name" />
    </xsl:with-param>
  </xsl:call-template>
  <xsl:value-of select="' != NULL, ( &quot;Field '" />
  <xsl:value-of select="@name" />
  <xsl:value-of select="' of node N_'" />
  <xsl:value-of select="../../@name" />
  <xsl:value-of select="' is mandatory but has value NULL&quot;));'" />
</xsl:template>

<!-- ignore all other sons -->
<xsl:template match="son" mode="make-assertion-nonnull">
  <xsl:param name="self"/>
</xsl:template>

<!-- check whether the value is of the right target -->
<xsl:template match="son" mode="make-assertion-target">
  <xsl:param name="self"/>
  <xsl:call-template name="newline" />
  <xsl:value-of select="'if ( ( '"/>
  <!-- check non-zero -->
  <xsl:call-template name="node-access">
    <xsl:with-param name="node">
      <xsl:value-of select="$self" />
    </xsl:with-param>
    <xsl:with-param name="nodetype">
      <xsl:value-of select="../../@name" />
    </xsl:with-param>
    <xsl:with-param name="field" >
      <xsl:value-of select="@name" />
    </xsl:with-param>
  </xsl:call-template>
  <xsl:value-of select="' != NULL) '"/>
  <!-- check for target types -->
  <xsl:apply-templates select="targets/target" mode="make-assertion-target">
    <xsl:with-param name="self" select="$self" />
  </xsl:apply-templates>
  <!-- a reasonable errormessage -->
  <xsl:value-of select="') { CTIwarn( &quot;Field '" />
  <xsl:value-of select="@name" />
  <xsl:value-of select="' of node N_'" />
  <xsl:value-of select="../../@name" />
  <xsl:value-of select="' has non-allowed target node.&quot;); } '" />
</xsl:template>

<!-- for each target node we generate one N_xxx for the conditional -->
<xsl:template match="target/node" mode="make-assertion-target">
  <xsl:param name="self"/>
  <xsl:value-of select="'&amp;&amp; ( NODE_TYPE( '" />
  <xsl:call-template name="node-access">
    <xsl:with-param name="node">
      <xsl:value-of select="$self" />
    </xsl:with-param>
    <xsl:with-param name="nodetype">
      <xsl:value-of select="../../../../../@name" />
    </xsl:with-param>
    <xsl:with-param name="field" >
      <xsl:value-of select="../../../@name" />
    </xsl:with-param>
  </xsl:call-template>
  <xsl:value-of select="') != '" />
  <xsl:call-template name="name-to-nodeenum">
    <xsl:with-param name="name">
      <xsl:value-of select="@name" />
    </xsl:with-param>
  </xsl:call-template>
  <xsl:value-of select="')'" />
</xsl:template>

<xsl:template match="target/set" mode="make-assertion-target">
  <xsl:param name="self"/>
  <!-- save information about current node for use in for-each loop -->
  <xsl:variable name="nodetype">
    <xsl:value-of select="../../../../../@name" />
  </xsl:variable>
  <xsl:variable name="field">
    <xsl:value-of select="../../../@name" />
  </xsl:variable>
  <!-- iterate over all nodeset members -->
  <xsl:for-each select="//nodesets/nodeset[@name = current()/@name]/target/node">
    <xsl:value-of select="'&amp;&amp; ( NODE_TYPE( '" />
    <xsl:call-template name="node-access">
      <xsl:with-param name="node">
        <xsl:value-of select="$self" />
      </xsl:with-param>
      <xsl:with-param name="nodetype">
        <xsl:value-of select="$nodetype" />
      </xsl:with-param>
      <xsl:with-param name="field" >
        <xsl:value-of select="$field" />
      </xsl:with-param>
    </xsl:call-template>
    <xsl:value-of select="') != '" />
    <xsl:call-template name="name-to-nodeenum">
      <xsl:with-param name="name">
        <xsl:value-of select="@name" />
      </xsl:with-param>
    </xsl:call-template>
    <xsl:value-of select="')'" />
  </xsl:for-each>
</xsl:template>

<xsl:template match="target/unknown" mode="make-assertion-target">
  <xsl:value-of select="'&amp;&amp; FALSE'" />
</xsl:template>

</xsl:stylesheet>
