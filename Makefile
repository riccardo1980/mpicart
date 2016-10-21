# Simple Makefile for C++ projects
# 
# Each .cpp source is compiled in a separate 
# translation unit. Each .o depends on the 
# relative .cpp and every header file found in this dir.
# Usage:
# make				production code
# make dbg=1	debug code

CXX = mpic++ 
CXXFLAGS = 

LD = mpic++ 
LFLAGS = 

# DO NOT CHANGE LINES BELOW
###########################################################################

OSUPPER = $(strip $(shell uname -s 2>/dev/null | tr [:lower:] [:upper:]) )

ifeq ($(OSUPPER),LINUX)
LFLAGS += -lrt -lpthread
endif

ifeq ($(dbg), 1)
	CXXFLAGS += -g -O0 -DDEBUG
	SUFFIX=_dbg
else
	CXXFLAGS += -O3 -msse2 -ftree-vectorize
	SUFFIX= 
endif

ifeq ($(verb), 1)
	VERB= 
else
	VERB=@
endif

SRCDIR = src
TESTSRCDIR = testsrc
OBJDIR = obj
BINDIR = bin

CXXFLAGS += -I$(SRCDIR)

SRCS = $(notdir $(wildcard $(SRCDIR)/*.cpp))
HDRS = $(wildcard $(SRCDIR)/*.hpp)
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o$(SUFFIX))

.PHONY: all clean cleanall makedir

all: makedir \
  $(BINDIR)/mpicart$(SUFFIX) \
  $(BINDIR)/2d_halo_scatter$(SUFFIX) 

# COMPILATION: all files other than main files
$(OBJDIR)/%.o$(SUFFIX): $(SRCDIR)/%.cpp $(HDRS)
	@echo compiling $@
	$(VERB)$(CXX) -c -o $@ $(CXXFLAGS) $<

# COMPILATION: main files

$(OBJDIR)/mpicart.o$(SUFFIX): $(TESTSRCDIR)/mpicart.cpp $(HDRS)
	@echo compiling $@
	$(VERB)$(CXX) -c -o $@ $(CXXFLAGS) $<

$(OBJDIR)/2d_halo_scatter.o$(SUFFIX): $(TESTSRCDIR)/2d_halo_scatter.cpp $(HDRS)
	@echo compiling $@
	$(VERB)$(CXX) -c -o $@ $(CXXFLAGS) $<

# LINKING

$(BINDIR)/mpicart$(SUFFIX): $(OBJDIR)/mpicart.o$(SUFFIX) $(OBJS)
	@echo linking $@
	$(VERB)$(LD) -o $@ $^ $(LFLAGS)

$(BINDIR)/2d_halo_scatter$(SUFFIX): $(OBJDIR)/2d_halo_scatter.o$(SUFFIX) $(OBJS)
	@echo linking $@
	$(VERB)$(LD) -o $@ $^ $(LFLAGS)


doc:
	doxygen mpicart.doxy

makedir:
	$(VERB)mkdir -p $(OBJDIR)
	$(VERB)mkdir -p $(BINDIR)

clean:
	$(VERB)rm -rf $(OBJDIR) $(BINDIR)

cleanall:
	$(VERB)make -C ./ clean
	$(VERB)make -C ./ dbg=1 clean
	$(VERB)rm -rf html
	$(VERB)rm -rf doxygen_sqlite3.db

2d_scatter_dbg: 2d_scatter.cpp $(SRCDIR)/logger.* $(SRCDIR)/CartSplitter.*
	mpic++ -std=c++0x -o 2d_scatter_dbg -g 2d_scatter.cpp \
	  $(SRCDIR)/logger.cpp $(SRCDIR)/CartSplitter.cpp -I $(SRCDIR) 

2d_scatter: 2d_scatter.cpp $(SRCDIR)/logger.* $(SRCDIR)/CartSplitter.* 
	mpic++ -std=c++0x -o 2d_scatter 2d_scatter.cpp \
	  $(SRCDIR)/logger.cpp $(SRCDIR)/CartSplitter.cpp -I $(SRCDIR) 
