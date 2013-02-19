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

<!-- generate a make function head -->
<xsl:template match="node" mode="make-head">
  <xsl:value-of select="'node *TBmake'"/>
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
  <xsl:apply-templates select="attributes/attribute[type/targets/target/phases/all][not(@default)][type/targets/target/@mandatory = &quot;yes&quot;]" mode="make-head"/>
  <!-- add a , if needed -->
  <xsl:if test="attributes/attribute[type/targets/target/phases/all][not(@default)][type/targets/target/@mandatory = &quot;yes&quot;]">
    <xsl:if test="sons/son[ not( @default)]">
      <xsl:value-of select="' ,'"/>
    </xsl:if>
  </xsl:if>
  <!-- sons without default value are last parameters -->
  <xsl:apply-templates select="sons/son[ not( @default)]" mode="make-head"/>
  <xsl:value-of select="')'"/>
</xsl:template>

<!-- generate a son argument -->
<xsl:template match="son" mode="make-head">
  <xsl:if test="position() != 1">
    <xsl:value-of select="' ,'"/>
  </xsl:if>
  <xsl:value-of select="'node * '"/>
  <xsl:value-of select="@name"/>
</xsl:template>

<!-- generate an attribute argument -->
<xsl:template match="attribute" mode="make-head">
  <xsl:if test="position() != 1">
    <xsl:value-of select="' ,'"/>   
  </xsl:if>
  <xsl:value-of select="key( &quot;types&quot;, ./type/@name)/@ctype"/>
  <!-- if it is an array, we have to add an indirection to the type -->
  <xsl:if test="key(&quot;arraytypes&quot;, ./type/@name)">
    <xsl:value-of select="'*'" />
  </xsl:if>
  <xsl:value-of select="' '"/>
  <xsl:value-of select="@name"/>
</xsl:template>

</xsl:stylesheet>
