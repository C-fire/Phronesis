include shared.mk

all: analyzer 

analyzer: phronesisanalyzer phronesisanalyzerAgent phronesisanalyzerDb  phronesisanalyzerCore  phronesisanalyzerClientApi phronesisCLI
analyzerClient : phronesisanalyzer phronesisanalyzerAgent 

phronesisanalyzerAgent:
	if [ ! -e ${ANALYZER_AGENT_OBJDIR} ]; then mkdir -p ${ANALYZER_AGENT_OBJDIR} ; fi;
	echo " " ; echo "\n\n~~~~ Phronesis Analyzer Agent ~~~~\n\n"; $(MAKE) -C ${ANALYZER_AGENT_SRCDIR} all

phronesisanalyzer:
	if [ ! -e ${ANALYZER_LIB_OBJDIR} ]; then mkdir -p ${ANALYZER_LIB_OBJDIR} ; fi;
	echo " " ; echo "~~~~ Phronesis Analyzer library ~~~~\n"; $(MAKE) -C ${ANALYZER_LIB_SRCDIR} all

phronesisanalyzerDb:
	if [ ! -e ${ANALYZER_DB_OBJDIR} ]; then mkdir -p ${ANALYZER_DB_OBJDIR} ; fi;
	echo " " ; echo "\n\n~~~~ Phronesis Analyzer Database API ~~~~\n\n"; $(MAKE) -C ${ANALYZER_DB_SRCDIR} all

phronesisanalyzerCore:
	if [ ! -e ${ANALYZER_CORE_OBJDIR} ]; then mkdir -p ${ANALYZER_CORE_OBJDIR} ; fi;
	echo " " ; echo "\n\n~~~~ Phronesis Analyzer Core ~~~~\n\n"; $(MAKE) -C ${ANALYZER_CORE_SRCDIR} all
	

phronesisanalyzerClientApi:
	if [ ! -e ${ANALYZER_CLIENT_OBJDIR} ]; then mkdir -p ${ANALYZER_CLIENT_OBJDIR} ; fi;
	echo " " ; echo "\n\n~~~~ Phronesis Analyzer Client Api ~~~~\n\n"; $(MAKE) -C ${ANALYZER_CLIENT_SRCDIR} all

phronesisCLI:
	if [ ! -e ${ANALYZER_CLI_OBJDIR} ]; then mkdir -p ${ANALYZER_CLI_OBJDIR} ; fi;
	echo " " ; echo "\n\n~~~~ Phronesis CLI ~~~~\n\n"; $(MAKE) -C ${ANALYZER_CLI_SRCDIR} all


doc:
	$(MAKE) -C ${DOCDIR} doc

clean :
	rm -rf ${BINDIR}/*
	find ${OBJDIR} -type f | xargs rm
listVar:
	echo ${MODE}
	echo ${OBJDIR}
	echo ${BINDIR}
	echo ${SRCDIR}
	echo ${DETECTOR_SRCDIR}
	echo ${DETECTOR_LIB_SRCDIR}
	echo ${DETECTOR_DB_SRCDIR}
	echo ${DETECTOR_CORE_SRCDIR}
	echo ${DETECTOR_COMPILER_SRCDIR}
	echo ${DETECTOR_OBJDIR}
	echo ${DETECTOR_LIB_OBJDIR}
	echo ${DETECTOR_DB_OBJDIR}
	echo ${DETECTOR_CORE_OBJDIR}
	echo ${DETECTOR_COMPILER_OBJDIR}
	echo "-"${HOSTNAME}"-"
	echo ${AI_LIB_BINDIR}
	echo ${CC}
	echo ${LINK_OPTIONS }
	echo ${COMPIL_OPTIONS }
	echo ${INCLUDES}
	echo ${BOOST_LIBS}
