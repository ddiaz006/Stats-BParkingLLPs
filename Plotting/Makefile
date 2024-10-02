CXX = $(shell root-config --cxx)
LD = $(shell root-config --ld)

INC = $(shell pwd)
CMSSW_INC = $(CMSSW_BASE)
REPO = ../Plotting

CPPFLAGS := $(shell root-config --cflags) -I$(REPO)/include  -I$(CMSSW_INC)/src
LDFLAGS := $(shell root-config --glibs) $(STDLIBDIR)  -lRooFit -lRooFitCore -L$(CMSSW_INC)/lib/slc6_amd64_gcc530/

#CPPFLAGS += -g -std=c++1z -fsanitize=address -Wall -Wextra -Wno-sign-compare
CPPFLAGS += -g -std=c++1z  -Wall -Wextra -Wno-sign-compare

TARGET = PlotLimits
TARGET_MULTI = PlotLimitsMulti

SRC = $(REPO)/app/plot_limits.cc $(REPO)/src/CommandLineInput.cc
SRC_MULTI = $(REPO)/app/plot_limits_multi.cc $(REPO)/src/CommandLineInput.cc

OBJ = $(SRC:.cc=.o)
OBJ_MULTI = $(SRC_MULTI:.cc=.o)

all : $(TARGET) $(TARGET_MULTI)

$(TARGET) : $(OBJ)
	$(LD) $(CPPFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)
	@echo $@
	@echo $<
	@echo $^

$(TARGET_MULTI) : $(OBJ_MULTI)
	$(LD) $(CPPFLAGS) -o $(TARGET_MULTI) $(OBJ_MULTI) $(LDFLAGS)
	@echo $@
	@echo $<
	@echo $^

%.o : %.cc
	$(CXX) $(CPPFLAGS) -o $@ -c $<
	@echo $@
	@echo $<

clean :
	rm -f *.o *.pdf *.png app/*.o src/*.o include/*.o $(TARGET)*~ $(TARGET_MULTI)*~
