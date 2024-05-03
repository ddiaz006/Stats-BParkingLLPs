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

SRC = $(REPO)/app/plot_limits.cc $(REPO)/src/CommandLineInput.cc

OBJ = $(SRC:.cc=.o)

all : $(TARGET)

$(TARGET) : $(OBJ)
	$(LD) $(CPPFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)
	@echo $@
	@echo $<
	@echo $^

%.o : %.cc
	$(CXX) $(CPPFLAGS) -o $@ -c $<
	@echo $@
	@echo $<

clean :
	rm -f *.o *.pdf *.png app/*.o src/*.o include/*.o $(TARGET)*~
