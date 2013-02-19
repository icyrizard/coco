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
xmlns="http://www.w3.org/1999/xhtml" version="1.0">

  <xsl:import href="common-key-tables.xsl" />
  <xsl:import href="common-make-head.xsl" />
  <xsl:import href="common-name-to-nodeenum.xsl" />

  <!-- this xslt script generates a nice html view given the ast xml
       definition file. It is no good example to get an overview, as 
       it is pretty ugly. Refer to the c code generating scripts
       instead -->
  <xsl:output method="xml" version="1.0" indent="yes" 
              doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd" 
              doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN" />
  <xsl:strip-space elements="*" />

  <!-- definition of transformation templates -->
  <!-- this template creates the html surrounding -->
  <xsl:template match="/">
    <html>
      <head>
        <link rel="SHORTCUT ICON" href="http://www.sac-home.org/images/icons/favicon.ico" />
        <title>SaC Syntax Tree</title>
        <style type="text/css">
          body { font-family: sans-serif } 
          table { border-width: 1pt; border-style: solid;
                  border-color: #000000; background-color: #cccccc; 
                  padding: 0pt; margin: 20pt; width: 90% } 
          table.inner { border-width: 1pt; border-style: solid;
                        border-color: #000000; background-color: #cccccc; 
                        padding: 0pt; margin: 20pt; width: 100% } 
          tr { padding: 10pt; margin: 0pt; border-style: none} 
          td { vertical-align: top; padding: 4pt; margin: 0pt; 
               border-style: none } 
          td.title { background-color: #000000; color: #ffffff; 
                     font-weight: bold } 
          td.heading { background-color: #999999; font-weight:
                       bold } 
          div.description { font-size: smaller; }
          td.subheading { background-color: #999999; padding: 2pt;
                          margin: 0pt } 
          td.toclink { text-align: right; font-size: smaller; } 
          td.hidden { padding: 0pt } 
          td.ccode { font-family: courier; }
          table.hidden { border-style: none; margin: 0pt; width: 100%; 
                         padding: 2pt } 
          tr.nonmandatory { color: #666666; font-style: italic } 
          div.alert { display: inline; color: #ff0000 }
          a:link { text-decoration: underline }
          a:visited { text-decoration: underline }
          a.footnote { vertical-align: top; text-decoration: none }
          tr.footnote-heading { background-color: #999999; font-weight:
                       bold; font-size: small }
          tr.footnote { font-size: small }
          #Nav { position:fixed; top:0px; left:0px; padding:0px; 
                 margin:0px; font-size: smaller; background-color: #AAAAAA; }
          #miniNav { position:fixed; top:0px; right:0px; padding:0px; 
                     margin:0px; font-size: smaller; 
                     background-color: #AAAAAA; display: none; }
          #NavPhases{ position:fixed; padding: 0px;
                      margin:0px; font-size: smaller; 
                      background-color: #AAAAAA; display: none;
                      width: 200pt; max-height: 90%; overflow: scroll; }
          #NavSets{ position:fixed; padding: 0px;
                    margin:0px; font-size: smaller; 
                    background-color: #AAAAAA; display: none;
                    width: 200pt; max-height: 90%; overflow: scroll; }
          #NavNodes{ position:fixed; padding: 0px;
                     margin:0px; font-size: smaller; 
                     background-color: #AAAAAA; display: none;
                     width: 200pt; max-height: 90%; overflow: scroll; }
          #TOC{ display: none; }
          table.nav { border-width: 1pt; border-style: solid;
                      border-color: #000000; background-color: #cccccc; 
                      padding: 0pt; margin: 0pt; width: 100% } 
          table.mininav { border-width: 1pt; border-style: solid;
                          border-color: #000000; background-color: #cccccc; 
                          padding: 0pt; margin: 0pt; width: 10pt;}
          table.sub { border-width: 1pt; border-style: solid;
                      border-color: #000000; background-color: #cccccc; 
                      padding: 0pt; margin: 0pt; width: 100%;}
        </style>
        <script type="text/javascript" language="JavaScript1.2">
          function toggle( id) {
            if (document.getElementById(id).style.display != "block") {
              document.getElementById(id).style.display = "block";
            } else {
              document.getElementById(id).style.display = "none";
            }
          }
          function showTOC() {
            document.getElementById("TOC").style.display = "block";
            document.getElementById("showTOC").style.display = "none";
          }
          function noNav() {
            document.getElementById("Nav").style.display = "none";
            document.getElementById("miniNav").style.display = "block";
          }
          function showNav() {
            document.getElementById("Nav").style.display = "block";
            document.getElementById("miniNav").style.display = "none";
          }
          function cleanup() {
            alert("pup");
          }
        </script>
      </head>
      <body>
        <div id="Nav">
          <table class="nav">
            <tr>
              <td>
                QuickNav
              </td>
              <td>
                <a href="javascript:void (toggle('NavPhases'))" >Phases</a>
                <div id="NavPhases">
                  <table class="sub">
                    <tr><td>
                      <ul>
                        <xsl:apply-templates select="/definition/phases/phase" mode="list-of-tables" />
                      </ul>
                    </td></tr>
                  </table>
                </div>
              </td>
              <td>
                <a href="javascript:void (toggle('NavSets'))">Nodesets</a>
                <div id="NavSets" >
                  <table class="sub" >
                    <tr><td>
                      <ul>
                        <xsl:apply-templates select="/definition/nodesets/nodeset" mode="list-of-tables" />
                      </ul>
                    </td></tr>
                  </table>
                </div>
              </td>
              <td>
                <a href="javascript:void (toggle('NavNodes'))">Nodes</a>
                <div id="NavNodes" >
                  <table class="sub" >
                    <tr><td>
                      <ul>
                        <xsl:apply-templates select="/definition/syntaxtree/node" mode="list-of-tables" >
                          <xsl:sort select="@name" />
                        </xsl:apply-templates>
                      </ul>
                    </td></tr>
                  </table>
                </div>
              </td>
              <td>
                <div style="text-align: right;">
                  <a href="javascript:void( noNav())">hide &gt;&gt;</a>
                </div>
              </td>
            </tr>
          </table>
        </div>
        <div id="miniNav">
          <table class="mininav">
            <tr><td>
              <a href="javascript: void( showNav())">&lt;&lt;</a>
            </td></tr>
          </table>
        </div>
        <div id="showTOC">
          In case QuickNav is not working as intended, click <a href="javascript:showTOC()">here</a> for a table of contents.
        </div>
        <a name="toc" />
        <div id="TOC">
          <h1>List Of Tables</h1>
          <ul>
            <xsl:apply-templates mode="list-of-tables" />
          </ul>
          <h1>Tables</h1>
        </div>
        <xsl:apply-templates mode="table" />
      </body>
    </html>
  </xsl:template>
  <!-- in list-of-tables mode, attribute types generates a link -->
  <xsl:template match="attributetypes" mode="list-of-tables">
    <li>
      <a href="#attributetypes">Attribute Types</a>
    </li>
  </xsl:template>
  <!-- each nodeset generates a link, as well -->
  <xsl:template match="nodesets" mode="list-of-tables">
    <li> Nodesets
      <ul>
        <xsl:apply-templates mode="list-of-tables" />
      </ul>
    </li>
  </xsl:template>

  <xsl:template match="nodeset" mode="list-of-tables">
    <li>
      <xsl:element name="a">
        <xsl:attribute name="href">
          <xsl:value-of select="'#'" />
          <xsl:value-of select="@name" />
        </xsl:attribute>
        <xsl:value-of select="'{'" />
        <xsl:value-of select="@name" />
        <xsl:value-of select="'}'" />
      </xsl:element>
    </li>
  </xsl:template>
  <!-- we sort the nodes by name using this template -->
  <xsl:template match="syntaxtree" mode="list-of-tables">
    <li>
      Nodes
      <ul>
        <xsl:apply-templates select="node" mode="list-of-tables">
          <xsl:sort select="@name" />
        </xsl:apply-templates>
      </ul>
    </li>
  </xsl:template>
  <!-- in list-of-tables mode, each node generates a link -->
  <xsl:template match="node" mode="list-of-tables">
    <li>
      <xsl:element name="a">
        <xsl:attribute name="href">
          <xsl:value-of select="'#'" />
          <xsl:value-of select="@name" />
        </xsl:attribute>
        <xsl:call-template name="name-to-nodeenum" >
          <xsl:with-param name="name" >
            <xsl:value-of select="@name" />
          </xsl:with-param>
        </xsl:call-template>
      </xsl:element>
    </li>
  </xsl:template>

  <xsl:template match="phases" mode="list-of-tables">
    <li>
      Phases
      <ul>
        <xsl:apply-templates mode="list-of-tables" />
      </ul>
    </li>
  </xsl:template>

  <xsl:template match="phase" mode="list-of-tables" >
    <li>
      <xsl:element name="a">
        <xsl:attribute name="href">
          <xsl:value-of select="'#PH_'" />
          <xsl:value-of select="@id" />
        </xsl:attribute>
        <xsl:value-of select="@name" />
      </xsl:element>
      <ul>
        <xsl:apply-templates mode="list-of-tables" />
      </ul>
    </li>
  </xsl:template>

  <xsl:template match="general" mode="list-of-tables" >
    <li>
      <xsl:element name="a">
        <xsl:attribute name="href">
          <xsl:value-of select="'#GENERALTRAVS'" />
        </xsl:attribute>
        General Traversals
      </xsl:element>
      <ul>
        <xsl:apply-templates mode="list-of-tables" />
      </ul>
    </li>
  </xsl:template>

  <xsl:template match="traversal" mode="list-of-tables">
    <li>
      <xsl:element name="a">
        <xsl:attribute name="href">
          <xsl:value-of select="'#TR_'" />
          <xsl:value-of select="@id" />
        </xsl:attribute>
        <xsl:value-of select="@name" />
      </xsl:element>
    </li>
  </xsl:template>

  <!-- from here on we define table mode -->
  <!-- attributetypes is tranformed to an html table -->
  <xsl:template match="attributetypes" mode="table">
    <a name="attributetypes" />
    <table>
      <tr>
        <td colspan="4" class="title">Attribute
        Types</td>
      </tr>
      <tr>
        <td class="heading">Name</td>
        <td class="heading">Name of C Type</td>
        <td class="heading">Initial Value</td>
        <td class="heading">Class of Type</td>
      </tr>
      <xsl:apply-templates select="type" mode="table" />
      <tr class="footnote-heading">
        <td colspan="4">
          <a name="xpln_cp" />
          Possible Classes of Type
        </td>
      </tr>
      <tr class="footnote">
        <td>literal</td>
        <td colspan="3">
          types of class literal can be processed like basic c types (e.g. int)
        </td>
      </tr>
      <tr class="footnote">
        <td>function</td>
        <td colspan="3">
          types of class function need a special function to be processed
        </td>
      </tr>
      <tr class="footnote">
        <td>hash</td>
        <td colspan="3">
          types of class hash are references to nodes in the tree and thus
          have to be processed using a hash table
        </td>
      </tr>
      <tr>
        <td colspan="4" class="toclink">
          <a href="#toc" class="toclink">top</a>
        </td>
      </tr>
    </table>
  </xsl:template>

  <!-- each type is tranformed into a row -->
  <xsl:template match="attributetypes/type" mode="table">
    <tr>
      <td>
        <xsl:element name="a">
          <xsl:attribute name="name">atype_<xsl:value-of select="@name" /></xsl:attribute>
        </xsl:element>
        <xsl:value-of select="@name" />
      </td>
      <td>
        <xsl:value-of select="@ctype" />
        <xsl:if test="@size">
          <xsl:value-of select="concat( concat( '[', @size), ']')"/>
        </xsl:if>
      </td>
      <td>
        <xsl:value-of select="@init" />
      </td>
      <td>
        <xsl:value-of select="@copy" /><a class="footnote" href="#xpln_cp">*</a>
      </td>
    </tr>
  </xsl:template>

  <!-- phases are transfered into a table -->
  <xsl:template match="phase" mode="table">
    <xsl:element name="a">
      <xsl:attribute name="name">
        <xsl:value-of select="'PH_'" />
        <xsl:value-of select="@id" />
      </xsl:attribute>
    </xsl:element>
    <table>
      <tr>
        <td class="title">
          <xsl:value-of select="'Phase PH_'" />
          <xsl:value-of select="@id" />
          <div class="description">
            <xsl:value-of select="@name" />
          </div>
        </td>
      </tr>
      <tr>
        <td>
          <xsl:apply-templates select="traversal" mode="table" />
        </td>
      </tr>
    </table>
  </xsl:template>

  <xsl:template match="general" mode="table">
    <xsl:element name="a">
      <xsl:attribute name="name">
        <xsl:value-of select="'GENERALTRAVS'" />
      </xsl:attribute>
    </xsl:element>
    <table>
      <tr>
        <td class="title">
          General Traversals
        </td>
      </tr>
      <tr>
        <td>
          <xsl:apply-templates select="traversal" mode="table" />
        </td>
      </tr>
    </table>
  </xsl:template>

  <xsl:template match="traversal" mode="table">
    <table class="inner">
      <tr>
        <td class="heading">
          <xsl:element name="a">
            <xsl:attribute name="name">
              <xsl:value-of select="'TR_'" />
              <xsl:value-of select="@id" />
            </xsl:attribute>
            <xsl:value-of select="@name" />
          </xsl:element>
        </td>
      </tr>
    </table>
  </xsl:template>
  
    
  <!-- each nodeset is transformed into a table -->
  <xsl:template match="nodeset" mode="table">
    <xsl:element name="a">
      <xsl:attribute name="name">
        <xsl:value-of select="@name" />
      </xsl:attribute>
    </xsl:element>
    <table>
      <tr>
        <td class="title">Nodeset 
          <xsl:value-of select="@name" />
          <xsl:apply-templates select="description" mode="table" />
        </td>
      </tr>
      <tr>
        <td>
          <xsl:apply-templates select="target/node" mode="table-target" />
        </td>
      </tr>
      <tr>
        <td class="toclink">
          <a href="#toc" class="toclink">top</a>
        </td>
      </tr>
    </table>
  </xsl:template>
 
  <!-- sort all nodes -->
  <xsl:template match="syntaxtree" mode="table">
    <xsl:apply-templates select="node" mode="table">
      <xsl:sort select="@name"/>
    </xsl:apply-templates>
  </xsl:template>

  <!-- each node is transformed to a table -->
  <xsl:template match="node" mode="table">
    <xsl:element name="a">
      <xsl:attribute name="name"><xsl:value-of select="@name" /></xsl:attribute>
    </xsl:element>
    <table>
      <tr>
        <td class="title">
          <xsl:call-template name="name-to-nodeenum" >
            <xsl:with-param name="name" >
              <xsl:value-of select="@name" />
            </xsl:with-param>
          </xsl:call-template>
          <xsl:apply-templates select="description" mode="table" />
        </td>
      </tr>
      <tr>
        <td class="heading">
          Make Function
        </td>
      </tr>
      <tr>
        <td class="ccode">
          <xsl:apply-templates select="." mode="make-head" />
        </td>
      </tr>
      <xsl:if test="sons/son">
        <tr>
          <td class="heading">Sons</td>
        </tr>
        <tr>
          <td>
            <table class="hidden">
              <tr>
                <td class="subheading">Name</td>
                <td class="subheading">Possible Target Nodes</td>
                <td class="subheading">Default</td>
                <td class="subheading">Comment</td>
              </tr>
              <xsl:apply-templates select="sons" mode="table" />
            </table>
          </td>
        </tr>
      </xsl:if>
      <xsl:if test="attributes/attribute[type/targets/target/phases/all]">
        <tr>
          <td class="heading">Permanent Attributes</td>
        </tr>
        <tr>
          <td>
            <table class="hidden">
              <tr>
                <td class="subheading">Name</td>
                <td class="subheading">Type</td>
                <td class="subheading">Default</td>
                <td class="subheading">Comment</td>
              </tr>
              <xsl:apply-templates select="attributes/attribute[type/targets/target/phases/all]"
              mode="table" />
            </table>
          </td>
        </tr>
      </xsl:if>
      <xsl:if test="attributes/attribute[not(type/targets/target/phases/all)]">
        <tr>
          <td class="heading">Temporary Attributes</td>
        </tr>
        <tr>
          <td>
            <table class="hidden">
              <tr>
                <td class="subheading">Name</td>
                <td class="subheading">Type</td>
                <td class="subheading">Phases</td>
                <td class="subheading">Default</td>
                <td class="subheading">Comment</td>
              </tr>
              <xsl:apply-templates select="attributes/attribute[not( type/targets/target/phases/all)]" mode="table" />
            </table>
          </td>
        </tr>
      </xsl:if>
      <xsl:if test="flags/flag" >
        <tr>
          <td class="heading">Flags</td>
        </tr>
        <tr>
          <td>
            <table class="hidden">
              <tr>
                <td class="subheading">Name</td>
                <td class="subheading">Default</td>
                <td class="subheading">Comment</td>
              </tr>
              <xsl:apply-templates select="flags/flag" mode="table" />
            </table>
          </td>
        </tr>
      </xsl:if>
      <tr>
        <td class="toclink">
          <a href="#toc" class="toclink">top</a>
        </td>
      </tr>
    </table>
  </xsl:template>

  <!-- sons are transformed into rows of a table -->
  <xsl:template match="son" mode="table">
    <xsl:element name="tr">
      <xsl:if test="@mandatory = 'yes'">
        <xsl:attribute name="class">
          <xsl:value-of select="'mandatory'" />
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="@mandatory = 'no'">
        <xsl:attribute name="class">
          <xsl:value-of select="'nonmandatory'" />
        </xsl:attribute>
      </xsl:if>
      <td>
        <xsl:value-of select="@name" />
      </td>
      <td>
        <xsl:apply-templates select="targets/target" mode="table-target" />
      </td>
      <td class="ccode" >
        <xsl:value-of select="@default" />
      </td>
      <td>
        <xsl:value-of select="description" />
      </td>
    </xsl:element>
  </xsl:template>

  <!-- a target node is transformed into a reference to that node -->
  <xsl:template match="target/node" mode="table-target">
    <xsl:value-of select="' '" />
    <xsl:element name="a">
      <xsl:attribute name="href">
       <xsl:value-of select="'#'" />
       <xsl:value-of select="@name" />
      </xsl:attribute>
      <xsl:call-template name="name-to-nodeenum" >
        <xsl:with-param name="name" >
          <xsl:value-of select="@name" />
        </xsl:with-param>
      </xsl:call-template>
    </xsl:element>
  </xsl:template>

  <!-- a target set is transformed into a reference to that node -->
  <xsl:template match="target/set" mode="table-target">
    <xsl:element name="a">
      <xsl:attribute name="href">
        <xsl:value-of select="'#'" />
        <xsl:value-of select="@name" />
      </xsl:attribute>
      <xsl:value-of select="'{'" />
      <xsl:value-of select="@name" />
      <xsl:value-of select="'}'" />
    </xsl:element>
  </xsl:template>

  <!-- a target any is transformed into any -->
  <xsl:template match="target/any" mode="table-target">
    <xsl:value-of select="'any'" />
  </xsl:template>

  <!-- attributes are transformed similar to sons -->
  <xsl:template match="attribute" mode="table">
    <xsl:element name="tr">
      <xsl:if test="@mandatory = 'yes'">
        <xsl:attribute name="class">
          <xsl:value-of select="'mandatory'" />
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="@mandatory = 'no'">
        <xsl:attribute name="class">
          <xsl:value-of select="'nonmandatory'" />
        </xsl:attribute>
      </xsl:if>
      <td>
        <xsl:value-of select="@name" />
      </td>
      <td>
        <xsl:apply-templates select="type" mode="table" />
      </td>
      <xsl:if test="not(type/targets/target/phases/all)">
        <td>
          <xsl:apply-templates select="type/targets/target/phases" mode="table-node" />
        </td>
      </xsl:if>
      <td class="ccode" >
        <xsl:value-of select="@default" />
      </td>
      <td>
        <xsl:value-of select="description" />
      </td>
    </xsl:element>
  </xsl:template>

  <!-- attribute types a transformed into references to type table -->
  <xsl:template match="type" mode="table">
    <div class="type">
      <xsl:element name="a">
        <xsl:attribute name="href">
          <xsl:value-of select="'#atype_'" />
          <xsl:value-of select="@name" />
        </xsl:attribute>
        <xsl:value-of select="@name" />
      </xsl:element>
      <xsl:apply-templates select="targets/target"
      mode="attrib-table" />
    </div>
  </xsl:template>

  <!-- target is transformed into a small list -->
  <xsl:template match="target" mode="attrib-table">
    <xsl:value-of select="' ( '" />
    <xsl:apply-templates select="target" mode="table-target" />
    <xsl:value-of select="' ) '" />
  </xsl:template>

  <!-- attribute phase information is just printed -->
  <xsl:template match="phases/unknown" mode="table-node">
    <xsl:value-of select="'[ '" />
    <div class="alert">
      <xsl:value-of select="'unknown'" />
    </div>
    <xsl:value-of select="' ]'" />
  </xsl:template>
  <xsl:template match="phases/phase" mode="table-node">
    <xsl:value-of select="concat( concat( '[ ', @name ), ' ]')" />
  </xsl:template>
  <xsl:template match="phases/range" mode="table-node">
    <xsl:value-of select="'[ '" />
    <xsl:value-of select="concat( @from, ' - ')" />
    <xsl:value-of select="concat( @to, ' ]')" />
  </xsl:template>

  <!-- flags are matched into a row of a table -->
  <xsl:template match="flags/flag" mode="table" >
    <tr>
      <td>
        <xsl:value-of select="@name" />
      </td>
      <td class="ccode" >
        <xsl:if test="not( @default)" >
          <xsl:value-of select="'FALSE'" />
        </xsl:if>
        <xsl:value-of select="@default" />
      </td>
      <td>
        <xsl:value-of select="description" />
      </td>
    </tr>
  </xsl:template>

  <!-- general template for descriptions -->
  <xsl:template match="description" mode="table">
    <div class="description">
      <xsl:value-of select="text()" />
    </div>
  </xsl:template>

  <!-- general template for unknown tags -->
  <xsl:template match="unknown" mode="table">
    <div class="alert">unknown</div>
  </xsl:template>
</xsl:stylesheet>
