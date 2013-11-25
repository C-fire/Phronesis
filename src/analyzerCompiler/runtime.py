core = None
fileDir = '/home/chaen/phronesis/phrRealFiles/v2'
#fileDir = '/home/chaen/phronesis/statistics/chep1/phrFiles/'
#fileDir = '/home/chaen/phronesis/src/analyzer/analyzerCompiler/myParser/testFiles/misc/2013-03-07'
#fileDir = './testFiles/misc/matching2'
#fileDir = '/home/chaen/phronesis/src/analyzer/analyzerCompiler/myParser/testFiles/abstracts/apache'
#fileDir = './testFiles/confWeb/'
#fileDir = '/tmp/phr'
currentFile = None
#debug = 0
debug = 1
#compareWithDb = True
compareWithDb = False

CORE_SUMMARY  =  1
CORE_METHOD   =  2
CORE_VERBOSE  =  4
OBJ_METHOD    =  8
OBJ_TRIGGER   =  16
OBJ_VERBOSE   =  32
TREE_SUMMARY  =  64
TREE_METHOD   =  128
TREE_VERBOSE  =  256
TOPO_SUMMARY  =  512
TOPO_METHOD   =  1024
TOPO_VERBOSE  =  2048
ALL_VERBOSE   = CORE_SUMMARY|CORE_METHOD|CORE_VERBOSE|OBJ_METHOD|OBJ_TRIGGER|OBJ_VERBOSE|TREE_SUMMARY|TREE_METHOD|TREE_VERBOSE|TOPO_SUMMARY|TOPO_METHOD|TOPO_VERBOSE


DEBUG_MODE = ALL_VERBOSE

