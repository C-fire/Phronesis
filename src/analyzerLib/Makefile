-include ../../shared.mk

LINK_OPTIONS += -shared -Wl,-soname,libphronesisanalyzer.so.${VERSION} ${BOOST_LIBS}
#LINK_OPTIONS += -shared -Wl,-soname,libphronesisanalyzer.so.${VERSION}  -lboost_system -lboost_serialization -lboost_thread -lboost_regex -lboost_program_options -lboost_filesystem -Wl,-Bdynamic -lpthread
COMPIL_OPTIONS += -fPIC
INCLUDES += -I${ANALYZER_ACTION_SRCDIR} -I${BOOST_INC}
#INCLUDES += -I${ANALYZER_ACTION_SRCDIR}

VERSION = 1.0
SOURCES_1 = MetaAgent.cpp Coordinator.cpp Agent.cpp UserQuestion.cpp UserMsg.cpp FileAgent.cpp Server.cpp EnvironmentAgent.cpp ProcessAgent.cpp RecoveryUnit.cpp AgentRecovery.cpp FileRecovery.cpp ProcessRecovery.cpp EnvironmentRecovery.cpp Message.cpp FolderAgent.cpp VetoManager.cpp
OBJECTS_1 = $(SOURCES_1:.cpp=.o)
OBJS_1 = $(patsubst %,$(ANALYZER_LIB_OBJDIR)/%,$(OBJECTS_1))

SOURCES_3 = testAgent.cpp
OBJECTS_3 = $(SOURCES_3:.cpp=.o)
OBJS_3 = $(patsubst %,$(ANALYZER_LIB_OBJDIR)/%,$(OBJECTS_3))

all : depend phronesisanalyzer 
	if [ ! -e ${BINDIR}/libphronesisanalyzer.so ]; then ln -s ${BINDIR}/libphronesisanalyzer.so.${VERSION} ${BINDIR}/libphronesisanalyzer.so; fi;
    



phronesisanalyzer : $(OBJS_1)
	$(CC) ${INCLUDES} -o ${BINDIR}/lib$@.so.${VERSION} $(OBJS_1)  ${LINK_OPTIONS} 


#Rules to make all the objects (.c -> .o)

$(ANALYZER_LIB_OBJDIR)/%.o: %.cpp 
	$(CC) -c ${INCLUDES} -o $@ $< $(COMPIL_OPTIONS) 

# Generates dependencies.

depend :
	$(CC) $(INCLUDES) -MM *.cpp > makefile.dep

# Clean everything

clean :
	rm -rf ${BINDIR}/libphronesisanalyzer.so.${VERSION} ${ANALYZER_LIB_OBJDIR}/* 

-include makefile.dep

