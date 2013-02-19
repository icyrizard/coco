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





<!--
  $Id: ast2travers_tables.c.xsl$
-->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  
  <xsl:import href="common-key-tables.xsl"/>
  <xsl:import href="common-travfun.xsl"/>
  <xsl:import href="common-name-to-nodeenum.xsl"/>

  <xsl:output method="text" indent="no"/>
  <xsl:strip-space elements="*"/>

  <!-- starting template -->
  <xsl:template match="/">
    <xsl:call-template name="travfun-file">
      <xsl:with-param name="file">
        <xsl:value-of select="'traverse_tables.c'"/>
      </xsl:with-param>
      <xsl:with-param name="desc">
        <xsl:value-of select="'This file defines the function tables for traversal.'"/>
      </xsl:with-param>
      <xsl:with-param name="xslt">
        <xsl:value-of select="'$Id: traverse_tables.c.xsl 14593 2006-01-31 17:09:55Z cg $'"/>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:text>
#include "traverse_tables.h"
#include "traverse_helper.h"
    </xsl:text>
      <xsl:apply-templates select="/definition/phases//traversal" mode="include" />
    <xsl:text>

travtables_t travtables = {
    /* TR_undefined */
      { &amp;TRAVerror
    </xsl:text>
    <xsl:apply-templates select="/definition/syntaxtree/node" mode="errortraversal" />
    <xsl:value-of select="'} '" />
    <xsl:apply-templates select="/definition/phases//traversal" />
    <xsl:text>
};

preposttable_t pretable = {
    NULL
    </xsl:text>
    <xsl:apply-templates select="/definition/phases//traversal" mode="pretable" />
    <xsl:text>
};

preposttable_t posttable = {
    NULL
    </xsl:text>
    <xsl:apply-templates select="/definition/phases//traversal" mode="posttable" />
    <xsl:text>
};

const char *travnames[ </xsl:text><xsl:value-of select="count(/definition/phases//traversal) + 1"/><xsl:text>] = {
    "unknown"
    </xsl:text>
    <xsl:apply-templates select="/definition/phases//traversal" mode="travnames" />
    <xsl:text>
};

    </xsl:text>
  </xsl:template>

  <xsl:template match="node" mode="errortraversal" >
    <xsl:value-of select="', &amp;TRAVerror'" />
  </xsl:template>

  <xsl:template match="traversal">
    <xsl:variable name="phase">
      <xsl:value-of select="@id" />
    </xsl:variable>
    <xsl:variable name="ifdef">
      <xsl:value-of select="@ifdef" />
    </xsl:variable>
    <xsl:text>

    </xsl:text>
    <xsl:value-of select="'/* TR_'" />
    <xsl:call-template name="lowercase" >
      <xsl:with-param name="string" select="@id" />
    </xsl:call-template>
    <xsl:value-of select="' */'" />
    <xsl:text>
    </xsl:text>
    <xsl:if test="@ifdef">
      <xsl:value-of select="'#ifdef '" />
      <xsl:call-template name="uppercase" >
        <xsl:with-param name="string" select="@ifdef" />          
      </xsl:call-template>
      <xsl:text>
      </xsl:text>
    </xsl:if>
    <xsl:value-of select="', { &amp;TRAVerror'" />
    <xsl:for-each select="/definition/syntaxtree/node" >
      <xsl:value-of select="', '" />
      <xsl:value-of select="'&amp;'" />
      <xsl:choose>
        <xsl:when test="key( &quot;traversals&quot;, $phase)/travuser/node/@name = ./@name" >
          <xsl:call-template name="travtab-to-travname">
            <xsl:with-param name="phase">
              <xsl:value-of select="$phase" />
            </xsl:with-param>
            <xsl:with-param name="node">
              <xsl:value-of select="@name" />
            </xsl:with-param>
            <xsl:with-param name="style">user</xsl:with-param>
          </xsl:call-template>
        </xsl:when>
        <xsl:when test="key( &quot;traversals&quot;, $phase)/travsons/node/@name = ./@name" >
          <xsl:call-template name="travtab-to-travname">
            <xsl:with-param name="phase">
              <xsl:value-of select="$phase" />
            </xsl:with-param>
            <xsl:with-param name="node">
              <xsl:value-of select="@name" />
            </xsl:with-param>
            <xsl:with-param name="style">sons</xsl:with-param>
          </xsl:call-template>
        </xsl:when>
        <xsl:when test="key( &quot;traversals&quot;, $phase)/travnone/node/@name = ./@name" >
          <xsl:call-template name="travtab-to-travname">
            <xsl:with-param name="phase">
              <xsl:value-of select="$phase" />
            </xsl:with-param>
            <xsl:with-param name="node">
              <xsl:value-of select="@name" />
            </xsl:with-param>
            <xsl:with-param name="style">none</xsl:with-param>
          </xsl:call-template>
        </xsl:when>
        <xsl:when test="key( &quot;traversals&quot;, $phase)/traverror/node/@name = ./@name" >
          <xsl:call-template name="travtab-to-travname">
            <xsl:with-param name="phase">
              <xsl:value-of select="current()/@id" />
            </xsl:with-param>
            <xsl:with-param name="node">
              <xsl:value-of select="@name" />
            </xsl:with-param>
            <xsl:with-param name="style">error</xsl:with-param>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="travtab-to-travname">
            <xsl:with-param name="phase">
              <xsl:value-of select="$phase" />
            </xsl:with-param>
            <xsl:with-param name="node">
              <xsl:value-of select="@name" />
            </xsl:with-param>
            <xsl:with-param name="style">
              <xsl:value-of select="key( &quot;traversals&quot;, $phase)/@default" />
            </xsl:with-param>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>
    <xsl:value-of select="' }'" />  
    <xsl:if test="@ifdef">
      <xsl:text>
      </xsl:text>
      <xsl:value-of select="'#else'" />
      <xsl:text>
        , { &amp;TRAVerror
      </xsl:text>
      <xsl:apply-templates select="/definition/syntaxtree/node" mode="errortraversal" />
      <xsl:value-of select="'} '" />
      <xsl:text>
      </xsl:text>
      <xsl:value-of select="'#endif '" />
    </xsl:if>
  </xsl:template>

  <xsl:template name="travtab-to-travname">
    <xsl:param name="phase" />
    <xsl:param name="node" />
    <xsl:param name="style" />
    <xsl:param name="ifdef" />
   
    <xsl:choose>
      <xsl:when test="$style = &quot;user&quot;">
        <xsl:call-template name="travfun-name">
          <xsl:with-param name="prefix">
            <xsl:value-of select="$phase" />
          </xsl:with-param>
          <xsl:with-param name="name">
            <xsl:value-of select="$node" />
          </xsl:with-param>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="$style = &quot;sons&quot;">
        <xsl:value-of select="'TRAVsons'" />
      </xsl:when>
      <xsl:when test="$style = &quot;none&quot;">
        <xsl:value-of select="'TRAVnone'" />
      </xsl:when>
      <xsl:when test="$style = &quot;error&quot;">
        <xsl:value-of select="'TRAVerror'" />
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$style" />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="traversal" mode="include">
    <xsl:value-of select="'#include &quot;'" />
    <xsl:value-of select="@include" />
    <xsl:value-of select="'&quot;'" />
    <xsl:call-template name="newline" />
  </xsl:template>

  <xsl:template match="traversal[@prefun]" mode="pretable">
    <xsl:value-of select="', '" />
    <xsl:if test="@ifdef">
      <xsl:value-of select="$newline" />    
      <xsl:value-of select="'#ifdef '"/>
      <xsl:call-template name="uppercase" >
        <xsl:with-param name="string" select="@ifdef" />          
      </xsl:call-template>
      <xsl:value-of select="$newline" />    
    </xsl:if>
    <xsl:value-of select="'&amp;'" />
    <xsl:value-of select="@prefun" />
    <xsl:if test="@ifdef">
      <xsl:value-of select="$newline" />    
      <xsl:value-of select="'#else '"/>
      <xsl:value-of select="$newline" />    
      <xsl:value-of select="'NULL'" />
      <xsl:value-of select="$newline" />    
      <xsl:value-of select="'#endif'" />
      <xsl:value-of select="$newline" />            
    </xsl:if>
  </xsl:template>
 
  <xsl:template match="traversal" mode="pretable">
    <xsl:value-of select="', NULL'" />
  </xsl:template>

  <xsl:template match="traversal[@postfun]" mode="posttable">
    <xsl:value-of select="', '" />
    <xsl:if test="@ifdef">
      <xsl:value-of select="$newline" />    
      <xsl:value-of select="'#ifdef '"/>
      <xsl:call-template name="uppercase" >
        <xsl:with-param name="string" select="@ifdef" />          
      </xsl:call-template>
      <xsl:value-of select="$newline" />    
    </xsl:if>
    <xsl:value-of select="'&amp;'" />
    <xsl:value-of select="@postfun" />
    <xsl:if test="@ifdef">
      <xsl:value-of select="$newline" />    
      <xsl:value-of select="'#else '"/>
      <xsl:value-of select="$newline" />    
      <xsl:value-of select="'NULL'" />
      <xsl:value-of select="$newline" />    
      <xsl:value-of select="'#endif'" /> 
      <xsl:value-of select="$newline" />    
    </xsl:if>
  </xsl:template>

  <xsl:template match="traversal" mode="posttable">
    <xsl:value-of select="', NULL'" />
  </xsl:template>

  <xsl:template match="/definition/phases//traversal" mode="travnames">
    <xsl:value-of select="', &quot;'" />
    <xsl:call-template name="lowercase" >
      <xsl:with-param name="string" select="@id" />
    </xsl:call-template>
    <xsl:value-of select="'&quot;'" />
  </xsl:template>

  <xsl:variable name="newline">
    <xsl:text>
    </xsl:text>
  </xsl:variable>

</xsl:stylesheet>
