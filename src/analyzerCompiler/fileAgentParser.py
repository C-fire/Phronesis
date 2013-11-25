from pyparsing import *
import copy
import utils
from utils import MetaAgent
import runtime


FileAndFolderBaseAttribute = ['agentName', 'filename', 'owner', 'server', 'grp', 'permissions', 'attributes', 'md5']

class FileAgent(MetaAgent):

    filename = None
    owner = None
    grp = None
    attributes = None
    permissions = None
    md5 = None

    contents = None

    specialAttributes = ['content']
    allowedAttributes = ['agentName', 'filename', 'owner', 'server', 'grp', 'permissions', 'attributes', 'md5']
    #__allowedAttribute = FileAndFolderBaseAttribute
    
    ANY_TRIGGER = ['FILE_CREATE', 'FILE_CHGRP', 'FILE_CHOWN', 'FILE_CHMOD', 'FILE_CHATTR', 'FILE_PATTERN', 'FILE_MD5']


    def __init__(self, agentName):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        MetaAgent.__init__(self,agentName)
        self.contents = []
        
        self.owner = ''
        self.grp = ''
        self.attributes = ''
        self.permissions = ''
        self.md5 = ''

    def __dir__(self):
        return ['filename', 'owner', 'server', 'grp', 'permissions', 'attributes', 'md5' ]


    #TODO:
    # * escape strings properly with re.escape
    def addContent(self, content):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        self.contents.append(content)

    def isFullyDefined(self):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD :
            print utils.whoami()

        if self.isFullyDefinedVal is None:
            self.isFullyDefinedVal = True
            if self.agentName is None:
                print "%s is not defined because agentName is not set" % self.agentName
                self.isFullyDefinedVal = False
            if self.filename is None:
                print "%s is not defined because filename is not set" % self.agentName
                self.isFullyDefinedVal = False
            if self.server is None:
                print "%s is not defined because server is not set" % self.agentName
                self.isFullyDefinedVal = False
                
            if len(self.variables): #if there are variables not assigned
                print "%s is not defined because of variables unassigned %s " % (self.agentName, self.variables)
                self.isFullyDefinedVal = False

            for t in self.triggers :
                if not t.isFullyDefined():
                    self.isFullyDefinedVal = False

        return self.isFullyDefinedVal


    def setMotherAgent(self, mother):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD :
            print utils.whoami()
            print utils.whataremyargs()

        self.contents = copy.deepcopy(mother.contents)
        #self.envVarFile.agentName = 'babla'
        MetaAgent.setMotherAgent(self, mother)



 
    def __str__(self):
        retStr = "------------------------------\n"

        retStr += MetaAgent.__str__(self)
        if (len(self.contents)):
            retStr += "\ncontent:\n"
            for content in self.contents:
                retStr += "%s\n" % content

        retStr += "\n------------------------------\n"


        return retStr   

#agentName = Word(alphanums + '_')
#file_inheritance = utils.COLUMN + agentName("motherAgent")
#file_start = "FileAgent" + agentName("agentName") + Optional(file_inheritance) + utils.LBRACE
#file_variableAssignment = "set" + Word('$' + alphanums) + Word(alphanums + '/' + '_') + utils.SEMI
#file_content = "content" + utils.ASSIGN + utils.QUOTE + Word(alphanums + '() */\\=,_') + utils.QUOTE + utils.SEMI 
#general_file_attribute = Word(alphanums) + utils.ASSIGN + Word(alphanums + '$' + '/' + '.' + '_' + '-') + utils.SEMI
#file_trigger = oneOf("before after") + Word(alphanums + '_') + "trigger" + Word(alphanums + '_') + "on" + agentName + utils.SEMI
#file_end = utils.RBRACE
#
#file_middle = Or([general_file_attribute, file_variableAssignment, file_content, file_trigger])
#fileDef = file_start + ZeroOrMore(file_middle) + file_end

file_start = "FileAgent" + utils.metaAgent_start 
#file_variableAssignment = "set" + Word('$' + alphanums) + Word(alphanums + '/' + '_') + utils.SEMI



#with quotes needed
#file_content = "content" + utils.ASSIGN + utils.QUOTE + Word(alphanums + '() */\\=,_') + utils.QUOTE + utils.SEMI 
#without quotes needed
file_content = "content" + utils.ASSIGN + Word(alphanums + '(). */\\=,_-$^[]') + utils.SEMI 



#general_file_attribute = Word(alphanums) + utils.ASSIGN + Word(alphanums + '$' + '/' + '.' + '_' + '-') + utils.SEMI
#file_trigger = oneOf("before after") + Word(alphanums + '_') + "trigger" + Word(alphanums + '_') + "on" + agentName + utils.SEMI
#file_end = utils.RBRACE
file_end = utils.metaAgent_end;
#file_middle = Or([general_file_attribute, file_variableAssignment, file_content, file_trigger])
file_middle = Or([file_content, utils.metaAgent_middle])
fileDef = file_start + ZeroOrMore(file_middle) + file_end
fileDef.ignore(utils.comment)


def parseAction_file_start(token):
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print utils.whoami(), token
    if len(token) >= 2: # only "processAgent" and the name of the agent
        ag = FileAgent(token[1])
        runtime.core.addFileAgent(ag)

    if len(token) == 3:
        ag.setMotherAgent(runtime.core.getFileAgent(token[2]))

file_start.setParseAction(parseAction_file_start)

#def parseAction_general_file_attribute(token):
#    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
#        print utils.whoami(), token
#    runtime.core.currentFileAgent.addAttribute(token[0], token[1])
#general_file_attribute.setParseAction(parseAction_general_file_attribute)
#
#def parse_action_file_variableAssignment(token):
#    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
#        print utils.whoami(), token
#    runtime.core.currentFileAgent.assignVariable(token[1], token[2])
#file_variableAssignment.setParseAction(parse_action_file_variableAssignment)
#

def parse_action_file_content(token):
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print utils.whoami(), token

    # I don't use runtime.core.currentFileAgent because
    # this function might be called by a folderAgent
    #runtime.core.currentFileAgent.addContent(token[1])
    runtime.core.currentMetaAgent.addContent(token[1])
file_content.setParseAction(parse_action_file_content)


#def parse_action_file_trigger(token):
#    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
#        print utils.whoami(), token
#    #runtime.core.currentFileAgent.addTrigger((token[0], token[1], token[3], runtime.core.getEntity(token[5])))
#    
#   #Replace list by object 
#    #runtime.core.currentFileAgent.addTrigger([token[0], token[1], token[3], token[5]])
#    runtime.core.currentFileAgent.addTrigger(utils.Trigger(token[0], token[1], token[3], token[5]))
#    
#file_trigger.setParseAction(parse_action_file_trigger)

