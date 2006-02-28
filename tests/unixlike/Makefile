###############################################################################
#
#	File    : $Id: Makefile,v 1.37.2.5 2005/09/27 09:46:42 epocman Exp $
#	Subject : IBPP 'tests' - Multi-Platform GNU Makefile
#
###############################################################################
#
#	The contents of this file are subject to the Mozilla Public License
#	Version 1.0 (the "License"); you may not use this file except in
#	compliance with the License. You may obtain a copy of the License at
#	http://www.mozilla.org/MPL/
#
#	Software distributed under the License is distributed on an "AS IS"
#	basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
#	License for the specific language governing rights and limitations
#	under the License.
#
#	The Original Code is "IBPP 0.9" and all its associated documentation.
#
#	The Initial Developer of the Original Code is T.I.P. Group S.A.
#	Portions created by T.I.P. Group S.A. are
#	Copyright (C) 2000 T.I.P Group S.A.
#	All Rights Reserved.
#
#	Contributor(s):	Matt Hortman, initial developer of this Makefile
#	                Olivier Mascia, maintainer
#
###############################################################################
#
#	COMMENTS
#
#	* Tabulations should be set every four characters when editing this file.
#	* For instructions, read the howtobuild.txt file. Some comments in this
#	* file can also be usefull if you have to tweak the build procedure to
#	* your needs.
#
###############################################################################

#
#	Select Compiler and Platform
#	This section works in conjunction with the 'checks' target to verify
#	the automatic detection of the platform and the manual selection of the
#	compiler. Read the howtobuild.txt file for useage information.
#

ifneq ($(IBPP_GCC),)
	COMPILER=IBPP_GCC
endif
ifneq ($(IBPP_BCC),)
	COMPILER=IBPP_BCC
endif
ifneq ($(IBPP_MSVC),)
	COMPILER=IBPP_MSVC
endif
ifneq ($(IBPP_DMC),)
	COMPILER=IBPP_DMC
endif

ifeq ($(shell uname),Darwin)
	#	Darwin (OS X) Target Platform
	PLATFORM=darwin
	COMPILER=IBPP_GCC
else
ifeq ($(shell uname),Linux)
	#	Linux Target Platform
	PLATFORM=linux
	COMPILER=IBPP_GCC	# The only compiler supported on linux anyway
else
	#	Win32 Target Platform
	ifeq ($(COMPILER),IBPP_MSVC)
		PLATFORM=win32_msvc
	else
	ifeq ($(COMPILER),IBPP_BCC)
		PLATFORM=win32_bcc
	else
	ifeq ($(COMPILER),IBPP_DMC)
		PLATFORM=win32_dmc
	else
		ifeq ($(findstring mingw32,$(shell gcc -v 2>&1)),mingw32)
			PLATFORM=win32_mingw
		else	
		ifeq ($(findstring cygwin,$(shell gcc -v 2>&1)),cygwin)
			PLATFORM=win32_cygwin
		endif
		endif
	endif
	endif
	endif
endif	
endif

# set target directories
ROOTDIR:=$(shell pwd)
ROOTDIR:=$(subst \,/,$(ROOTDIR))
ifeq ($(DEBUG),)
	DEBUG=0
endif
ifneq ($(DEBUG),0)
	TARGETDIR=debug
	DS=d
else
	TARGETDIR=release
	DS=
endif

# *************************************************
# Source Files
# *************************************************

HDRS =	ibpp.h
HDRS +=	_internals.h
HDRS +=	_msvcwarnings.h

SRCS =	_dpb.cpp
SRCS +=	_ibs.cpp
SRCS +=	_rb.cpp
SRCS +=	_spb.cpp
SRCS +=	_tpb.cpp
SRCS +=	_epb.cpp
SRCS +=	array.cpp
SRCS +=	blob.cpp
SRCS +=	database.cpp
SRCS +=	dbkey.cpp
SRCS +=	exception.cpp
SRCS +=	ibpp.cpp
SRCS +=	service.cpp
SRCS += row.cpp
SRCS +=	statement.cpp
SRCS +=	transaction.cpp

SRCS +=	date.cpp
SRCS +=	time.cpp

# *************************************************
# Conditional Compile Options
# *************************************************

# building on darwin (OS X)
ifeq ($(PLATFORM),darwin)
	CXXOUTPUT =	-o
	CXXFLAGS =-DIBPP_DARWIN
	CXXFLAGS+=-DIBPP_GCC
	CXXFLAGS+=-Wall -fPIC
	CXXFLAGS+=-I./ -I./fbheaders
	#
	LIBS+=-framework Firebird -lm -ldl -lpthread
	ifeq ($(TARGETDIR),release)
		CXXFLAGS+= -O3
	else
		CXXFLAGS+= -g -DDEBUG
	endif
	#
	TARGETS =	$(TARGETDIR)/$(PLATFORM)/libibpp$(DS).a \
				$(TARGETDIR)/$(PLATFORM)/tests
endif

# building on linux (any flavour, I suppose)
ifeq ($(PLATFORM),linux)
	CXXOUTPUT =	-o
	CXXFLAGS =-DIBPP_LINUX
	CXXFLAGS+=-DIBPP_GCC
	CXXFLAGS+=-Wall -fPIC
	CXXFLAGS+=-I./ -I./fbheaders
	#
	# change -lfbclient by -lgds if building with Firebird 1.x or InterBase
	# -lfbclient (or -lgds) is for Firebird 1.5 SS and higher
	# -lfbembed is for Firebird 1.5 CS and higher
	#
	LIBS+=-lfbclient -lcrypt -lm -ldl -lpthread # dependancies for shared library
	ifeq ($(TARGETDIR),release)
		CXXFLAGS+= -O3
	else
		CXXFLAGS+= -g -DDEBUG
	endif
	#
	TARGETS =	$(TARGETDIR)/$(PLATFORM)/libibpp$(DS).a \
				$(TARGETDIR)/$(PLATFORM)/libibpp$(DS).so \
				$(TARGETDIR)/$(PLATFORM)/tests
endif

# building with mingw (MinGW 3.0)
ifeq ($(PLATFORM),win32_mingw)
	CXXOUTPUT =	-o
	CXXFLAGS =-DIBPP_WINDOWS
	CXXFLAGS+=-DIBPP_GCC
	CXXFLAGS+=-mthreads -D_MT -DWIN32_LEAN_AND_MEAN -I./ -I./fbheaders
	ifeq ($(TARGETDIR),release)
		CXXFLAGS+= -O3
	else
		CXXFLAGS+= -ggdb -DDEBUG -Wall
	endif
	#
	TARGETS =	$(TARGETDIR)/$(PLATFORM)/libibpp$(DS).a \
				$(TARGETDIR)/$(PLATFORM)/tests.exe
endif

# building with cygwin
ifeq ($(PLATFORM),win32_cygwin)
	CXXOUTPUT =	-o
	CXXFLAGS =-DIBPP_WINDOWS
	CXXFLAGS+=-DIBPP_GCC
	CXXFLAGS+=-D_MT -DWIN32_LEAN_AND_MEAN -I./ -I./fbheaders
	ifeq ($(TARGETDIR),release)
		CXXFLAGS+= -O3
	else
		CXXFLAGS+= -ggdb -DDEBUG -Wall
	endif
	#
	TARGETS =	$(TARGETDIR)/$(PLATFORM)/libibpp$(DS).a
				$(TARGETDIR)/$(PLATFORM)/tests.exe
endif

# building on win32_bcc (Borland C++ Builder)
ifeq ($(PLATFORM),win32_bcc)
	COMPDIR =	"/c/Program Files/Borland/CBuilder6"
	CXX = 		bcc32.exe
	CXXOUTPUT =	-o
	CXXFLAGS =	-DIBPP_WINDOWS
	CXXFLAGS +=	-DIBPP_BCC
	CXXFLAGS +=	-q -6 -w -a8 -H -H=$(ROOTDIR)/$(TARGETDIR)/$(PLATFORM)/bcc.csm -pc -tWM \
			   	-DWIN32 -D_WINDOWS \
			   	-I./ -I./fbheaders
	ifeq ($(TARGETDIR),release)
		CXXFLAGS +=	-O2 -OS -DNDEBUG
	else
		CXXFLAGS +=	-Od -r- -k -v -y -vi- -D_DEBUG
		CXXFLAGS += -vG
	endif
	#
	TARGETS =	$(TARGETDIR)/$(PLATFORM)/ibpp$(DS).lib \
				$(TARGETDIR)/$(PLATFORM)/tests.exe
endif

# building on win32_msvc (Microsoft Visual Studio 6.0 or 7.1)
ifeq ($(PLATFORM),win32_msvc) 
	CXX = 		cl.exe
	CXXOUTPUT =	-Fo
	CXXFLAGS =	-DIBPP_WINDOWS
	CXXFLAGS +=	-DIBPP_MSVC
	CXXFLAGS +=	-nologo -YX -GX -G6 -GR -Zp8 \
				-DWIN32 -D_WINDOWS \
				-I./ -I./fbheaders
	ifeq ($(TARGETDIR),release)
		CXXFLAGS +=	-MD -Ox -Oi- -FI _msvcwarnings.h -DNDEBUG -W4
	else
		CXXFLAGS +=	-MDd -Od -GZ -Zi -FI _msvcwarnings.h -D_DEBUG -W4
	endif
	#
	TARGETS =	$(TARGETDIR)/$(PLATFORM)/ibpp$(DS).lib \
				$(TARGETDIR)/$(PLATFORM)/tests.exe
endif

# building on win32_dmc (Digital Mars Compiler)
ifeq ($(PLATFORM),win32_dmc)
	COMPDIR =	c:/dm/bin
	CXX = 		$(COMPDIR)/dmc.exe
	CXXOUTPUT =	-o
	CXXFLAGS =	-DIBPP_WINDOWS
	CXXFLAGS +=	-DIBPP_DMC
	CXXFLAGS +=	-mn -Aa -Ab -Ae -Ar -a8 \
			   	-DWIN32 -D_WINDOWS \
			   	-D_MT -DWIN32_LEAN_AND_MEAN -I./ -I./fbheaders
	ifeq ($(TARGETDIR),release)
		CXXFLAGS +=	-o+all -DNDEBUG
	else
		CXXFLAGS +=	-o+none -g -D_DEBUG
	endif
	#
	TARGETS =	$(TARGETDIR)/$(PLATFORM)/ibpp$(DS).lib \
				$(TARGETDIR)/$(PLATFORM)/tests.exe
endif

# *************************************************
# Derived Definitions
# *************************************************

# make an object from each source file
ifeq ($PLATFORM),win32_dmc)
OBJS:=$(addprefix $(TARGETDIR)\\$(PLATFORM)\\,$(addsuffix .o,$(basename $(SRCS))))
else
OBJS:=$(addprefix $(TARGETDIR)/$(PLATFORM)/,$(addsuffix .o,$(basename $(SRCS))))
endif

# *************************************************
# Build Rules
# *************************************************

# don't check for existance of files named:
.PHONY: checks tests clean reallyclean runtests

#don't delete when generated indirectly
.SECONDARY: $(HDRS) $(SRCS)

all: checks $(TARGETS)

checks:
ifeq ($(PLATFORM),)
	$(warning Unsupported or unrecognized platform.)
	$(error Please read the howtobuild.txt file)
endif
ifeq ($(COMPILER),)
	$(warning Unspecified compiler choice.)
	$(error Please read the howtobuild.txt file)
endif
	@if [ ! -d $(TARGETDIR)/$(PLATFORM) ]; then mkdir -p $(TARGETDIR)/$(PLATFORM); fi
	@echo ""
	@echo "Platform : $(PLATFORM)"
	@echo "Compiler : $(COMPILER)"
	@echo "Target   : $(TARGETDIR)"
	@echo ""

#
#	PRODUCTION RULES : TRULY COMMON TO ALL PLATFORMS
#

# disable internal rules
%.o: %.cpp

# our own specific compilation rule
$(TARGETDIR)/$(PLATFORM)/%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) $(CXXOUTPUT)$@ -c $<

#
#	PRODUCTION RULES : SHARED BETWEEN LINUX, MINGW, CYGWIN
#

ifeq ($(findstring $(PLATFORM),linux darwin win32_mingw win32_cygwin),$(PLATFORM))
$(TARGETDIR)/$(PLATFORM)/libibpp$(DS).a: $(OBJS)
	@rm -f $@
	@ar rv $@ $^
	@ranlib $@

	@echo ""
	@echo "$@ *** Build Successful ***"
	@echo ""
endif

#
#	PRODUCTION RULES : SHARED BETWEEN LINUX, DARWIN
#

ifeq ($(findstring $(PLATFORM),linux darwin),$(PLATFORM))
$(TARGETDIR)/$(PLATFORM)/libibpp$(DS).so: $(OBJS)
	$(CXX) $^ $(LDFLAGS) -shared -o $@ $(LIBS)

	@echo ""
	@echo "$@ *** Build Successful ***"
	@echo ""

$(TARGETDIR)/$(PLATFORM)/tests : $(TARGETDIR)/$(PLATFORM)/libibpp$(DS).a tests/tests.cpp
	cd $(TARGETDIR)/$(PLATFORM);\
		$(CXX) $(CXXFLAGS) -I../../ -o tests ../../tests/tests.cpp -L./ -libpp$(DS) $(LIBS)
endif

#
#	PRODUCTION RULES : SHARED BETWEEN MINGW, CYGWIN
#

ifeq ($(findstring $(PLATFORM),win32_mingw win32_cygwin),$(PLATFORM))
$(TARGETDIR)/$(PLATFORM)/tests.exe : $(TARGETDIR)/$(PLATFORM)/libibpp$(DS).a tests/tests.cpp
	cd $(TARGETDIR)/$(PLATFORM);\
		$(CXX) $(CXXFLAGS) -I../../ -o tests.exe ../../tests/tests.cpp -L./ -libpp$(DS)
endif

#
#	PRODUCTION RULES : WIN32_BCC SPECIFIC
#

$(TARGETDIR)/win32_bcc/ibpp$(DS).lib: $(OBJS)
	@rm -f $@
#	Build a response file for tlib.exe
	@rm -f $@.txt
	@$(foreach obj,$^,echo +$(subst /,\\\\,$(obj)) \& >>$@.txt;)
	@echo "+" >>$@.txt
#	Build a script to run tlib.exe
#	tlib won't run properly unless ran from a Win32 command interpreter
	@echo "tlib $(subst /,\\,$@) /C /P128 $(subst /,\\,@$@.txt)" >tlibx.bat
#	If you use Win98, Cygwin and BCC, change the following line to:
#	@start /w tlibx.bat
	@cmd.exe "/C tlibx.bat"
	@rm -f tlibx.bat

	@echo ""
	@echo "$@ *** Build Successful ***"
	@echo ""

$(TARGETDIR)/win32_bcc/tests.exe : $(TARGETDIR)/$(PLATFORM)/ibpp$(DS).lib tests/tests.cpp
	cd $(TARGETDIR)/$(PLATFORM);\
		$(CXX) $(CXXFLAGS) -I../../ ../../tests/tests.cpp ibpp$(DS).lib

#
#	PRODUCTION RULES : WIN32_MSVC SPECIFIC
#

$(TARGETDIR)/win32_msvc/ibpp$(DS).lib: $(OBJS)
	@rm -f $@
	@lib.exe -OUT:$@ $(OBJS)

	@echo ""
	@echo "$@ *** Build Successful ***"
	@echo ""

$(TARGETDIR)/win32_msvc/tests.exe : $(TARGETDIR)/$(PLATFORM)/ibpp$(DS).lib tests/tests.cpp
	cd $(TARGETDIR)/$(PLATFORM);\
		$(CXX) $(CXXFLAGS) -I../../  ../../tests/tests.cpp ibpp$(DS).lib advapi32.lib

#
#	PRODUCTION RULES : WIN32_DMC SPECIFIC
#

$(TARGETDIR)/win32_dmc/ibpp$(DS).lib: $(OBJS)
	@rm -f $@
#	dmc lib won't run properly unless ran from a Win32 command interpreter
	@echo "$(subst /,\\\\,$(COMPDIR)/lib -c -n -p256 $@ $(OBJS))" >libx.bat
	@cmd.exe "/C libx.bat"
	@rm -f libx.bat

	@echo ""
	@echo "$@ *** Build Successful ***"
	@echo ""

$(TARGETDIR)/win32_dmc/tests.exe : $(TARGETDIR)/$(PLATFORM)/ibpp$(DS).lib tests/tests.cpp
	cd $(TARGETDIR)/$(PLATFORM);\
		$(CXX) $(CXXFLAGS) -I../../ ../../tests/tests.cpp ibpp$(DS).lib advapi32.lib

#
#	PRODUCTION RULES : SPECIAL TARGETS
#

clean: checks
	@rm -rf $(TARGETDIR)/$(PLATFORM)
	@echo "CLEANED ./$(TARGETDIR)/$(PLATFORM)"

reallyclean:
	@rm -rf debug 
	@rm -rf release
	@echo "REALLY CLEANED ./debug and ./release"

runtests: checks $(TARGETS)
	@echo ""
	@echo "Now running tests programs..."
	@cd $(TARGETDIR)/$(PLATFORM); ./tests

#
#	EOF
#
