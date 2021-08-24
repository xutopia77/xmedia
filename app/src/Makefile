
CROSS_COMPILE	:=
AR 				:= $(CROSS_COMPILE)ar
CC 				:= $(CROSS_COMPILE)gcc
CXX 			:= $(CROSS_COMPILE)g++

#--
PRJ_PATH 	= .
OBJS_PATH 	= $(PRJ_PATH)/objs/tmp/tmp
DEPS_PATH 	= $(OBJS_PATH)
LIB_PATH 	= $(PRJ_PATH)/lib

NOVERSION		= 1

#--------------------------------Output------------------------------
# OUTTYPE: 0-exe, 1-dll, 2-static
#--------------------------------------------------------------------
OUTTYPE = 0
EXE = main

#--
I_FLAGS	= 	-I$(PRJ_PATH)

I_FLAGS	+= 	-I$(PRJ_PATH)/CorCfg
I_FLAGS	+= 	-I$(LIB_PATH)/include
I_FLAGS	+=	-I$(LIB_PATH)/include/libutil
I_FLAGS	+= 	-I$(LIB_PATH)/include/lib3rdinlcude
# I_FLAGS	+=	-I$(LIB_PATH)/include/libaio
I_FLAGS	+=	-I$(LIB_PATH)/include/libhttp
# I_FLAGS	+=	-I$(LIB_PATH)/include/libice
# I_FLAGS	+=	-I$(LIB_PATH)/include/libice/test
I_FLAGS	+=	-I$(LIB_PATH)/include/libavbsf
I_FLAGS	+=	-I$(LIB_PATH)/include/libavcodec
# I_FLAGS	+=	-I$(LIB_PATH)/include/libdash
I_FLAGS	+=	-I$(LIB_PATH)/include/libflv
# I_FLAGS	+=	-I$(LIB_PATH)/include/libhls 
I_FLAGS	+=	-I$(LIB_PATH)/include/libmov 
I_FLAGS	+=	-I$(LIB_PATH)/include/libmpeg
I_FLAGS	+=	-I$(LIB_PATH)/include/libmkv 
I_FLAGS	+=	-I$(LIB_PATH)/include/librtmp
# I_FLAGS	+=	-I$(LIB_PATH)/include/librtmp/aio
I_FLAGS	+=	-I$(LIB_PATH)/include/librtp
I_FLAGS	+=	-I$(LIB_PATH)/include/librtsp
I_FLAGS	+=	-I$(LIB_PATH)/include/librtsp/test
# I_FLAGS	+=	-I$(LIB_PATH)/include/libsip

I_FLAGS	+=	-I$(LIB_PATH)/include/CorThread

I_FLAGS	+=	-I$(PRJ_PATH)/HttpServer
I_FLAGS	+=	-I$(PRJ_PATH)/MediaServer

#--
C_CXX_FLAGS	=	-g -Wall
C_CXX_FLAGS	+=	-DOS_INT64_TYPE
C_CXX_FLAGS	+= 	-DDEBUG -D_DEBUG
C_CXX_FLAGS	+= 	-DOS_LINUX
C_CXX_FLAGS	+= 	-Wno-comment
C_CXX_FLAGS +=	-DCOR_VER
# C_CXX_FLAGS +=	-DHLS
# default don't export anything
# C_FLAGS += -fvisibility=hidden
C_FLAGS += $(C_CXX_FLAGS)
#C_FLAGS += -fsanitize=address
CXX_FLAGS += $(C_CXX_FLAGS) -std=c++11

#--
LD_FLAGS 	+= -lrt -lpthread -ldl -lssl -lcrypt -lcorthread -lst

LD_FLAGS	+= 	-L${LIB_PATH}/							
LD_FLAGS	+= 	-lxlog -lcjson

LD_FLAGS	+=	-lutil
# LD_FLAGS	+= 	-ldash
LD_FLAGS	+=	-lflv
# LD_FLAGS	+=	-lhls
LD_FLAGS	+=	-lmov
LD_FLAGS	+=	-lmpeg
LD_FLAGS	+=	-lrtmp
LD_FLAGS	+=	-lrtsp  -lmov -lflv
LD_FLAGS	+=	-lrtp
# LD_FLAGS	+=	-lsip
LD_FLAGS	+=	-lhttp -lmpeg
# LD_FLAGS	+=	-lice
LD_FLAGS	+=	-lavbsf
LD_FLAGS	+=	-lavcodec
LD_FLAGS	+=	-lh264
LD_FLAGS	+=	-lh265 -lutil

#--
SRCS_PATH 	= $(PRJ_PATH) 
SRCS_PATH	+=	$(PRJ_PATH)/CorCfg
SRCS_PATH	+=	$(PRJ_PATH)/HttpServer
SRCS_PATH	+=	$(PRJ_PATH)/MediaServer
SRCS_PATH	+=	$(PRJ_PATH)/MediaServer/RtspSrv
SRCS_PATH	+=	$(PRJ_PATH)/unitTest

#--	
C_SOURCES 		= $(foreach dir,$(SRCS_PATH), $(wildcard $(dir)/*.c))
C_SOURCES 		:= $(filter-out $(_C_SOURCES),$(C_SOURCES))

CXX_SOURCES 	= $(foreach dir,$(SRCS_PATH), $(wildcard $(dir)/*.cpp))

SOURCE_FILES = $(CXX_SOURCES)
SOURCE_FILES += $(C_SOURCES)

#-----------------------------------------------------------------------
# ifeq ($(STATIC_LINK),1)
#     LDFLAGS += -static
# endif

OBJS_FILES 	:= 	$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE_FILES)))
OBJS_FILES 	:= 	$(addprefix $(OBJS_PATH)/,$(OBJS_FILES))
DEP_FILES 	:= 	$(OBJS_FILES:%.o=%.d)
MKDIR 		=	@mkdir -p $(dir $@)

cur_fullpath_filename:=$(shell pwd)/$(lastword $(MAKEFILE_LIST))
cur_fullpath=$(shell dirname $(cur_fullpath_filename))

#--------------------------Makefile Rules----------------------------
$(EXE): $(OBJS_FILES)
ifeq ($(OUTTYPE),0)
	@echo -e "\033[35m	LINK	$(notdir $@)\033[0m"
	@$(CXX) -o $@ -Wl,-rpath $(cur_fullpath)/${LIB_PATH} $(LDFLAGS) $^ $(LD_FLAGS)
else
ifeq ($(OUTTYPE),1)
	$(CXX) -o $@ -shared -fPIC -rdynamic -Wl,-rpath . $(LDFLAGS) $^ $(LD_FLAGS)
else
	@echo -e "\033[35m	AR	$(notdir $@)\033[0m"
	@$(AR) -rc $@ $^
endif
endif
	@echo make ok, output: $(EXE)

$(OBJS_PATH)/%.o : %.c
	$(MKDIR)
	@$(CC) $(I_FLAGS) $(C_FLAGS) -c -o $@ $<
	@echo -e "\033[35m	CC	$(notdir $@)\033[0m"

$(OBJS_PATH)/%.o : %.cpp
	$(MKDIR)
	@$(CXX) $(I_FLAGS) $(CXX_FLAGS) -c -o $@ $<
	@echo -e "\033[35m	CXX	$(notdir $@)\033[0m"

$(OBJS_PATH)/%.d: %.c
	$(MKDIR)
	@echo -e "\033[32m	CREATE	$(notdir $@)\033[0m"
	@rm -f $@; \
	 $(CC) $(I_FLAGS) $(C_FLAGS) -MM $(C_FLAGS) $< > $@.$$$$; \
     sed 's,\($(notdir $*)\)\.o[ :]*,$*\.o $@ : ,g' < $@.$$$$ > $@; \
     rm -f $@.$$$$
$(OBJS_PATH)/%.d: %.cpp
	$(MKDIR)
	@echo -e "\033[32m	CREATE	$(notdir $@)\033[0m"
	@rm -f $@; \
	 $(CXX) $(I_FLAGS) $(CXX_FLAGS) -MM $(CXX_FLAGS) $< > $@.$$$$; \
     sed 's,\($(notdir $*)\)\.o[ :]*,$*\.o $@ : ,g' < $@.$$$$ > $@; \
     rm -f $@.$$$$

ifeq ($(MAKECMDGOALS), clean)
else ifeq ($(MAKECMDGOALS), debug)
else
sinclude $(DEP_FILES)
endif

version.h : version.ver
	$(PRJ_PATH)/svnver.sh version.ver version.h

.PHONY: clean
clean:
	@echo -e "\033[35m	 rm -rf *.o  *.d $(EXE) \033[0m"
	@rm -f $(OBJS_FILES) $(EXE) $(DEP_FILES)

debug:
	echo $(EXE)
	echo $(OBJS_FILES)
	echo $(DEP_FILES)
	echo cur_fullpath: $(cur_fullpath)