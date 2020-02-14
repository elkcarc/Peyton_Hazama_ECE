#/bin/make
###############################################################################
#            Make file for PROC files on lawtech.law.miami.edu
#	 		       Modified from the original procdemo
#	       		          Miguel A. Gavidia
###############################################################################
#  Usage :
#    make
#    make clean
#
#    To make use of any PROC options during precompilation, 
#        make PROCFLAGS="<list of proc options>" 
#
# NOTES: 
#    1. To change the executable name replace EXEC_NAME
#    2. Replace the contents in src/procdemo.pc or delete it and add your own .pc file
###############################################################################


CC=/usr/bin/gcc
cc=/usr/bin/gcc

# InstantClient Directories.
ICSDKHOME=$(ORACLE_HOME)/instantclient/sdk/
ICLIBHOME=lib/

MKLINK=ln
REMOVE=rm -rf
MKDIR=mkdir -p
CLNCACHE=cleancache
MAKE=make
MAKEFILE=Makefile

###############################################################################
# Executable name change to desired filename
###############################################################################
EXEC_NAME=final
###############################################################################
# Object directory
###############################################################################
OBJ_DIR=obj

PROC=proc
SO_EXT=.so
I_SYM=-I

CCINCLUDES= $(I_SYM)$(ICSDKHOME)include

# Pre-compiler Flags.
PRECOMP_INCLUDE=$(I_SYM). $(SYS_INCLUDE)
PRECOMPPUBH=$(ICSDKHOME)include
SYS_INCLUDE=sys_include=\($(PRECOMPPUBH),/usr/include,/usr/lib/gcc/x86_64-redhat-linux/4.8.2/include,/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include,/usr/lib/gcc/x86_64-redhat-linux/4.4.7/include,/usr/lib/gcc/x86_64-redhat-linux/4.4.4/include,/usr/lib64/gcc/x86_64-suse-linux/4.1.2/include,/usr/lib64/gcc/x86_64-suse-linux/4.3/include\)

# Compiler Flags.
OPTIMIZE=-O2
LDPATHFLAG=-L
SPFLAGS=-DLINUX -D_GNU_SOURCE -D_LARGEFILE64_SOURCE=1 -D_LARGEFILE_SOURCE=1 -DSLTS_ENABLE -DSLMXMX_ENABLE -D_REENTRANT -DNS_THREADS
CCFLAGS= -fPIC -DPRECOMP
LDFLAGS=-g
LPFLAGS=
GFLAG=
CDEBUG=
USRFLAGS=
ICLIBPATH=$(LDPATHFLAG)$(ICLIBHOME)
PFLAGS=$(CCINCLUDES) $(SPFLAGS) $(LPFLAGS)
CFLAGS=$(GFLAG) $(OPTIMIZE) $(CDEBUG) $(CCFLAGS) $(PFLAGS) $(USRFLAGS)

# Libraries.
PROLDLIBS=$(LDCLIENTLIBS) $(THREADLIBS)
LDCLIENTLIBS=$(ICLIBPATH) $(LLIBCLNTSH) $(LDLIBS)
LLIBCLNTSH=$(LDLIBFLAG)$(LIBCLNTSHNAME)
LDLIBFLAG=-l
LIBCLNTCORENAME=clntshcore
LIBCLNTSHNAME=clntsh
LDLIBS=$(EXSYSLIBS) $(MATHLIB) $(USRLIBS)
EXSYSLIBS=-ldl
MATHLIB=-lm
THREADLIBS=-lpthread

###############################################################################
# Default target build executable
###############################################################################
all: $(EXEC_NAME)

###############################################################################
# Build profiles
###############################################################################
PROC_FILES := $(wildcard src/final.pc)
SRC_FILES := $(addprefix $(OBJ_DIR)/,$(notdir $(PROC_FILES:.pc=.c)))
OBJ_FILES := $(addprefix $(OBJ_DIR)/,$(notdir $(PROC_FILES:.pc=.o)))

$(EXEC_NAME): $(OBJ_FILES)
	$(MKLINK) $(ICLIBHOME)libclntshcore$(SO_EXT).12.1 $(ICLIBHOME)libclntshcore$(SO_EXT)
	$(MKLINK) $(ICLIBHOME)libclntsh$(SO_EXT).12.1 $(ICLIBHOME)libclntsh$(SO_EXT)
	$(CC) -o $@ $< $(LDFLAGS) $(PROLDLIBS)
	$(REMOVE) $(ICLIBHOME)libclntshcore$(SO_EXT)
	$(REMOVE) $(ICLIBHOME)libclntsh$(SO_EXT)

$(OBJ_FILES): $(SRC_FILES)
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_FILES): $(PROC_FILES)
	$(MKDIR) $(OBJ_DIR)
	$(PROC) $(PROCFLAGS) iname=$(basename $<) oname=$@ lname=$(basename $@).lis include=. $(SYS_INCLUDE)

clean:
	$(REMOVE) $(EXEC_NAME) $(SRC_FILES) $(OBJ_FILES) $(basename $(SRC_FILES)).lis $(OBJ_DIR)

