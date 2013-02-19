<?xml version="1.0" encoding="utf-8"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text" version="1.0"
encoding="utf-8" indent="yes"/>

<xsl:template match="/">
    digraph my_fsm { 
    
    edge [color=blue]
    <xsl:for-each select="definition/nodesets/nodeset">
        <xsl:variable name="name">
            <xsl:value-of select="@name"/>
        </xsl:variable>
        <xsl:for-each select="target/node">
            <xsl:copy-of select="$name" /> -> <xsl:value-of select="@name"/>; 
        </xsl:for-each>
    </xsl:for-each>
    
    edge [color=black]
    <xsl:for-each select="definition/syntaxtree/node">
        <xsl:variable name="name">
            <xsl:value-of select="@name"/>
        </xsl:variable>
        <xsl:for-each select="sons/son">
            <xsl:variable name="label">
                [label = "<xsl:value-of select="@name"/>"]
            </xsl:variable>
            <xsl:for-each select="targets/target/set">
                <xsl:copy-of select="$name" /> -> <xsl:value-of select="@name"/> <xsl:copy-of select="$label"/>;
            </xsl:for-each>
            <xsl:for-each select="targets/target/node">
                <xsl:copy-of select="$name" /> -> <xsl:value-of select="@name"/> <xsl:copy-of select="$label"/>;
            </xsl:for-each>
        </xsl:for-each>
    </xsl:for-each>
    
    }
</xsl:template>

</xsl:stylesheet>
