from pyparsing import *
import copy
from utils import *
import utils
import runtime
from fileAgentParser import FileAgent
import sys

class ProcessAgent(MetaAgent):

    procName = None
    user = None
    server = None
    command = None
    service = None
    maxCpu = None
    maxMemory = None
    multiplicity = None
    
    allowedAttributes = ['agentName', 'procName', 'user', 'server', 'command', 'service', 'maxCpu', 'maxMemory', 'multiplicity']

    
    __parentProcessAgent = None
    __childProcessAgent = None


    __limit_names = {"cpu_time" : 0,
                    "file_size" : 1,
                    "data_size" : 2,
                    "stack_size" : 3,
                    "core_file_size" : 4,
                    "resident_set" : 5,
                    "processes" : 6,
                    "open_files" : 7,
                    "locked_memory" : 8,
                    "address_space" : 9,
                    "file_locks" : 10,
                    "pending_signals" : 11,
                    "msgqueue_size" : 12,
                    "nice_priority" : 13,
                    "realtime_priority" : 14,
                    "realtime_timeout" : 15
                    }
    __limits = None

    ANY_TRIGGER = ["PROCESS_START", "PROCESS_STOP", "PROCESS_RESTART", "PROCESS_ULIMIT", "PROCESS_CHUSER", "PROCESS_ENVIRONMENT",  "SERVICE_START", "SERVICE_STOP",  "SERVICE_RESTART", "SERVICE_RELOAD","SERVICE_CHUSER", "SERVICE_ENVIRONMENT"]



    #envVariables = None
    envVarFile = None
    envVarFileUsed = False


    def __init__(self, agentName):
        MetaAgent.__init__(self,agentName)
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        self.__limits = {}
        self.command = ''
        self.service = ''
        self.maxCpu = 100
        self.maxMemory = 100 
        self.multiplicity = -1

        #self.envVariables = []
        

    def __dir__(self):
        return [ 'procName', 'user', 'server', 'command', 'service', 'maxCpu', 'maxMemory', 'multiplicity' ]

    #TODO
    # * check if attribute is in __allowedAttribute, and if it matches the type
    # * throw exception if variable name already exists -> NO!! can be used by several field
    # * make sur that only either the command either the service is given, not both
    def addAttribute(self, attribute, value):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        if attribute in self.__limit_names.keys():
            self.__limits[self.__limit_names[attribute]] = value
        elif attribute == "envVar":
            self.envVarFileUsed = True
            self.envVarFile.addContent(value)
        else:
            MetaAgent.addAttribute(self,attribute,value)
        if attribute == "server":
            self.envVarFile.addAttribute(attribute, value)


    def setParentProcessAgent(self, parent):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print self.agentName, " has parents ", parent.agentName
        self.__parentProcessAgent = parent
        parent.__childProcessAgent = self
        #copy of the parent attributes
        self.variables = copy.deepcopy(parent.variables)
        self.__limits = copy.deepcopy(parent.__limits)
        for arg in self.__dir__():
            setattr(self, arg, getattr(parent, arg))

        # TEST TO SEE IF my envFile can inherit from the one of my parent process
        self.envVarFileUsed = parent.envVarFileUsed
        self.envVarFile.setMotherAgent(parent.envVarFile)

    def getChildProcessAgent(self):
        if runtime.DEBUG_MODE & runtime.OBJ_VERBOSE:
            print utils.whoami()
          
        return self.__childProcessAgent
    
    def getParentProcessAgent(self):
        if runtime.DEBUG_MODE & runtime.OBJ_VERBOSE:
            print utils.whoami()
            
        return self.__parentProcessAgent
    
    def getLimits(self):
        if runtime.DEBUG_MODE & runtime.OBJ_VERBOSE:
            print utils.whoami()
            
        return self.__limits

    #TODO
    # * throw exception if the variable is not in the __variables dic -> NO!! can be use in childProcess
    def assignVariable(self, varName, value):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        MetaAgent.assignVariable(self,varName,value)
        if self.__childProcessAgent:
            self.__childProcessAgent.assignVariable(varName, value)
        self.envVarFile.assignVariable(varName, value)



    def recursiveRename(self, suffix):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        MetaAgent.recursiveRename(self,suffix)
        if self.__childProcessAgent:
            self.__childProcessAgent.recursiveRename( "_c_" + suffix)
        self.envVarFile.recursiveRename(suffix)



    def setMotherAgent(self, mother):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD :
            print utils.whoami()
            print utils.whataremyargs()

        self.envVarFile = copy.deepcopy(mother.envVarFile)
        #self.envVarFile.agentName = 'babla'
        self.envVarFile.recursiveRename(self.agentName)
        MetaAgent.setMotherAgent(self, mother)
        





    def isFullyDefined(self):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()

        if self.isFullyDefinedVal is None:
            self.isFullyDefinedVal = True
            if self.agentName is None:
                print "%s is not defined because agentName is not set" % self.agentName
                self.isFullyDefinedVal = False
            if self.procName is None:
                print "%s is not defined because procName is not set" % self.agentName
                self.isFullyDefinedVal = False
            if self.server is None:
                print "%s is not defined because server is not set" % self.agentName
                self.isFullyDefinedVal = False
            if self.__childProcessAgent:
                if not self.__childProcessAgent.isFullyDefined():
                    print "%s is not defined because child %s is not defined" % (self.agentName, self.__childProcessAgent.agentName)
                    self.isFullyDefinedVal = False
            if self.__parentProcessAgent:
                if not self.__parentProcessAgent.isFullyDefined():
                    print "%s is not defined because parent %s is not defined" % (self.agentName, self.__parentProcessAgent.agentName)
                    self.isFullyDefinedVal = False
            for t in self.triggers : 
                if not t.isFullyDefined():
                    print "%s is not defined because a trigger is not defined" % (self.agentName)
                    self.isFullyDefinedVal = False
                    
            if len(self.variables): #if there are variables not assigned
                print "%s is not defined because of variables unassigned %s " % (self.agentName, self.variables)
                self.isFullyDefinedVal = False

            if not self.isFullyDefinedVal:
                if self.envVarFile:
                    print "I set isFullyDefined to False to %s" % self.envVarFile.agentName
                    self.envVarFile.setIsFullyDefined(False)

        
        return self.isFullyDefinedVal
        
    def __str__(self):

        retStr = "------------------------------\n"
	retStr += MetaAgent.__str__(self)
        if self.__parentProcessAgent:
            retStr += "Parent %s (%s)\n" % (self.__parentProcessAgent.agentName, id(self.__parentProcessAgent))
        if self.__childProcessAgent:
            retStr += "ChildProcess %s (%s)\n" % (self.__childProcessAgent.agentName, id(self.__childProcessAgent))
            
        for limit, val in self.__limits.items():
            retStr += "limit : %s -> %s\n" % (limit, val)

        retStr += "envVarFileUsed :%s\n"%(self.envVarFileUsed)

        try: #depending on when we print, we might already have removed the envVarFile
            retStr += "envVarFile : %s"%self.envVarFile.agentName
        except:
            pass
            
        retStr += "\n------------------------------\n"

        return retStr   


#agentName = Word(alphanums + '_')
#procName = Word(alphanums + '*')
#user = Word(alphanums)
#procServer = Word(alphanums)
#process_inheritance = COLUMN + agentName("motherAgent")
#process_start = "ProcessAgent" + agentName("agentName") + Optional(process_inheritance) + LBRACE
#
#process_variableAssignment = "set" + Word('$' + alphanums + '_') + Word(alphanums + '_') + SEMI
#parent_attribute = "parent" + ASSIGN + Word(alphanums + '_') + SEMI
#general_attribute = Word(alphanums + '_') + ASSIGN + Word(alphanums + '$' + '/' + '*' + '_' + ' ' + '-' + '.' + '=') + SEMI
#process_trigger = oneOf("before after") + Word(alphanums + '_') + "trigger" + Word(alphanums + '_') + "on" + agentName + utils.SEMI
#
#process_midle = Or([general_attribute,process_variableAssignment, process_trigger])
#
#process_end = RBRACE
#
##processDef = process_start + Optional(parent_attribute) + Dict(Group(ZeroOrMore(general_attribute))).setResultsName("myattributes") + Dict(Group(ZeroOrMore(process_variableAssignment))).setResultsName("varAssignment") + process_end
#processDef = process_start + Optional(parent_attribute)  + ZeroOrMore(process_midle) + process_end



process_start = "ProcessAgent" + utils.metaAgent_start
parent_attribute = "parent" + utils.ASSIGN + Word(alphanums + '_') + utils.SEMI
process_midle = Or([utils.metaAgent_middle])
#process_attribute = Word(alphanums + '_') + utils.ASSIGN + Word(alphanums + '$' + '/' + '*' + '_' + ' ' + '-' + '.' + '=') + utils.SEMI
#process_midle = Or([process_attribute, utils.metaAgent_variableAssignment, utils.metaAgent_trigger])
process_end = utils.metaAgent_end

#processDef = process_start + Optional(parent_attribute) + Dict(Group(ZeroOrMore(general_attribute))).setResultsName("myattributes") + Dict(Group(ZeroOrMore(process_variableAssignment))).setResultsName("varAssignment") + process_end
processDef = process_start + Optional(parent_attribute)  + ZeroOrMore(process_midle) + process_end
processDef.ignore(utils.comment)

def parseAction_process_start(token):
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print utils.whoami(), token
    if len(token) >= 2: # only "processAgent" and the name of the agent
        ag = ProcessAgent(token[1])

    if len(token) == 2: # only "processAgent" and the name of the agent

        envVarFile = FileAgent("envVariables_" + token[1] )
        #envVarFile.recursiveRename(token[1])
        envVarFile.addAttribute('filename', '/proc/[pid]/env')
        ag.envVarFile = envVarFile

    if len(token) == 3:
        try:
          ag.setMotherAgent(runtime.core.getProcessAgent(token[2]))
        except Exception as inst:
          print "Cannot find mother agent called %s"%token[2]
          sys.exit(1) 

    runtime.core.addProcessAgent(ag)

process_start.setParseAction(parseAction_process_start)

def parseAction_general_attribute(token):
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print utils.whoami(), token
    runtime.core.currentProcessAgent.addAttribute(token[0], token[1])
#general_attribute.setParseAction(parseAction_general_attribute)
#process_attribute.setParseAction(parseAction_general_attribute)

def parseAction_parent_attribute(token):
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print utils.whoami(), token
    runtime.core.currentProcessAgent.setParentProcessAgent(runtime.core.getProcessAgent(token[1]))
parent_attribute.setParseAction(parseAction_parent_attribute)


def parse_action_process_variableAssignment(token):
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print utils.whoami(), token
    runtime.core.currentProcessAgent.assignVariable(token[1], token[2])
#process_variableAssignment.setParseAction(parse_action_process_variableAssignment)

def parse_action_process_trigger(token):
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print utils.whoami(), token
    #runtime.core.currentFileAgent.addTrigger((token[0], token[1], token[3], runtime.core.getEntity(token[5])))

    #Replace list by object 
    #runtime.core.currentFileAgent.addTrigger([token[0], token[1], token[3], token[5]])
    runtime.core.currentProcessAgent.addTrigger(utils.Trigger(token[0], token[1], token[3], token[5]))

#process_trigger.setParseAction(parse_action_process_trigger)


#for proc in processDef.searchString(procTest):
#    print 'dump ', proc



#core.listProcessAgents()
