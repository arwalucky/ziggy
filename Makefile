##########################################################################################################################
# File automatically-generated by tool: [projectgenerator] version: [3.7.1] date: [Tue Jun 30 14:26:05 CEST 2020]
##########################################################################################################################

# ------------------------------------------------
# Generic Makefile (based on gcc)
#
# ChangeLog :
#	2017-02-10 - Several enhancements + project update mode
#   2015-07-22 - first version
# ------------------------------------------------

######################################
# target
######################################
TARGET = ziggy

######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT =  -O3 #TODO -Os (size) or -O3 (speed/Size)


#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
CPP_SOURCES =  \
main.cpp \
redis_client.cpp \
mqtt_client.cpp \
anchorList.cpp \
tag_list.cpp \
sqlite3.cpp \
scripts/auxiliary_functions.cpp 

# C sources
C_SOURCES =  




#######################################
# binaries
#######################################
PREFIX = 
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CXX = $(GCC_PATH)/$(PREFIX)g++
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CXX = $(PREFIX)g++
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################



# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS = 


# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  



# compile gcc flags

CFLAGS =   $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall 


# TODO: GET RID OF WARNINGS -> C++2a
# TODO: Compile ERROR FOR c++>14 WITH BLOCK MANAGER FIND FUNCTION
CXXFLAGS =   $(C_DEFS) $(C_INCLUDES) $(OPT) -g -Wall  -std=c++17

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script


# libraries
LIBS = -lpaho-mqtt3c  -lredis++   -lhiredis -pthread  -l sqlite3
LIBDIR = -L/usr/local/lib 
LDFLAGS =   $(LIBDIR) $(LIBS)

# default action: build all
all: $(BUILD_DIR)/$(TARGET)


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.o)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))

OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))



$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR) 
	$(CXX) -c $(CXXFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.cpp=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@


$(BUILD_DIR)/$(TARGET): $(OBJECTS) Makefile
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@





all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)
	
$(BUILD_DIR):
	mkdir $@		

$(EXE): $(OBJS)
	$(CXX) -o -g $@ $^ $(CXXFLAGS) $(LIBS) 


#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***