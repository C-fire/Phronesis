USE_SPECIFIC_BOOST=False
LD_MODE=dynamic
MODE = Debug
BASE=/home/chaen/phronesis-to-git-boost-1.53
OBJDIR = ${BASE}/obj
BINDIR = ${BASE}/bin
SRCDIR = ${BASE}/src
DOCDIR = ${BASE}/docs/

ANALYZER_SRCDIR=${SRCDIR}

ANALYZER_LIB_SRCDIR=${ANALYZER_SRCDIR}/analyzerLib
ANALYZER_DB_SRCDIR=${ANALYZER_SRCDIR}/analyzerDBAPI
ANALYZER_CORE_SRCDIR=${ANALYZER_SRCDIR}/analyzerCore
ANALYZER_AGENT_SRCDIR=${ANALYZER_SRCDIR}/analyzerAgent
ANALYZER_COMPILER_SRCDIR=${ANALYZER_SRCDIR}/analyzerCompiler
ANALYZER_CLIENT_SRCDIR=${ANALYZER_SRCDIR}/analyzerClientApi
ANALYZER_CLIENT_SRCDIR_2=${ANALYZER_SRCDIR}/analyzerClientApi2
ANALYZER_ACTION_SRCDIR=${ANALYZER_SRCDIR}/analyzerAgentAction
ANALYZER_CLI_SRCDIR=${ANALYZER_SRCDIR}/analyzerInterfaces/

ANALYZER_OBJDIR=${OBJDIR}/analyzer

ANALYZER_LIB_OBJDIR=${ANALYZER_OBJDIR}/analyzerLib
ANALYZER_DB_OBJDIR=${ANALYZER_OBJDIR}/analyzerDBAPI
ANALYZER_CORE_OBJDIR=${ANALYZER_OBJDIR}/analyzerCore
ANALYZER_AGENT_OBJDIR=${ANALYZER_OBJDIR}/analyzerAgent
ANALYZER_COMPILER_OBJDIR=${ANALYZER_OBJDIR}/analyzerCompiler
ANALYZER_CLIENT_OBJDIR=${ANALYZER_OBJDIR}/analyzerClientApi
ANALYZER_CLIENT_OBJDIR_2=${ANALYZER_OBJDIR}/analyzerClientApi2
ANALYZER_ACTION_OBJDIR=${ANALYZER_OBJDIR}/analyzerAgentAction
ANALYZER_CLI_OBJDIR=${ANALYZER_OBJDIR}/analyzerInterfaces






CC = g++
LINK_OPTIONS = -L${BINDIR} -L/usr/lib/ -L/lib64
COMPIL_OPTIONS = 
INCLUDES =

ifeq (${MODE},Debug)
	COMPIL_OPTIONS += -DDEBUG -g -pedantic -Wall -ansi 
endif	

ifneq (${HOSTNAME}, pclhcb178)
	LINK_OPTIONS += -L/usr/lib64/mysql
endif

BOOST_LIBS = /usr/lib64
BOOST_INC = /usr/include/boost
PYTHON_VERS = 2.7
##################################################################################################
# Done to compile with versions of boost that are not the one installed
ifeq (${USE_SPECIFIC_BOOST},True)

  OS = $(shell cat /etc/redhat-release | awk {'print $$(NF-1)'} | cut -d '.' -f 1)
  
  
  ifeq (${OS},5)
          OS_BUILD=slc5
          PYTHON_VERS=2.4
  else
  ifeq (${OS},6)
          OS_BUILD=slc6
          PYTHON_VERS=2.6
  else
          OS_BUILD=${OS}
          PYTHON_VERS=2.6
  endif
  endif
  
  BOOST_BUILD_DIR = /home/chaen/boostBuild/boost_1_42_0_${OS_BUILD}/
  BOOST_LIBS_SPE_DIR =  -L${BOOST_BUILD_DIR}/lib64/ 
  BOOST_INC = ${BOOST_BUILD_DIR}
  

endif
#####################################################################################################

PYTHON_INC = /usr/include/python${PYTHON_VERS}

LD_OPTION=
ifeq (${LD_MODE},static)
        LD_OPTION=-Wl,-Bstatic
endif

BOOST_LIBS =  ${BOOST_LIBS_SPE_DIR}  ${LD_OPTION} -lboost_system -lboost_serialization -lboost_thread -lboost_regex -lboost_program_options -lboost_filesystem -Wl,-Bdynamic -lpthread


