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
<xsl:import href="common-node-access.xsl"/>
<xsl:import href="common-name-to-nodeenum.xsl"/>

<xsl:output method="text" indent="no"/>
<xsl:strip-space elements="*"/>

<!-- generate accessors for nodes -->
<xsl:template match="son" mode="accessor-macros">
  <xsl:value-of select="'#define '"/>
  <xsl:call-template name="node-access">
    <xsl:with-param name="node">n</xsl:with-param>
    <xsl:with-param name="nodetype">
      <xsl:value-of select="../../@name" />
    </xsl:with-param>
    <xsl:with-param name="field">
      <xsl:value-of select="@name" />
    </xsl:with-param>
  </xsl:call-template>
  <xsl:value-of select="'((n)->sons.'"/> 
  <xsl:call-template name="name-to-nodeenum">
    <xsl:with-param name="name">
      <xsl:value-of select="../../@name"/>
    </xsl:with-param>
  </xsl:call-template>
  <xsl:value-of select="'->'" />
  <xsl:value-of select="@name" />
  <xsl:value-of select="')'" />
  <xsl:call-template name="newline"/>
</xsl:template>

<!-- generate accessors for attributes -->
<xsl:template match="attribute" mode="accessor-macros">
  <xsl:value-of select="'#define '"/>
  <!-- generate left side of macro -->
  <xsl:call-template name="node-access">
    <xsl:with-param name="node">n</xsl:with-param>
    <xsl:with-param name="nodetype">
      <xsl:value-of select="../../@name" />
    </xsl:with-param>
    <xsl:with-param name="field">
      <xsl:value-of select="@name" />
    </xsl:with-param>
    <!-- if the attribute is an array, we need to add the index to the macro -->
    <xsl:with-param name="index">
      <xsl:if test="key( &quot;arraytypes&quot;, ./type/@name)">
        <xsl:value-of select="'x'" />
      </xsl:if>
    </xsl:with-param>
  </xsl:call-template>
  <!-- generate right side of macro -->
  <xsl:value-of select="'((n)->attribs.'"/> 
  <xsl:call-template name="name-to-nodeenum">
    <xsl:with-param name="name">
      <xsl:value-of select="../../@name"/>
    </xsl:with-param>
  </xsl:call-template>
  <xsl:value-of select="'->'"/>
  <xsl:value-of select="@name"/>
  <!-- if the attribute is an array, we need to add the index to the macro -->
  <xsl:if test="key( &quot;arraytypes&quot;, ./type/@name)">
    <xsl:value-of select="'[x]'" />
  </xsl:if>
  <xsl:value-of select="')'" />
  <xsl:call-template name="newline"/>
</xsl:template>

<!-- generate macros for flags -->
<xsl:template match="flags[flag]" mode="accessor-macros" >
  <xsl:value-of select="'#define '"/>
  <!-- generate left side of macro -->
  <xsl:call-template name="node-access">
    <xsl:with-param name="node">n</xsl:with-param>
    <xsl:with-param name="nodetype">
      <xsl:value-of select="../@name" />
    </xsl:with-param>
    <xsl:with-param name="field">
      <xsl:value-of select="'FlagStructure'" />
    </xsl:with-param>
  </xsl:call-template>
  <!-- generate right side of macro -->
  <xsl:value-of select="'((n)->attribs.'"/> 
  <xsl:call-template name="name-to-nodeenum">
    <xsl:with-param name="name">
      <xsl:value-of select="../@name"/>
    </xsl:with-param>
  </xsl:call-template>
  <xsl:value-of select="'->flags'"/>
  <xsl:value-of select="')'" />
  <xsl:call-template name="newline"/>
  <!-- generate macros for each flag -->
  <xsl:apply-templates select="flag" mode="accessor-macros" />
</xsl:template>
  
<xsl:template match="flag" mode="accessor-macros">
  <xsl:value-of select="'#define '"/>
  <!-- generate left side of macro -->
  <xsl:call-template name="node-access">
    <xsl:with-param name="node">n</xsl:with-param>
    <xsl:with-param name="nodetype">
      <xsl:value-of select="../../@name" />
    </xsl:with-param>
    <xsl:with-param name="field">
      <xsl:value-of select="@name" />
    </xsl:with-param>
  </xsl:call-template>
  <!-- generate right side of macro -->
  <xsl:value-of select="'((n)->attribs.'"/> 
  <xsl:call-template name="name-to-nodeenum">
    <xsl:with-param name="name">
      <xsl:value-of select="../../@name"/>
    </xsl:with-param>
  </xsl:call-template>
  <xsl:value-of select="'->flags.'"/>
  <xsl:value-of select="@name"/>
  <xsl:value-of select="')'" />
  <xsl:call-template name="newline"/>
</xsl:template>

</xsl:stylesheet>
