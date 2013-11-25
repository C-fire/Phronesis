-include ../../shared.mk


#LINK_OPTIONS += -shared -Wl,-soname,libphronesisanalyzerDBAPI.so.1 -lmysqlclient
#COMPIL_OPTIONS += -fPIC
#INCLUDES = -I${SRCDIR}/analyzer/analyzerLib -L${SRCDIR}/analyzer/analyzerLib
VERSION=1.0

LINK_OPTIONS += -shared -Wl,-soname,libphronesisanalyzerDBAPI.so.${VERSION} -lmysqlclient -lphronesisanalyzer
COMPIL_OPTIONS += -fPIC
INCLUDES += -I${ANALYZER_LIB_SRCDIR} -I${ANALYZER_ACTION_SRCDIR}




SOURCES_1 = AnalyzerDBAPI.cpp
OBJECTS_1 = $(SOURCES_1:.cpp=.o)
OBJS_1 = $(patsubst %,$(ANALYZER_DB_OBJDIR)/%,$(OBJECTS_1))


all : depend phronesisanalyzerDBAPI
	if [ ! -e ${BINDIR}/libphronesisanalyzerDBAPI.so ]; then ln -s ${BINDIR}/libphronesisanalyzerDBAPI.so.${VERSION} ${BINDIR}/libphronesisanalyzerDBAPI.so; fi;

phronesisanalyzerDBAPI : $(OBJS_1)
	$(CC) ${INCLUDES}  -o ${BINDIR}/lib$@.so.${VERSION} $(OBJS_1) ${LINK_OPTIONS}

#Rules to make all the objects (.c -> .o)

$(ANALYZER_DB_OBJDIR)/%.o: %.cpp 
	$(CC) -c ${INCLUDES} -o $@ $< $(COMPIL_OPTIONS) 

# Generates dependencies.

depend :
	$(CC) -MM *.cpp ${INCLUDES} > makefile.dep

# Clean everything

clean :
	rm -rf ${BINDIR}/libphronesisanalyzerDBAPI.so.${VERSION} ${ANALYZER_DB_OBJDIR}/*

-include makefile.dep

