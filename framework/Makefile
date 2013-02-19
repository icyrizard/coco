# ---------------------------------------------------------------------------
# 
# SAC Compiler Construction Framework
# 
# ---------------------------------------------------------------------------
# 
# SAC COPYRIGHT NOTICE, LICENSE, AND DISCLAIMER
# 
# (c) Copyright 1994 - 2011 by
# 
#   SAC Development Team
#   SAC Research Foundation
# 
#   http://www.sac-home.org
#   email:info@sac-home.org
# 
#   All rights reserved
# 
# ---------------------------------------------------------------------------
# 
# The SAC compiler construction framework, all accompanying 
# software and documentation (in the following named this software)
# is developed by the SAC Development Team (in the following named
# the developer) which reserves all rights on this software.
# 
# Permission to use this software is hereby granted free of charge
# exclusively for the duration and purpose of the course 
#   "Compilers and Operating Systems" 
# of the MSc programme Grid Computing at the University of Amsterdam.
# Redistribution of the software or any parts thereof as well as any
# alteration  of the software or any parts thereof other than those 
# required to use the compiler construction framework for the purpose
# of the above mentioned course are not permitted.
# 
# The developer disclaims all warranties with regard to this software,
# including all implied warranties of merchantability and fitness.  In no
# event shall the developer be liable for any special, indirect or
# consequential damages or any damages whatsoever resulting from loss of
# use, data, or profits, whether in an action of contract, negligence, or
# other tortuous action, arising out of or in connection with the use or
# performance of this software. The entire risk as to the quality and
# performance of this software is with you. Should this software prove
# defective, you assume the cost of all servicing, repair, or correction.
# 
# ---------------------------------------------------------------------------
# 




#######################################################################################
#
# general setup:
#

PROJECT_ROOT := .

include $(PROJECT_ROOT)/Makefile.Config
include $(PROJECT_ROOT)/Makefile.Targets

ALL_SOURCE_DIRS = $(addprefix src/,$(src))
SOURCE_DIRS     = $(addprefix src/,$(TARGET))
BIN_DIR         = $(PROJECT_ROOT)/bin
DOC_DIR         = $(PROJECT_ROOT)/doc

TARGETS = $(addprefix src/,$(foreach target,$(TARGET),$(addprefix $(target)/,$($(target)))))

SOURCE_MAKEFILES = $(addsuffix /Makefile,$(ALL_SOURCE_DIRS))

INCS         = $(patsubst %,-I%,$(ALL_SOURCE_DIRS))

XML_DIR     = $(PROJECT_ROOT)/src/framework
XML_COMMONS = $(wildcard $(XML_DIR)/common-*.xsl)
AST_DIR     = $(PROJECT_ROOT)/src/global

GENERATED_INCLUDE_FILES = $(patsubst %.xsl,%,$(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.h.xsl))) \
                          $(patsubst %.xsl,%,$(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.mac.xsl))) \
                          $(patsubst %.y,%.tab.h,$(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.y)))

GENERATED_SOURCE_FILES = $(patsubst %.xsl,%,$(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.c.xsl))) \
                         $(patsubst %.y,%.tab.c,$(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.y))) \
                         $(patsubst %.l,%.lex.c,$(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.l))) \

GENERATED_HTML_FILES = $(patsubst %.xsl,%,$(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.html.xsl)))

GENERATED_PNG_FILES = $(patsubst %.xsl,%,$(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.png.xsl)))

GENERATED_FILES = $(GENERATED_INCLUDE_FILES) $(GENERATED_SOURCE_FILES) $(GENERATED_HTML_FILES) $(GENERATED_PNG_FILES)

DEPENDENCY_FILES = $(patsubst %.o,%.d,$(TARGETS))

DEPS = $(foreach file,$(DEPENDENCY_FILES),$(dir $(file)).$(notdir $(file)))


  
###############################################################################
#
# Dummy rules
#

.PHONY: clean all devel make_devel %.track
.PHONY: check_os makefiles 
.PHONY: %.go makesubdir 

.PRECIOUS: %.c %.h %.o %.prod.o .%.d %.c %.mac %.lex.c %.tab.c %.tab.h



###############################################################################
#
# Start rules
#

all: make_devel

devel:
	@$(MAKE) CHECK_DEPS="yes" make_devel

make_devel: makefiles civicc.go 

makefiles: $(SOURCE_MAKEFILES)

src/%/Makefile: Makefile.Source
	@$(ECHO) "Creating makefile: $@"
	@cp -f $< $@

%.go: 
	@$(ECHO) ""
	@$(ECHO) "************************************"
	@$(ECHO) "* Making $*"
	@$(ECHO) "************************************"
	@touch make_track
	@$(ECHO) ""
	@$(ECHO) "Generating files in directory src/framework"
	@cd $(XML_DIR); $(MAKE)
	@$(MAKE) CHECK_DEPS="yes" MODE="$(suffix $*)" TARGET="$(src)" $*

civicc: $(TARGETS)
	@$(ECHO) ""
	@$(ECHO) "Linking civicc (developer version)"
	@$(ECHO) ""
	@$(CC) $(CCLINKFLAGS) -o $(BIN_DIR)/$@ $(TARGETS) $(LIB) $(LIBS)
	@$(RM) make_track


###############################################################################
#
# Rules for making subdirectories
#

makesubdir: $(TARGETS)
	@$(ECHO) ""


###############################################################################
#
# Rules for cleaning directories
#

clean: makefiles $(addsuffix .clean,$(ALL_SOURCE_DIRS))
	@$(RM) $(BIN_DIR)/civicc $(DOC_DIR)/ast.html $(DOC_DIR)/ast.png *~

%.clean:
	@$(ECHO) "Cleaning directory $*"
	@$(MAKE) CHECK_DEPS="no" -C $* clean


###############################################################################
#
# Pattern rules for compilation
#

%.o: %.c 
	@if [ ! -f make_track -o "$(dir $*)" != "`cat make_track`" ] ; \
         then $(ECHO) "$(dir $*)" > make_track; \
              $(ECHO) ""; \
              $(ECHO) "Compiling files in directory $(dir $@)" ; \
         fi
	@$(ECHO) "  Compiling developer code:  $(notdir $<)"
	@$(CC) $(CCFLAGS) $(CFLAGS) $(YYFLAGS) $(INCS) -o $@ -c $<



%.lex.c: %.l %.track
	@$(ECHO) "  Generating source code from LEX specification:  $(notdir $<)"
	@$(LEX) $<
	@mv lex.yy.c $@

%.tab.c: %.y %.track
	@$(ECHO) "  Generating source code from YACC specification:  $(notdir $<)"
	@$(YACC) $<
	@mv y.tab.c $@
	@$(RM) y.tab.h
	@mv y.output $(dir $@)

%.tab.h: %.y %.track
	@$(ECHO) "  Generating header file from YACC specification:  $(notdir $<)"
	@$(YACC) $<
	@mv y.tab.h $@
	@$(RM) y.tab.c 
	@mv y.output $(dir $@)

%.h: %.h.xsl $(AST_DIR)/ast.xml $(XML_COMMONS) 
	@$(ECHO) "  Generating header file from XML specification:  $(notdir $@)"
	@$(XSLTENGINE) $< $(AST_DIR)/ast.xml | $(INDENT) >$@

%.c: %.c.xsl $(AST_DIR)/ast.xml $(XML_COMMONS) 
	@$(ECHO) "  Generating source code from XML specification:  $(notdir $@)"
	@$(XSLTENGINE) $< $(AST_DIR)/ast.xml | $(INDENT) >$@

%.html: %.html.xsl $(AST_DIR)/ast.xml $(XML_COMMONS) 
	@$(ECHO) "  Generating html file from XML specification:  $(notdir $@)"
	@$(XSLTENGINE) $< $(AST_DIR)/ast.xml > $(DOC_DIR)/$(notdir $@)

%.png: %.png.xsl $(AST_DIR)/ast.xml $(XML_COMMONS) 
	@if [ $(DOT) = "no" ];\
		then \
			$(ECHO) "  Generating png file from XML specification:  $(notdir $@) failed. Please install Graphviz" ;\
		else \
            $(ECHO) "  Generating png file from XML specification:  $(notdir $@)" ; \
            $(XSLTENGINE)  $< $(AST_DIR)/ast.xml > $(DOC_DIR)/ast.dot; \
            dot  $(DOC_DIR)/ast.dot -Tpng -o  $(DOC_DIR)/$(notdir $@); \
            rm  $(DOC_DIR)/ast.dot;  \
		fi

%.track: 
	@if [ ! -f make_track -o "$(dir $*)" != "`cat make_track`" ] ; \
         then $(ECHO) "$(dir $*)" > make_track; \
              $(ECHO) ""; \
              $(ECHO) "Generating files in directory $(dir $@)" ; \
         fi



#######################################################################################
#
# Pattern rules for dependency tracking mechanism:
#

.%.d: %.c $(GENERATED_FILES) 
	@if [ ! -f make_track -o "$(dir $*)" != "`cat make_track`" ] ; \
        then $(ECHO) "$(dir $*)" > make_track; \
             $(ECHO) ""; \
             $(ECHO) "Checking dependencies in directory $(dir $@)" ; \
        fi
	@$(ECHO) "  Checking dependencies of source file: $(notdir $<)"
	@if $(CC) $(CCDEPS_FLAGS) $(CFLAGS) $(INCS) $<  > $@d ; \
	 then sed 's/\($(notdir $*)\)\.o[ :]*/$(subst /,\/,$*)\.o $(subst /,\/,$@)\: $$\(PROJECT_ROOT\)\/Makefile.Config /'  <$@d >$@; \
	      $(RM) $@d ; \
	 else $(RM) $@d ; \
	      exit 1 ;  \
	 fi





###############################################################################
#
# Includes for dependency tracking mechanism
#

ifeq ($(CHECK_DEPS),yes)
  ifneq ($(DEPS),)
    -include $(sort $(DEPS))
  endif
endif
