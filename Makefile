CXX = clang++
CFLAGS = -I./third_party/CLI11/include \
		 -I./third_party/rapidjson/include

ODIR = obj
LDIR = ./lib
SDIR = ./src

LIBS =

_DEPS = po2json.hpp
DEPS = $(patsubst %,$(SDIR)/%,$(_DEPS))

_OBJ = po2json.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

.PHONY: all clean
.SECONDARY: main-build

all: pre-build main-build

main-build: po2json

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CFLAGS)

po2json: $(OBJ)
	$(CXX) -o $@ $^ $(CFLAGS) $(LIBS)

pre-build:
	@if git submodule status | egrep -q '^[-]|^[+]' ; then 		\
		echo "INFO: Need to reinitialize git submodules"; 		\
		git submodule update --init; 							\
	else 														\
		echo "INFO: No need to reinitialize git submodules"; 	\
	fi
	@if test ! -d $(ODIR) ; then \
		mkdir $(ODIR);           \
	fi

clean:
	rm -rf $(ODIR) *~ core $(INCDIR)/*~
