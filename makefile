IDIR = ./include
CXX = clang++
CFLAGS = -I$(IDIR) \
		 -I./third_party/CLI11/include \
		 -I./third_party/rapidjson/include

ODIR = obj
LDIR = ./lib
SDIR = ./src

LIBS =

_DEPS = po2json.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = po2json.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CFLAGS)

po2json: $(OBJ)
	make init
	$(CXX) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

.PHONY: init

init:
	@if git submodule status | egrep -q '^[-]|^[+]' ; then 		\
		echo "INFO: Need to reinitialize git submodules"; 		\
		git submodule update --init; 							\
	else 														\
		echo "INFO: No need to reinitialize git submodules"; 	\
	fi

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
