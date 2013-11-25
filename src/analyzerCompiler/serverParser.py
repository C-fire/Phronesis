from pyparsing import *
import copy
import utils
import runtime
from  fileAgentParser import FileAgent
from sets import Set

class EnvironmentAgent:
    
    agentName = None
    server = None
    
    #THOSE VARIABLES ARE NOT YET AVAILABLE IN THE DB. FUTURE USE
    maxLoad = None
    maxMemory = None
    maxSwap = None
    
    __fstabAgent = None
    
    __variables = None
    
    allowedAttributes = ['agentName', 'address', 'server', 'maxLoad', 'maxMemory', 'maxSwap']
    specialAttributes = ['ignoreFs']

    __isFullyDefined = None

    ignoredFS = None
    
    def __init__(self, agentName):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

	self.maxLoad = 2
	self.maxMemory = 100
	self.maxSwap = 99
        self.ignoredFS = Set([])

        self.agentName = agentName
        
    def __dir__(self):
        return []
        
    def setFstab(self, fstabAgent):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        self.__fstabAgent = fstabAgent
        
    def getFstab(self):
        if runtime.DEBUG_MODE & runtime.OBJ_VERBOSE:
            print utils.whoami()

        return self.__fstabAgent
        
    
    def isFullyDefined(self):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD :
            print utils.whoami()

        if self.__isFullyDefined is None:
            self.__isFullyDefined = True
            if self.agentName is None:
                self.__isFullyDefined = False
            if self.server is None:
                self.__isFullyDefined = False

        return self.__isFullyDefined
 

    #TODO
    # * check if attribute is in __allowedAttribute, and if it matches the type
    # * throw exception if variable name already exists -> NO!! can be used by several field
    def addAttribute(self, attribute, value):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        if (value[0] == '$'):
            if (value in self.__variables): #if the variable name already exists
                self.__variables[value].append(attribute)
            
            else:
                self.__variables[value] = [attribute]
        #setattr(self, attribute, value)
        if attribute in self.allowedAttributes:
            setattr(self, attribute, value)
        elif attribute == "mountpoint":
            self.__fstabAgent.addContent(value)
        elif attribute == "ignoreFs":
            self.ignoredFS.add(value)
        else:
            print "SPE (%s, %s)"%(attribute, value)
        
            
    def recursiveRename(self, newName):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()
        
        self.__fstabAgent.agentName = newName + '_fstab'

    def __str__(self):
#        retStr = "------------------------------\n"
        retStr = "\n"
        for arg in self.__dir__():
            retStr += "%s -> %s;\n" % (arg, getattr(self, arg))
        retStr += "\n"
        retStr += "IgnoredFS\n"
        for fs in self.ignoredFS:
            retStr += "%s\n"%fs
        retStr += "\n"
        #retStr += "\n------------------------------\n"


        return retStr 

serverName = Word(alphanums)
mountpointDefinition = Word(alphanums + '() */\\=,_:')
mountpoint_env = "mountpoint" + mountpointDefinition + utils.SEMI
load_env = "maxLoad" + Word(nums) + utils.SEMI
memory_env = "maxMemory" + Word(nums) + utils.SEMI
swap_env = "maxSwap" + Word(nums) + utils.SEMI
env_constraint = Or([mountpoint_env, load_env, memory_env, swap_env])
setEnvDef = "setEnv" + Word(alphanums)("serverName") + env_constraint 


def parseAction_setEnvDef(token):
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print utils.whoami(), token

    serverName = token[1]
    server = runtime.core.getServer(serverName)
    server.addAttribute(token[2], token[3])
    #runtime.core.currentServer.addAttribute(token[0], token[1])
    
setEnvDef.setParseAction(parseAction_setEnvDef)


# TODO :
# * plan inheritance for Server

class Server:
    
    agentName = None
    address = None
    
    allowedAttributes = ['agentName', 'address']

    attachedCoords = None    
    attachedCoordNames = None    
    
    __envAgent = None
    
    __isFullyDefined = None
    
    __motherAgent = None
    __childrenAgents = None

    
    def __init__(self, agentName):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        self.agentName = agentName
        
        self.__isFullyDefined = None
        self.__childrenAgents = {}
        self.attachedCoords = []
        self.attachedCoordNames = []
        
    def __dir__(self):
        return ['address']

    #TODO
    # * check if attribute is in __allowedAttribute, and if it matches the type
    def addAttribute(self, attribute, value):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        if attribute in self.allowedAttributes:
            setattr(self, attribute, value)
        else:
            self.__envAgent.addAttribute(attribute, value)
        for child in self.__childrenAgents.values():
            child.addAttribute(attribute, value)
        
    def setEnvironmentAgent(self, envAgent):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        self.__envAgent = envAgent
        
    def isFullyDefined(self):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD :
            print utils.whoami()

        if self.__isFullyDefined is None:
            self.__isFullyDefined = True
            if self.agentName is None:
                self.__isFullyDefined = False
            if self.address is None:
                self.__isFullyDefined = False
            
            #if not self.__isFullyDefined:
            #    self.__envAgent.setIsFullyDefined(False)



        return self.__isFullyDefined
 

    def addAttachedCoordinatorName(self, coordName):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        self.attachedCoordNames.append(coordName)
        print "ATTACHED %s"%self.attachedCoordNames

    def removeAttachedCoordinatorName(self, coordName):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        try:
            self.attachedCoordNames.remove(coordName)
        except:
            raise 'No Coordinator "%s" attached to server %s'%(coordName, self.agentName)
        
    def addAttachedCoordinator(self, coord):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        if coord not in self.attachedCoords:
            self.attachedCoords.append(coord)
        #for child in self.__childrenAgents.values():
        #    child.addAttachedCoordinator(coord)


    def recursiveRename(self, newName):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()
        
        self.__envAgent.agentName = newName + '_env'
        self.__envAgent.recursiveRename(newName)

    def getEnvironmentAgent(self):
        if runtime.DEBUG_MODE & runtime.OBJ_VERBOSE:
            print utils.whoami()

        return self.__envAgent

    def getChildrenAgents(self):
        if runtime.DEBUG_MODE & runtime.OBJ_VERBOSE:
            print utils.whoami()

        return self.__childrenAgents

    def setChildrenAgents(self, children):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        self.__childrenAgents = children


    #TODO
    # * catch the exception when the mother does not exist 
    def setMotherAgent(self, mother, realName):

#        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
#            print utils.whoami()
#            print utils.whataremyargs()
#
#        self.agentName = realName
#        self.__realName = realName
#        #if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
#        #    print self.agentName, " inherits from ", mother.agentName
#        self.__motherAgent = mother
#        mother.__childrenAgents[self.agentName] = self
#
#
#        self.recursiveRename('_' + self.agentName)
#        self.__envAgent.server = self.agentName
#        self.__envAgent.getFstab().server = self.agentName
#

        #More recent system, but with a flaw
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()
        
        self.agentName = realName
        #if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
        #    print self.agentName, " inherits from ", mother.agentName
        self.__motherAgent = mother
        mother.__childrenAgents[self.agentName] = self
        self.recursiveRename(self.agentName)
        self.__envAgent.server = self.agentName
        self.__envAgent.getFstab().server = self.agentName



        #OLD SYSTEM, DOES NOT WORK BUT KEEP FOR RECORD
        #copy of the mother attributes
        #self.__variables = copy.deepcopy(mother.__variables)
        #self.__needsDic = copy.deepcopy(mother.__needsDic)
        
        #self.__needsAssignmentDic = copy.deepcopy(mother.__needsAssignmentDic )
        #self.__fileAgentList = copy.deepcopy(mother.__fileAgentList)
        #self.__processAgentList = copy.deepcopy(mother.__processAgentList)
        #self.__coordinatorList = copy.deepcopy(mother.__coordinatorList)

        #NEW METHOD, NOT SURE IF THIS WILL REALLY WORK...
        #realName = self.agentName
        #self = copy.deepcopy(mother)
        #self.agentName = realName




    def __str__(self):
        retStr = "------------------------------\n"
        for arg in self.__dir__():
            retStr += "%s -> %s;\n" % (arg, getattr(self, arg))

        if len(self.attachedCoordNames):
            retStr += "AttachedCoordinatorNames:\n"
            for attached in self.attachedCoordNames:
                retStr += "    %s"%attached

        retStr += "\n%s\n" % self.__envAgent

        retStr += "\n------------------------------\n"


        return retStr     


#ipAddress = Combine(Word(nums) + ('.' + Word(nums)) * 3)
#dnsAddress = Word(alphanums)
#address = Or([ dnsAddress, ipAddress])
#location = Word(alphanums)
#serverName = Word(alphanums)
#server_start = "Server" + serverName("serverName") + utils.LBRACE
#server_address = "address" + utils.ASSIGN + address("address") + utils.SEMI
##server_location = "location" + utils.ASSIGN + location("location") + utils.SEMI
#server_attributes = Or([server_address, utils.wrong_attributes])
#server_end = utils.RBRACE
#serverDef = server_start + OneOrMore(server_attributes) + server_end


agentName = Word(alphanums + '_')
server_inheritance = utils.COLUMN + agentName("motherAgent")
server_start = "Server" + agentName("serverName") +  Optional(server_inheritance) + utils.LBRACE
general_server_attribute = Word(alphanums)  + utils.ASSIGN + Word( '$' + alphanums + '$:(). */\\=,_') + utils.SEMI
server_attachCoord = "attach" + Word(alphanums+ '_') + utils.SEMI
server_detachCoord = "detach" + Word(alphanums+ '_') + utils.SEMI
server_end = utils.RBRACE
server_middle = Or([general_server_attribute, server_attachCoord, server_detachCoord])
#serverDef = server_start + ZeroOrMore(general_server_attribute) + server_end
serverDef = server_start + ZeroOrMore(server_middle) + server_end
serverDef.ignore(utils.comment)

def parseAction_server_start(token):
    print "SERVER START MERDE"
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print utils.whoami(), token

    ag = Server(token[1])

    if len(token) == 2: #no inheritance

        env = EnvironmentAgent("%s_env" % token[1])
        env.addAttribute("server", token[1])
        fstabAg = FileAgent("%s_fstab" % token[1])
        fstabAg.addAttribute("server", token[1])
        fstabAg.addAttribute('filename', '/etc/fstab')
        env.setFstab(fstabAg)
        ag.setEnvironmentAgent(env)
        
    if len(token) == 3:

        # If just making a deepcopy, we copy everything, including the __childrenAgents attribute,
        # which is huge and useless (and wrong I think...). The memory grows exponentially,
        # and the parsing is stuck. So we save the __childrenAgents attribute of the mother,
        # empty it, copy the mother, and put it back to her
        motherChildren = runtime.core.getServer(token[2]).getChildrenAgents()
        runtime.core.getServer(token[2]).setChildrenAgents({})
        ag = copy.deepcopy(runtime.core.getServer(token[2]))
        runtime.core.getServer(token[2]).setChildrenAgents(motherChildren)

        ag.setMotherAgent(runtime.core.getServer(token[2]), token[1])




        #ag = copy.deepcopy(runtime.core.getServer(token[2]))
        #ag.setMotherAgent(runtime.core.getServer(token[2]), token[1])
        
    runtime.core.addServer(ag)
#    runtime.core.addEnvironmentAgent(env)
#    runtime.core.addFileAgent(fstabAg)


server_start.setParseAction(parseAction_server_start)

def parseAction_general_server_attribute(token):
    print "SERVER ATTR"
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print utils.whoami(), token

    runtime.core.currentServer.addAttribute(token[0], token[1])
    
general_server_attribute.setParseAction(parseAction_general_server_attribute)



def parseAction_server_attachCoord(token):
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print utils.whoami(), token

    runtime.core.currentServer.addAttachedCoordinatorName(token[1])
    
server_attachCoord.setParseAction(parseAction_server_attachCoord)


def parseAction_server_detachCoord(token):
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print utils.whoami(), token

    runtime.core.currentServer.removeAttachedCoordinatorName(token[1])
    
server_detachCoord.setParseAction(parseAction_server_detachCoord)
