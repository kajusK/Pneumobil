###########################################
# Jakub Kaderka 2018
###########################################
#resulting bin
TARGET		= run_tests

#dirs
SOURCES = ../../sources
EXTERNAL = $(SOURCES)/external

OBJDIR		= obj
UNITY_DIR	= $(EXTERNAL)/Unity
APP_DIR		= $(SOURCES)/app/pneumatic
SOURCES_DIRS	= $(UNITY_DIR)/src $(UNITY_DIR)/extras/fixture/src \
	$(SOURCES)/tests/unit

#additional build flags
OPT		:= -O0
CSTD		= -std=c11
INCLUDES	= \
	-I $(SOURCES) \
	-I $(SOURCES)/tests/unit \
	-I $(SOURCES)/tests/unit/helpers \
	-I $(SOURCES)/app/ecu \
	-I $(UNITY_DIR)/src \
	-I $(UNITY_DIR)/extras/fixture/src \
	-I $(UNITY_DIR)/src/app/ecu
BUILD_FLAGS	= -fprofile-arcs -ftest-coverage
DEFS		= -DUNITY_FLOAT_VERBOSE

#files
C_SRC		:= $(shell find $(SOURCES_DIRS) -name '*.c')
C_SRC		:= $(subst $(SOURCES)/, , $(C_SRC))
CPP_SRC		:= $(shell find $(SOURCES_DIRS) -name '*.cpp')
CPP_SRC		:= $(subst $(SOURCES)/, , $(CPP_SRC))
AS_SRC		:= $(shell find $(SOURCES_DIRS) -name '*.s')
AS_SRC		:= $(subst $(SOURCES)/, , $(AS_SRC))
OBJS		:= $(addprefix $(OBJDIR)/, $(C_SRC:.c=.o) $(CPP_SRC:.cpp=.o) $(AS_SRC:.s=.o))
DEPS		:= $(addprefix $(OBJDIR)/, $(C_SRC:.c=.d) $(CPP_SRC:.cpp=.d))
DIRS		:= $(shell find $(SOURCES_DIRS) -type d)
DIRS		:= $(addprefix $(OBJDIR)/, $(subst $(SOURCES)/, , $(DIRS)))

#toolchain
PREFIX		=
#toolchain definitions
CC		:= $(PREFIX)gcc
CPP		:= $(PREFIX)g++
LD		:= $(PREFIX)gcc
AR		:= $(PREFIX)ar
AS		:= $(PREFIX)as
OBJCOPY		:= $(PREFIX)objcopy
OBJDUMP		:= $(PREFIX)objdump
GDB		:= $(PREFIX)gdb
SIZE		:= $(PREFIX)size

###############################################################################
# C flags
CFLAGS	+= $(OPT) $(CSTD) $(ARCH_FLAGS)
CFLAGS	+= $(DEFS)
CFLAGS	+= $(BUILD_FLAGS)
CFLAGS	+= $(INCLUDES)
CFLAGS	+= -g
CFLAGS	+= -Wall -Wextra -pedantic -Wundef -Wshadow
CFLAGS	+= -Wmissing-prototypes -Wstrict-prototypes
CFLAGS	+= -fno-common -ffunction-sections -fdata-sections
CFLAGS	+= -Wno-unused-function

# C++ flags
CXXFLAGS	+= $(CFLAGS)
CXXFLAGS	+=  -Weffc++

# ASM flags
ASFLAGS		+= -felf

# Linker flags
LDFLAGS		+= $(BUILD_FLAGS) -lm -fprofile-arcs --coverage

all: $(TARGET) tests coverage

$(DIRS):
	mkdir -pv $(DIRS)

$(OBJDIR)/%.o : $(SOURCES)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
	@$(CC) $(CFLAGS) -MM -MT '$@ $(basename $@).d' $< > $(basename $@).d

$(OBJDIR)/%.o : $(SOURCES)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
	@$(CXX) $(CXXFLAGS) -MM -MT '$@ $(basename $@).d' $< > $(basename $@).d

$(OBJDIR)/%.o : $(SOURCES)/%.s
	$(AS) $(ASFLAGS) $< -o $@

$(TARGET): $(DIRS) $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@

tests: $(TARGET)
	./$< -v

coverage: tests
	mkdir -p coverage
	gcovr --html --html-details -o coverage/index.html -s -u -r "." -e ".*/tests/.*" -e ".*/external/.*"

clean:
	$(RM) -r $(OBJDIR) $(TARGET) coverage

.PHONY : clean all

-include $(DEPS)
