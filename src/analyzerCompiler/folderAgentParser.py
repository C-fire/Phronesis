from pyparsing import *
import copy
import utils
from utils import MetaAgent
import runtime
#from fileAgentParser import FileAgent,file_middle
import fileAgentParser



class FolderAgent(fileAgentParser.FileAgent):

    allowedAttributes = fileAgentParser.FileAndFolderBaseAttribute + ['maxDepth']    
    specialAttributes = ['content', 'nameFilter']

    filenameFilters = None

    def __init__(self, agentName):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        fileAgentParser.FileAgent.__init__(self,agentName)
        self.maxDepth = 0
        self.filenameFilters = []



    def addFilenameFilter(self, nameFilter):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        self.filenameFilters.append(nameFilter)
    
        


folder_start = "FolderAgent" +  utils.metaAgent_start
folder_end = utils.RBRACE
folder_filenameFilter = "nameFilter" + utils.ASSIGN + Word(alphanums + '(). */\\=,_-') + utils.SEMI
folder_middle = Or([folder_filenameFilter, fileAgentParser.file_middle])
folderDef = folder_start + ZeroOrMore(folder_middle) + folder_end
folderDef.ignore(utils.comment)



def parseAction_folder_start(token):
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print utils.whoami(), token
    if len(token) >= 2: # only "processAgent" and the name of the agent
        ag = FolderAgent(token[1])
        runtime.core.addFolderAgent(ag)

    if len(token) == 3:
        ag.setMotherAgent(runtime.core.getFolderAgent(token[2]))

folder_start.setParseAction(parseAction_folder_start)



def parse_action_folder_filenameFilter(token):
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print utils.whoami(), token

    runtime.core.currentFolderAgent.addFilenameFilter(token[1])
folder_filenameFilter.setParseAction(parse_action_folder_filenameFilter)




#def parse_action_folder_def(token):
#    print "I just defined the folder ",token
#folderDef.setParseAction(parse_action_folder_def)

#for folder in folderDef.searchString(folderTest):
#    print 'dump ', folder



#runtime.core.listFolderAgents()
