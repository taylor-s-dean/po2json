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
	$(CXX) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
