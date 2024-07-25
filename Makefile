#######################
# Main configurations #
#######################

INCDIR := include
SRCDIR := src
LIBDIR := lib

CFLAGS   := -std=c2x -lm -lglfw3 -lgdi32
CPPFLAGS := -I$(INCDIR) -L$(LIBDIR) -finput-charset=UTF-8 -MMD
LDFLAGS  := 
	TARGET   := release
	CFLAGS   += -c
	#CFLAGS   += -O3
	LDFLAGS += -std=c2x -lm -lglfw3 -lgdi32
	LDFLAGS += -I$(INCDIR) -L$(LIBDIR)

OBJDIR := obj/$(TARGET)

CC  := gcc
EXE := target/$(TARGET)/Main.exe

# Disable built-in rules and variables
MAKEFLAGS += --no-builtin-rules
MAKEFLAGS += --no-builtin-variables

#######################

#############
# Functions #
#############

# Convert 1° arg suffix to 2° arg suffix
define to_suffix
	$(addsuffix $(2),$(basename $(1)))
endef

# Get 1° arg extension files from 2° arg base folder
define get_suffix
	$(call to_suffix,$(addprefix $(OBJDIR)/,$(2)),$(1))
endef

# Compile recipe with 1° arg compiler and 2° arg flags
define create_src_obj
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(1) $< -MF $(basename $@).d -o $@
endef

#############

# Get all source files recursively
SRC         := $(shell find $(SRCDIR)/ -type f)
SRCOBJS     := $(addprefix $(OBJDIR)/,$(patsubst %.c,%.o,$(SRC)))
MAKEFILES   := $(addsuffix .d,$(basename $(SRCOBJS)))

###########
# Targets #
###########

# Default target
all: $(EXE)

# Compile sources
$(call get_suffix,.o  ,$(SRC)) : $(OBJDIR)/%.o : %.c
	$(call create_src_obj,-c)

$(call get_suffix,.dll,$(SRC)) : $(OBJDIR)/%.dll : %.c
	$(info The following is private information: $^)
	$(call create_src_obj,-shared)

## Compile executable

$(EXE): $(SRCOBJS)
	
	@mkdir -p target/$(TARGET)
	$(CC) $(SRCOBJS) -o $@ $(LDFLAGS)

	

clean:
	-rm -rf $(EXE) $(OBJDIR)

###########

########################################
# Automatic makefile generation (-MMD) #
########################################

ifneq ($(MAKECMDGOALS),clean)
$(MAKEFILES): ;
-include $(MAKEFILES)
endif

########################################

