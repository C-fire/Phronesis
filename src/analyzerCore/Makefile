-include ../../shared.mk



LINK_OPTIONS += ${BOOST_LIBS} -Wl,-Bdynamic  -lphronesisanalyzer -lphronesisanalyzerDBAPI -rdynamic -ldl -lpthread -L/usr/local/lib 

 
INCLUDES += -I${ANALYZER_LIB_SRCDIR} -I${ANALYZER_DB_SRCDIR} -I${BOOST_INC} -I/usr/local/include/  -I${ANALYZER_ACTION_SRCDIR}


SOURCES_1 = AnalyzerEngine.cpp main.cpp MetaAgentPool.cpp ProbabilityManagerDb.cpp Aggregator.cpp Scheduler.cpp RuleManager.cpp SyncManager.cpp InteractionManager.cpp ServerPool.cpp RecoveryEngine.cpp
OBJECTS_1 = $(SOURCES_1:.cpp=.o)
OBJS_1 = $(patsubst %,$(ANALYZER_CORE_OBJDIR)/%,$(OBJECTS_1))


all : depend phronesisAnalyzerCore

phronesisAnalyzerCore : $(OBJS_1)
	$(CC) ${INCLUDES} -o ${BINDIR}/$@ $(OBJS_1)  ${LINK_OPTIONS}

#Rules to make all the objects (.c -> .o)

$(ANALYZER_CORE_OBJDIR)/%.o: %.cpp 
	$(CC) -c $(INCLUDES) -o $@ $< $(COMPIL_OPTIONS) 

# Generates dependencies.

depend :
	$(CC) -MM *.cpp $(INCLUDES) > makefile.dep

# Clean everything

clean :
	rm -rf ${BINDIR}/phronesisAnalyzerCore ${ANALYZER_CORE_OBJDIR}/*

-include makefile.dep

