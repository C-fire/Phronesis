-include ../../shared.mk

LINK_OPTIONS +=  -L/usr/local/lib ${BOOST_LIBS} -lphronesisanalyzer 
INCLUDES += -I${BOOST_INC} -I/usr/local/include/ -I${PYTHON_INC} -I${ANALYZER_LIB_SRCDIR}

#SOURCES_1 = PhronesisConsole.cpp
#OBJECTS_1 = $(SOURCES_1:.cpp=.o)
#OBJS_1 = $(patsubst %,$(ANALYZER_CLIENT_OBJDIR_2)/%,$(OBJECTS_1))

SOURCES_2 = phrUtils.cpp
OBJECTS_2 = $(SOURCES_2:.cpp=.o)
OBJS_2 = $(patsubst %,$(ANALYZER_CLIENT_OBJDIR)/%,$(OBJECTS_2))

all : depend phrUtils
	
    



#phronesisConsole : $(OBJS_1)
#	$(CC) ${INCLUDES}  ${LINK_OPTIONS} -o ${BINDIR}/$@ $(OBJS_1)
	
phrUtils : $(OBJS_2)
	$(CC) ${INCLUDES}  -o ${BINDIR}/$@ $(OBJS_2)  ${LINK_OPTIONS}

#Rules to make all the objects (.c -> .o)

$(ANALYZER_CLIENT_OBJDIR)/%.o: %.cpp 
	$(CC) -c $(INCLUDES) -o $@ $< $(COMPIL_OPTIONS) 

# Generates dependencies.

depend :
	$(CC) -MM *.cpp $(INCLUDES) > makefile.dep

# Clean everything

clean :
	rm -rf ${BINDIR}/phrUtils ${ANALYZER_CLIENT_OBJDIR}/*

-include makefile.dep

