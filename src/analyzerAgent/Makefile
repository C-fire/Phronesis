-include ../../shared.mk



#LINK_OPTIONS += -lboost_regex -lboost_thread -lboost_serialization -lboost_system -L/usr/local/lib -lphronesisanalyzer
LINK_OPTIONS += ${BOOST_LIBS} -Wl,-Bdynamic -lphronesisanalyzer

 
INCLUDES += -I/usr/local/include/  -I${BOOST_INC} -I${ANALYZER_LIB_SRCDIR}


SOURCES_1 = PhronesisAgent.cpp Treatment.cpp
OBJECTS_1 = $(SOURCES_1:.cpp=.o)
OBJS_1 = $(patsubst %,$(ANALYZER_AGENT_OBJDIR)/%,$(OBJECTS_1))

SOURCES_2 = queryCLI.cpp
OBJECTS_2 = $(SOURCES_2:.cpp=.o)
OBJS_2 = $(patsubst %,$(ANALYZER_AGENT_OBJDIR)/%,$(OBJECTS_2))

all : depend phronesisAnalyzerAgent queryAgent 

phronesisAnalyzerAgent : $(OBJS_1)
	$(CC) ${INCLUDES}  -o ${BINDIR}/$@ $(OBJS_1) ${LINK_OPTIONS}
	
queryAgent : $(OBJS_2)
	$(CC) ${INCLUDES} -o ${BINDIR}/$@ $(OBJS_2) ${LINK_OPTIONS}

#Rules to make all the objects (.c -> .o)

$(ANALYZER_AGENT_OBJDIR)/%.o: %.cpp 
	$(CC) -c $(INCLUDES) -o $@ $< $(COMPIL_OPTIONS) 

# Generates dependencies.

depend :
	$(CC) -MM *.cpp $(INCLUDES) > makefile.dep

# Clean everything

clean :
	rm -rf ${BINDIR}/phronesisAnalyzerAgent ${ANALYZER_AGENT_OBJDIR}/*

-include makefile.dep

