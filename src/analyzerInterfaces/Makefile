-include ../../shared.mk



LINK_OPTIONS += ${BOOST_LIBS} -lsqlite3
 
INCLUDES += -I${ANALYZER_LIB_SRCDIR} -I${ANALYZER_CLIENT_SRCDIR} -I/usr/local/include/ -I${BOOST_INC}


SOURCES_5 = icingaOutputParse.cpp
OBJECTS_5 = $(SOURCES_5:.cpp=.o)
OBJS_5 = $(patsubst %,$(ANALYZER_CLI_OBJDIR)/%,$(OBJECTS_5))

SOURCES_7 = automTestGeneric.cpp
OBJECTS_7 = $(SOURCES_7:.cpp=.o)
OBJS_7 = $(patsubst %,$(ANALYZER_CLI_OBJDIR)/%,$(OBJECTS_7))

SOURCES_8 = phronesisXmlClient.cpp
OBJECTS_8 = $(SOURCES_8:.cpp=.o)
OBJS_8 = $(patsubst %,$(ANALYZER_CLI_OBJDIR)/%,$(OBJECTS_8))

SOURCES_9 = phrStat.cpp
OBJECTS_9 = $(SOURCES_9:.cpp=.o)
OBJS_9 = $(patsubst %,$(ANALYZER_CLI_OBJDIR)/%,$(OBJECTS_9))

all : depend icingaOutputParse automTestGeneric phronesisXmlClient phrStat


icingaOutputParse : $(OBJS_5)
	$(CC) ${INCLUDES} -o ${BINDIR}/$@ $(OBJS_5)  ${LINK_OPTIONS} 
	
automTestGeneric : $(OBJS_7)
	$(CC) ${INCLUDES} -o ${BINDIR}/$@ $(OBJS_7)  ${LINK_OPTIONS} 
	
phronesisXmlClient : $(OBJS_8)
	$(CC) ${INCLUDES} -o ${BINDIR}/$@ $(OBJS_8)  ${LINK_OPTIONS} 
	
phrStat : $(OBJS_9)
	$(CC) ${INCLUDES} -o ${BINDIR}/$@ $(OBJS_9)  ${LINK_OPTIONS} 
	
#Rules to make all the objects (.c -> .o)

$(ANALYZER_CLI_OBJDIR)/%.o: %.cpp 
	$(CC) -c $(INCLUDES) -o $@ $< $(COMPIL_OPTIONS) 

# Generates dependencies.

depend :
	$(CC) -MM *.cpp $(INCLUDES) > makefile.dep

# Clean everything

clean :
	rm -rf ${BINDIR}/icingaOutputParse ${BINDIR}/automTestGeneric ${BINDIR}/phronesisXmlClient ${BINDIR}/phrStat ${ANALYZER_CLI_OBJDIR}/*

-include makefile.dep

