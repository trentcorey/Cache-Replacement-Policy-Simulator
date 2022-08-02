# Lines starting with the pound sign are comments.
#
# These are the options that may need tweaking

# Debug
#DEBUG = -DDEBUG
DEBUG =

# Main executable target
EXECUTABLE = splaytest

# Compiler/Linker Name
LINKCC = $(CC)

# Other object files that this executable needs to link with
# but are not in this directories
# e.g. subdir1/abc.o subdir2/cde.o
OTHEROBJS = 

# List any obj files that need to be built
# before the EXPORTOBJS or OTHEREXEC can be built
IMPORTOBJS = 

# Other executable files in this directory that needs to be
# made but are not linked into the executable.
# These are usually localized test programs.
OTHEREXEC =

# List any obj files that needs to be built
# for use by other modules but not by the executables
# (Include here files in other directories that these files depend on.)
EXPORTOBJS =

EXPORTINCLUDES =

# Other Makefiles that contain additional dependencies
OTHERDEPS = 

# Dir where EXPORTOBJS and libraries are stored
BINDIR =
INCLUDEDIR =

# sub dir to process
DIRS = 

# You can modify the below as well, but probably
# won't need to.
#

# CC is for the name of the C compiler. CPPFLAGS denotes pre-processor
# flags, such as -I options. CFLAGS denotes flags for the C compiler.
# CXXFLAGS denotes flags for the C++ compiler. You may add additional
# settings here, such as PFLAGS, if you are using other languages such
# as Pascal.

export CPPFLAGS = 
#export LDFLAGS = -lgsl -lgslblas -lm
#use static library
export LDFLAGS = -static -lgsl -lgslcblas -lm
export CC = gcc
#export CFLAGS = ${DEBUG} -Wall -Wstrict-prototypes -g
export CFLAGS = ${DEBUG} -Wall -Wstrict-prototypes

SRCS := $(wildcard *.c) 
#OBJS := $(patsubst %.c,%.o,$(wildcard *.c)) 
OBJS := splay.o splaytest.o
DEPS := $(patsubst %.o,%.d,$(OBJS)) $(OTHERDEPS)

# "all" is the default target. 

all: subdirs export $(IMPORTOBJS) $(EXPORTOBJS) $(OTHEROBJS) $(EXECUTABLE)\
 $(OTHEREXEC) install

subdirs:
	@for dir in $(DIRS); do $(MAKE) -C $$dir; done

export:
	@for f in $(EXPORTINCLUDES); do cp -p $$f $(INCLUDEDIR); done
	@for dir in $(DIRS); do $(MAKE) -C $$dir export; done

# Define the components of the program, and how to link them together.
# These components are defined as dependencies; that is, they must be
# made up-to-date before the code is linked.

$(EXECUTABLE): $(DEPS) $(OBJS) $(OTHEROBJS)
	$(LINKCC) -o $(EXECUTABLE) $(OBJS) $(OTHEROBJS) $(LDFLAGS)
#	$(LINKCC) $(LDFLAGS) -o $(EXECUTABLE) $(OBJS) $(OTHEROBJS)

# Specify that the dependency files depend on the C source files.
# Dependency (.d) files are Makefile fragments that specify the
# *.d and *.c files depend on the .h files included in .c
%.d: %.c
	@echo "Generating dependencies of $<"; \
	$(SHELL) -ec '$(CC) -MM $(CPPFLAGS) $< \
                      | sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
                      [ -s $@ ] || rm -f $@'

install:
	@for f in $(EXPORTOBJS); do cp -p $$f $(BINDIR); done

clean:
	-rm -f $(OBJS) $(EXECUTABLE) $(DEPS) $(EXPORTOBJS) $(OTHEREXEC) *~
	@for dir in $(DIRS); do $(MAKE) -C $$dir clean; done

realclean:
	-rm -f $(OBJS) $(EXECUTABLE) $(DEPS) $(EXPORTOBJS) $(OTHEREXEC) \
$(IMPORTOBJS) $(OTHEROBJS) *~
	@for dir in $(DIRS); do $(MAKE) -C $$dir realclean; done


explain:
	@echo "--------------------------------------------------"
	@echo "The following information represents your program:"
	@echo "Final executable name:    $(EXECUTABLE)"
	@echo "Final exec extra obj dep: $(OTHEROBJS)"
	@echo "Other executable          $(OTHEREXEC)"
	@echo "Objs or libs imported     $(IMPORTOBJS)"
	@echo "Objs or libs produced     $(EXPORTOBJS)"
	@echo "Objs or libs stored in    $(BINDIR)"
	@echo "Source files:             $(SRCS)"
	@echo "Object files:             $(OBJS)"
	@echo "Dependency files:         $(DEPS)"
	@echo "Subdirectories:           $(DIRS)"
	@echo "--------------------------------------------------"

dep: $(DEPS)
	@for dir in $(DIRS); do $(MAKE) -C $$dir ; done
	@echo "Dependencies are now up-to-date."

-include $(DEPS)
