#################MACROS##########

# $(1) : Compiler
# $(2) : Object file to generate
# $(3) : Source file
# $(4) : Additional dependencies
# $(5) : Compiler flags
define COMPILE
$(2) : $(3) $(4)
	$(1) -c -o $(2) $(3) $(5)
endef

# $1 : Source file
define C2O
$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst $(SRC)%,$(OBJ)%,$(1))))
endef

# $1 : Source file
define C2H
$(patsubst %.c,%.h,$(patsubst %.cpp,%.hpp,$(1)))
endef

#############CONFIG##############
CC 			:= g++
C			:= gcc
CCFLAGS 	:= -Wall -pedantic -std=c++14
CFLAGS		:= $(CCFLAGS)
APP			:= libpicopng.a
SRC 		:= src
OBJ			:= obj
AR			:= ar
RANLIB		:= ranlib
ARFLAGS		:= -crs

ifdef DEBUG
	CCFLAGS += -g
else
	CCFLAGS += -O3
endif

ALLCPP		:= $(shell find src/ -type f -iname *.cpp)
ALLC		:= $(shell find src/ -type f -iname *.c)
ALLCOBJ		:= $(patsubst %.c, %.o, $(ALLC))
ALLOBJS	:= $(foreach F,$(ALLCPP) $(ALLC),$(call C2O,$(F)))
SRCFOLDERS	:= $(shell find $(SRC)/ -type d)
OBJFOLDERS	:= $(patsubst $(SRC)%, $(OBJ)%, $(SRCFOLDERS))
MKDIR 		:= mkdir -p

.PHONY: dir

#Generate library
$(APP) : $(OBJFOLDERS) $(ALLOBJS)
	$(AR) $(ARFLAGS) $(APP) $(ALLOBJS)
	$(RANLIB) $(APP)


$(foreach F,$(ALLCPP),$(eval $(call COMPILE,$(CC),$(call C2O,$(F)),$(F),$(call C2H,$(F)),$(CCFLAGS))))

$(foreach F,$(ALLC),$(eval $(call COMPILE,$(C),$(call C2O,$(F)),$(F),$(call C2H,$(F)),$(CFLAGS))))

# #Compila todos los cs
# %.o : %.c
# 	$(C) -o $(patsubst $(SRC)%, $(OBJ)%, $@) -c $^ $(CFLAGS)

# #Compila todos los cpps
# %.o : %.cpp
# 	$(CC) -o $(patsubst $(SRC)%, $(OBJ)%, $@) -c $^ $(CCFLAGS)

dir :
	# $(info $(SRCFOLDERS))
	# $(info $(OBJFOLDERS))
	$(info $(foreach F,$(ALLCPP),$(call COMPILE,$(CC),$(call C2O,$(F)),$(F),$(call C2H,$(F)),$(flags))))
	# $(info $(ALLC))
	# $(info $(ALLCOBJ))
	# $(info $(ALLCOBJ))

$(OBJFOLDERS) :
	$(MKDIR) $(OBJFOLDERS)

clean :
	rm -r ./$(OBJ)

cleanall : clean
	rm ./$(APP)