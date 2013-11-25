#!/usr/bin/python

# TODO
# * add a 'commit' like thing when an object is fully read. Otherwise it can be partially created. It sucks...
# * add support for comment inside an object definition
# * for the time being, all the "set variable" have to be done at the end, otherwise they are not propagated
# * add the support for the classification of the Coordinators
# * add the support for the limits and the environment file for processes 
# * add support for variables in the limits



# When inheriting, assigned value should also take into account variable.
# for example : 
#FileAgent fileA{
#        filename -> $var;
#        server -> x;
#}
#FileAgent FileC : fileA {
#        filename -> other;
#}
#
#Will not work for FileC because we still have $var in the  var assignment need

# * if we put an abstract entity into a coordinator, it is like putting all those that inherits from the abstract. I don't know
#    how to do this yet, but it would be cool


import utils
import serverParser
import processAgentParser
import fileAgentParser
import folderAgentParser
import coordinatorParser
import runtime
import sys
import os

if len(sys.argv) == 2:
    runtime.fileDir = sys.argv[1]

runtime.core = utils.Core()
orderedFileList = utils.buildSingleCompiledFile(runtime.fileDir)
print orderedFileList

functionList = ["serverParser", "processAgentParser", "fileAgentParser", "folderAgentParser", "coordinatorParser", "setEnvParser"]
functionDic = {"serverParser" : serverParser.serverDef.searchString,
               "processAgentParser" : processAgentParser.processDef.searchString,
               "fileAgentParser": fileAgentParser.fileDef.searchString,
               "folderAgentParser": folderAgentParser.folderDef.searchString,
               "coordinatorParser" :  coordinatorParser.coordinatorDef.searchString,
               "setEnvParser" : serverParser.setEnvDef.searchString}


for function in functionList:
    for currentFile in orderedFileList:
        runtime.currentFile = currentFile
        print "Doing %s in %s" % (function, currentFile)

        #phrFile = open(os.path.join(runtime.fileDir, currentFile), 'r')
        phrFile = open(currentFile, 'r')
        phrData = phrFile.read()
        functionDic[function](phrData)
        phrFile.close()

#        processAgentParser.processDef.searchString(phrData)
#        fileAgentParser.fileDef.searchString(phrData)
#        coordinatorParser.coordinatorDef.searchString(phrData)

#if runtime.debug:
#    runtime.core.listServers()
#    runtime.core.listEnvironmentAgents()
#    runtime.core.listProcessAgents()
#    runtime.core.listFileAgents()
#    runtime.core.listFolderAgents()
#    runtime.core.listCoordinators()


runtime.core.assignServerAttachments()
runtime.core.deleteUselessEnvVarFiles()
runtime.core.setClassification()
if runtime.debug:
    runtime.core.listServers()
    runtime.core.listEnvironmentAgents()
    runtime.core.listProcessAgents()
    runtime.core.listFileAgents()
    runtime.core.listFolderAgents()
    runtime.core.listCoordinators()
    runtime.core.listFullyDefinitionStatus()

runtime.core.compareWithDb()
runtime.core.exportToSql(os.path.join(runtime.fileDir, "output.sql"), "/home/chaen/phronesis/phronesisAnalyzer.sql")
print "I really arrive at the end"
