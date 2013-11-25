from pyparsing import *
import runtime
import os
import sys
import topologicSort as ts
import string
import copy
import TreeComparison
import inspect



############ TO BE USED TO PRINT THE LINE NUMBER 
#def errorMsg(st,locn,tokens):
#    print 'Unknown attribute %s at line %d'%(tokens[0],lineno(locn,st))
#    sys.exit(1)
#
#wrong_attributes.setParseAction(errorMsg)
################################################





def get_class_from_frame(fr):
    args, _, _, value_dict = inspect.getargvalues(fr)
    # we check the first parameter for the frame function is
    # named 'self'
    if len(args) and args[0] == 'self':
        # in that case, 'self' will be referenced in value_dict
        instance = value_dict.get('self', None)
        if instance:
            # return its class
            v = "%s"%getattr(instance, '__class__', None)
            instName = ""
            try:
                instName = "(%s)"%instance.agentName,
            except:
                pass
            return "%s%s::"%(v.split('.')[-1],instName[0]),
    # return None otherwise
    return ""


#def whoami():
#    get_class_from_frame(inspect.stack()[1][0])
#    print inspect.stack()[1][3]


def whoami():
#    return "%s%s"%( get_class_from_frame(inspect.stack()[1][0])[0], inspect.stack()[1][3])
    className = ""
    funcName = inspect.stack()[1][3]
    try:
        className = get_class_from_frame(inspect.stack()[1][0])[0]
    except:
        pass
    return "%s%s"%( className, funcName)


def whosdaddy():
    return inspect.stack()[2][3]

def whataremyargs():
    frame = inspect.stack()[1][0]
    args, _, _, values = inspect.getargvalues(frame)
    argsvals = []
    for i in args:
        #Here we can filter ...
        if i == 'self':
            continue
        argsvals.append((i, values[i]))

    return argsvals


class Trigger:
    
    when = None
    firstAction = None
    secondMa = None
    secondAction = None
    
    latestSuffix = None
    useSuffix = True
    #variables = None

    isFullyDefinedVal = None

    def __init__(self, when, firstAction, secondAction, secondMa):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print whoami()
            print whataremyargs()

        self.when = when
        self.firstAction = firstAction 
        self.secondAction = secondAction
        self.secondMa = secondMa
        self.latestSuffix = ""
        self.variables = []
        
    def rename(self, suffix):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print whoami()
            print whataremyargs()

        self.latestSuffix = suffix

    def assignVariable(self, varName, value):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print whoami()
            print whataremyargs()

        if self.secondMa == varName:
            self.secondMa = value
            self.useSuffix = False

    def setSuffix(self, suffix):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print whoami()
            print whataremyargs()

        self.latestSuffix = suffix


    def isFullyDefined(self):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print whoami()

        if self.isFullyDefinedVal is None:
            self.isFullyDefinedVal = True
            if self.secondMa[0] == '$': #if there are variables not assigned
                print "Trigger is not defined because of variables unassigned (%s) " % (self)
                self.isFullyDefinedVal = False

        return self.isFullyDefinedVal



    def __str__(self):
	return "%s %s %s %s (%s)"%(self.when,self.firstAction,self.secondAction, self.secondMa,self.latestSuffix) 

LBRACE, RBRACE, SEMI, QUOTE, COLUMN, EQUAL = map(Suppress, '{};":=')
ASSIGN = Suppress("->")
comment = '#' + restOfLine
wrong_attributes = Word(alphanums) + ASSIGN + Word(alphanums) + SEMI
myPrintable = printables.strip('\n{}/')
#importStatment = "import" + Word(alphanums + './_') + ".phr" + SEMI
importStatment = LineStart() + "import" + Word(alphanums + './_') + SEMI
importStatment.ignore(comment)

# TODO
# * enforce unicty of the name on a general scope.
# * make it more transparent to the type of entity

class Core:

    agentName = ""

    __metaAgentDic = None

    __processAgentDic = None
    __processAgentList = None #The list is ordered. It is required in order to output them in the proper order for SQL
    __fileAgentDic = None
    __folderAgentDic = None
    __environmentAgentDic = None
    __coordinatorDic = None
    __serverDic = None

    __total = None
    __occurrence = None

    currentProcessAgent = None
    currentFileAgent = None
    currentFolderAgent = None
    currentEnvironmentAgent = None
    currentCoordinator = None
    currentServer = None

    dependencyRules = None

    currentMetaAgent = None

    #counters
    __idAgent = None
    __idLink = None
    __idDependencyRule = None
    __idFile = None
    __idProcess = None
    __idEnvironment = None
    __idServer = None
    __idMetaAgentTree = None

    tc = None #Tree comparison object


    def __init__(self):
        if runtime.DEBUG_MODE & runtime.CORE_METHOD:
            print whoami()

        self.__metaAgentDic = {}
        self.__processAgentDic = {}
        self.__processAgentList = []
        self.__fileAgentDic = {}
        self.__folderAgentDic = {}
        self.__environmentAgentDic = {}
        self.__coordinatorDic = {}
        self.__serverDic = {}
        self.__total = 0
        self.__occurrence = 0
        self.__idAgent = 1
        self.__idFile = 1
        self.__idProcess = 1
        self.__idEnvironment = 1
        self.__idMetaAgentTree = 1
        self.__idServer = 1
        self.__idLink = 0
        self.__idDependencyRule = 0

        

    #TODO
    # *add a control that the agent does not exist yet
    def addProcessAgent(self, newAgent):
        if runtime.DEBUG_MODE & runtime.CORE_METHOD:
            print whoami()
            print whataremyargs()

        if newAgent.agentName in self.__metaAgentDic.keys():
          raise Exception("Agent %s already exists"%newAgent.agentName)
        self.currentProcessAgent = newAgent
	self.currentMetaAgent = newAgent
        self.__processAgentDic[newAgent.agentName] = newAgent
        self.__metaAgentDic[newAgent.agentName] = newAgent
        self.__processAgentList.append(newAgent)
        newAgent.idAgent = self.__idAgent 
        self.__idAgent += 1
        newAgent.idProcess = self.__idProcess
        self.__idProcess += 1
        print "CORE : I add %s (id : %s , pid: %s)"%(newAgent.agentName, newAgent.idAgent, newAgent.idProcess)

        envVarFile = newAgent.envVarFile
        print "THE ENVVAR %s"%envVarFile.agentName
        self.__metaAgentDic[envVarFile.agentName] = envVarFile
        self.__fileAgentDic[envVarFile.agentName] = envVarFile
        envVarFile.idAgent = self.__idAgent
        self.__idAgent += 1
        envVarFile.idFile = self.__idFile
        self.__idFile += 1
      

    def getProcessAgent(self, agentName):
        if runtime.DEBUG_MODE & runtime.CORE_VERBOSE:
            print whoami()
            print whataremyargs()

        return self.__processAgentDic[agentName]

    def listProcessAgents(self, onlyFullyDefined=False):
        print "\n\n%%%%%%%%%%%%%%% PROCESS AGENT LIST %%%%%%%%%%%%%%%\n\n"
        if runtime.DEBUG_MODE & runtime.CORE_METHOD:
            print whoami()
            print whataremyargs()

        for agentName, agent in self.__processAgentDic.items():
            if onlyFullyDefined :
                if agent.isFullyDefined():
                    print 'AgentName : %s (%s)' % (agentName, id(agent))
                    print agent
            else:
                print 'AgentName : %s (%s)' % (agentName, id(agent))
                print agent


    #TODO
    # *add a control that the agent does not exist yet
    def addFileAgent(self, newAgent):
        if runtime.DEBUG_MODE & runtime.CORE_METHOD:
            print whoami()
            print "%s"%whataremyargs(), " newAgentName : ",newAgent.agentName

        if newAgent.agentName in self.__metaAgentDic.keys():
          raise Exception("Agent %s already exists"%newAgent.agentName)

        self.currentFileAgent = newAgent
        self.currentMetaAgent = newAgent
        self.__fileAgentDic[newAgent.agentName] = newAgent
        self.__metaAgentDic[newAgent.agentName] = newAgent
        newAgent.idAgent = self.__idAgent
        self.__idAgent += 1
        newAgent.idFile = self.__idFile
        self.__idFile += 1



    def getFileAgent(self, agentName):
        if runtime.DEBUG_MODE & runtime.CORE_VERBOSE:
            print whoami()
            print whataremyargs()

        return self.__fileAgentDic[agentName]

    def listFileAgents(self, onlyFullyDefined=False):
        print "\n\n%%%%%%%%%%%%%%% FILE AGENT LIST %%%%%%%%%%%%%%%\n\n"
        if runtime.DEBUG_MODE & runtime.CORE_METHOD:
            print whoami()
            print whataremyargs()

        for agentName, agent in self.__fileAgentDic.items():
            if onlyFullyDefined :
                if agent.isFullyDefined():
                    print 'AgentName : %s (%s)' % (agentName, id(agent))
                    print agent
            else:
                print 'AgentName : %s (%s)' % (agentName, id(agent))
                print agent




    #TODO
    # *add a control that the agent does not exist yet
    def addFolderAgent(self, newAgent):
        if runtime.DEBUG_MODE & runtime.CORE_METHOD:
            print whoami()
            print whataremyargs()

        if newAgent.agentName in self.__metaAgentDic.keys():
          raise Exception("Agent %s already exists"%newAgent.agentName)

        self.currentFolderAgent = newAgent
        self.currentMetaAgent = newAgent
        self.__folderAgentDic[newAgent.agentName] = newAgent
        self.__metaAgentDic[newAgent.agentName] = newAgent
        newAgent.idAgent = self.__idAgent
        self.__idAgent += 1
        newAgent.idFile = self.__idFile
        self.__idFile += 1


    def getFolderAgent(self, agentName):
        if runtime.DEBUG_MODE & runtime.CORE_verbose:
            print whoami()
            print whataremyargs()

        return self.__folderAgentDic[agentName]

    def listFolderAgents(self, onlyFullyDefined=False):
        print "\n\n%%%%%%%%%%%%%%% FOLDER AGENT LIST %%%%%%%%%%%%%%%\n\n"
        if runtime.DEBUG_MODE & runtime.CORE_METHOD:
            print whoami()
            print whataremyargs()

        for agentName, agent in self.__folderAgentDic.items():
            if onlyFullyDefined :
                if agent.isFullyDefined():
                    print 'AgentName : %s (%s)' % (agentName, id(agent))
                    print agent
            else:
                print 'AgentName : %s (%s)' % (agentName, id(agent))
                print agent



    #TODO
    # *add a control that the agent does not exist yet
    def addEnvironmentAgent(self, newAgent):
        if runtime.DEBUG_MODE & runtime.CORE_METHOD:
            print whoami()
            print whataremyargs()


        if newAgent.agentName in self.__metaAgentDic.keys():
          raise Exception("Agent %s already exists"%newAgent.agentName)

        self.currentEnvironmentAgent = newAgent
        self.__environmentAgentDic[newAgent.agentName] = newAgent
        self.__metaAgentDic[newAgent.agentName] = newAgent
        newAgent.idAgent = self.__idAgent
        self.__idAgent += 1
        newAgent.idEnvironment = self.__idEnvironment
        self.__idEnvironment += 1

    def getEnvironmentAgent(self, agentName):
        if runtime.DEBUG_MODE & runtime.CORE_VERBOSE:
            print whoami()
            print whataremyargs()

        return self.__environmentAgentDic[agentName]

    def listEnvironmentAgents(self, onlyFullyDefined=False):
        print "\n\n%%%%%%%%%%%%%%% ENVIRONMENT AGENT LIST %%%%%%%%%%%%%%%\n\n"

        if runtime.DEBUG_MODE & runtime.CORE_METHOD:
            print whoami()
            print whataremyargs()

        for agentName, agent in self.__environmentAgentDic.items():
            if onlyFullyDefined :
                if agent.isFullyDefined():
                    print 'AgentName : %s (%s)' % (agentName, id(agent))
                    print agent
            else:
                print 'AgentName : %s (%s)' % (agentName, id(agent))
                print agent




    #TODO
    # *add a control that the agent does not exist yet
    def addCoordinator(self, newAgent, isCurrent=True):
        if runtime.DEBUG_MODE & runtime.CORE_METHOD:
            print whoami()
            print whataremyargs(), "newAgentName : ",newAgent.agentName
            

        if newAgent.agentName in self.__metaAgentDic.keys():
          raise Exception("Coordinator %s already exists"%newAgent.agentName)

        self.__coordinatorDic[newAgent.agentName] = newAgent
        self.__metaAgentDic[newAgent.agentName] = newAgent
        newAgent.idAgent = self.__idAgent
        self.__idAgent += 1


        for fa in newAgent.getFileAgentList():
            if fa.agentName in self.__metaAgentDic.keys():
              raise Exception("Agent %s already exists"%fa.agentName)
            #self.addFileAgent(pa)
            self.__fileAgentDic[fa.agentName] = fa
            self.__metaAgentDic[fa.agentName] = fa
            print "Add %s to metaAgentDic"%fa.agentName
            fa.idAgent = self.__idAgent
            self.__idAgent += 1
            fa.idFile = self.__idFile
            self.__idFile += 1 

        for fa in newAgent.getFolderAgentList():
            if fa.agentName in self.__metaAgentDic.keys():
              raise Exception("Agent %s already exists"%fa.agentName)
            #self.addFileAgent(pa)
            print "Add %s to metaAgentDic"%fa.agentName
            self.__folderAgentDic[fa.agentName] = fa
            self.__metaAgentDic[fa.agentName] = fa
            fa.idAgent = self.__idAgent
            self.__idAgent += 1
            fa.idFile = self.__idFile
            self.__idFile += 1


        for pa in newAgent.getProcessAgentList():
            if pa.agentName in self.__metaAgentDic.keys():
              raise Exception("Agent %s already exists"%pa.agentName)
            print "Add %s to metaAgentDic"%pa.agentName
            self.__processAgentDic[pa.agentName] = pa
            self.__metaAgentDic[pa.agentName] = pa
            self.__processAgentList.append(pa)
            pa.idAgent = self.__idAgent
            self.__idAgent += 1
            pa.idProcess = self.__idProcess
            self.__idProcess += 1
            print "CORE : I add %s (id : %s , pid: %s)"%(pa.agentName, pa.idAgent, pa.idProcess)

	    self.__fileAgentDic[pa.envVarFile.agentName] = pa.envVarFile
            self.__metaAgentDic[pa.envVarFile.agentName] = pa.envVarFile
            pa.envVarFile.idAgent = self.__idAgent
            self.__idAgent += 1
            pa.envVarFile.idFile = self.__idFile
            self.__idFile += 1
            print "CORE : I add %s (id : %s , pid: %s)"%(pa.envVarFile.agentName, pa.envVarFile.idAgent, pa.envVarFile.idFile)
            


            if pa.getChildProcessAgent():
                if pa.getChildProcessAgent().agentName in self.__metaAgentDic.keys():
                  raise Exception("Agent %s already exists"%pa.getChildProcessAgent().agentName)
                print "Add %s to metaAgentDic"%pa.getChildProcessAgent().agentName
                self.__processAgentDic[pa.getChildProcessAgent().agentName] = pa.getChildProcessAgent()
                self.__metaAgentDic[pa.getChildProcessAgent().agentName] = pa.getChildProcessAgent()
                self.__processAgentList.append(pa.getChildProcessAgent())
                pa.getChildProcessAgent().idAgent = self.__idAgent
                self.__idAgent += 1
                pa.getChildProcessAgent().idProcess = self.__idProcess
                self.__idProcess += 1

    	        self.__fileAgentDic[pa.getChildProcessAgent().envVarFile.agentName] = pa.getChildProcessAgent().envVarFile
                self.__metaAgentDic[pa.getChildProcessAgent().envVarFile.agentName] = pa.getChildProcessAgent().envVarFile
                pa.envVarFile.idAgent = self.__idAgent
                self.__idAgent += 1
                pa.getChildProcessAgent().envVarFile.idFile = self.__idFile
                self.__idFile += 1
                print "CORE : I add %s (id : %s , pid: %s)"%(pa.getChildProcessAgent().envVarFile.agentName, pa.getChildProcessAgent().envVarFile.idAgent, pa.getChildProcessAgent().envVarFile.idFile)

            #self.addProcessAgent(pa)

        for co in newAgent.getCoordinatorList():
            self.addCoordinator(co, False)

        if isCurrent:
            self.currentCoordinator = newAgent



    def getCoordinator(self, agentName):
        if runtime.DEBUG_MODE & runtime.CORE_VERBOSE:
            print whoami()
            print whataremyargs()

        return self.__coordinatorDic[agentName]

    def listCoordinators(self, onlyFullyDefined=False):
        print "\n\n%%%%%%%%%%%%%%% COORDINATOR AGENT LIST %%%%%%%%%%%%%%%\n\n"
        if runtime.DEBUG_MODE & runtime.CORE_METHOD:
            print whoami()
            print whataremyargs()

        for agentName, agent in self.__coordinatorDic.items():
            if onlyFullyDefined :
                if agent.isFullyDefined():
                    print 'AgentName : %s (%s)' % (agentName, id(agent))
                    print agent
            else:
                print 'AgentName : %s (%s)' % (agentName, id(agent))
                print agent

    def addServer(self, newServer):
        if runtime.DEBUG_MODE & runtime.CORE_METHOD:
            print whoami()
            print whataremyargs()

        if newServer.agentName in self.__serverDic.keys():
            raise Exception("Server %s already exists"%newServer.agentName)

        self.currentServer = newServer
        self.__serverDic[newServer.agentName] = newServer
        newServer.idServer = self.__idServer
        self.__idServer += 1
        
        envAg = newServer.getEnvironmentAgent()
        
        if envAg:
            #print "I ADD ENV"
            self.addEnvironmentAgent(envAg)
            fstab = envAg.getFstab()
            if fstab:
                #print "I ADD FSTAB"
                self.addFileAgent(fstab)
        
    def getServer(self, agentName):
        if runtime.DEBUG_MODE & runtime.CORE_VERBOSE:
            print whoami()
            print whataremyargs()

        return self.__serverDic[agentName]

    def listServers(self, onlyFullyDefined=False):
        print "\n\n%%%%%%%%%%%%%%% SERVER LIST %%%%%%%%%%%%%%%\n\n"
        if runtime.DEBUG_MODE & runtime.CORE_METHOD:
            print whoami()
            print whataremyargs()

        for agentName, agent in self.__serverDic.items():
            if onlyFullyDefined :
                if agent.isFullyDefined():
                    print 'AgentName : %s (%s)' % (agentName, id(agent))
                    print agent
            else:
                print 'AgentName : %s (%s)' % (agentName, id(agent))
                print agent
  


    #TODO : 
    # * be more generic : use a list of dic or so
    def getEntity(self, entityName):
        if runtime.DEBUG_MODE & runtime.CORE_VERBOSE:
            print whoami()
            print whataremyargs()

        if entityName in self.__processAgentDic:
            return self.__processAgentDic[entityName]

        if entityName in self.__fileAgentDic:
            return self.__fileAgentDic[entityName]

        if entityName in self.__folderAgentDic:
            return self.__folderAgentDic[entityName]

        if entityName in self.__coordinatorDic:
            return self.__coordinatorDic[entityName]

        raise Exception("%s is not know anywhere" % (entityName))    

    def getNextTotal(self):
        if runtime.DEBUG_MODE & runtime.CORE_VERBOSE:
            print whoami()

        self.__total += 1   
        return self.__total 

    def getNextOccurrence(self):
        if runtime.DEBUG_MODE & runtime.CORE_VERBOSE:
            print whoami()

        self.__occurrence += 1   
        return self.__occurrence 

    def getNextLink(self):
        if runtime.DEBUG_MODE & runtime.CORE_VERBOSE:
            print whoami()

        self.__idLink += 1
        return self.__idLink

    def getNextDependencyRuleId(self):
        if runtime.DEBUG_MODE & runtime.CORE_VERBOSE:
            print whoami()

        self.__idDependencyRule += 1
        return self.__idDependencyRule

    def listFullyDefinitionStatus(self):
        if runtime.DEBUG_MODE & runtime.CORE_METHOD:
            print whoami()
            print whataremyargs()

        for agentName, agent in self.__coordinatorDic.items():
            if agent.isFullyDefined():
                print "%s : YES" % agentName
            else:
                print "%s : NO" % agentName
        for agentName, agent in self.__processAgentDic.items():
            if agent.isFullyDefined():
                print "%s : YES" % agentName
            else:
                print "%s : NO" % agentName

        for agentName, agent in self.__fileAgentDic.items():
            if agent.isFullyDefined():
                print "%s : YES" % agentName
            else:
                print "%s : NO" % agentName
               

        for agentName, agent in self.__folderAgentDic.items():
            if agent.isFullyDefined():
                print "%s : YES" % agentName
            else:
                print "%s : NO" % agentName
 
        for agentName, agent in self.__serverDic.items():
            if agent.isFullyDefined():
                print "%s : YES" % agentName
            else:
                print "%s : NO" % agentName
                
    def deleteEntity(self, entityName):
        if runtime.DEBUG_MODE & runtime.CORE_METHOD:
            print whoami()
            print whataremyargs()

        print "I HAVE TO DELETE SOMETHING |%s|"%entityName                
        if ( entityName in self.__metaAgentDic):
            del self.__metaAgentDic[entityName]
        else:
            print "There is no entity called %s to delete"%entityName
            
        if ( entityName in self.__processAgentDic):
           print "It's a proc"
           procObj = self.__processAgentDic[entityName]
           del self.__processAgentDic[entityName]
           self.__processAgentList.remove(procObj)
           if (procObj.getChildProcessAgent()):
               self.deleteEntity(procObj.getChildProcessAgent().agentName)

        if ( entityName in self.__fileAgentDic ):
           print "It's a file"
           del self.__fileAgentDic[entityName]
        if ( entityName in self.__folderAgentDic ):
           print "It's a folder"
           del self.__folderAgentDic[entityName]
        
        
         # I don't think I ever will delete an environmentAgent, but just in case
         # I leave this here not to forget it
         #self.__environmentAgentDic = None
        if ( entityName in self.__coordinatorDic):
           print "It's a coord"
           coordObj = self.__coordinatorDic[entityName]
           del self.__coordinatorDic[entityName]            
           for child in coordObj.getFileAgentList():
               self.deleteEntity(child.agentName)
           for child in coordObj.getProcessAgentList():
               self.deleteEntity(child.agentName)
           for child in coordObj.getFolderAgentList():
                self.deleteEntity(child.agentName)
           for child in coordObj.getCoordinatorList():
                self.deleteEntity(child.agentName)



    def setClassificationOld(self):
        if runtime.DEBUG_MODE & runtime.CORE_METHOD:
            print whoami()

        children = set([])
        allCoordsName = set(self.__coordinatorDic.keys())
    
    
        for coord in self.__coordinatorDic.values():
            childrenCoord = coord.getCoordinatorList()
            for child in childrenCoord:
                children.add(child.agentName)
    
    
        rootCoordinatorNames = allCoordsName - children
        
        print "All coords %s" % allCoordsName
        print  "children %s" % children
        print "The root coordinators are %s" % rootCoordinatorNames
        rootCoordinators = []
        for name in rootCoordinatorNames:
            coord = self.__coordinatorDic[name]
            if coord.isFullyDefined():
                print "%s is fully defined so it is a real root" % name
                rootCoordinators.append(self.__coordinatorDic[name])
    
        ltrs = string.uppercase
        letters = [''.join(a) for a in ltrs]
        letters += [''.join([a, b]) for a in ltrs for b in ltrs]
        letters += [''.join([a, b,c ]) for a in ltrs for b in ltrs for c in ltrs]
 
    
        curLetterId = 0
        curDepth = 0
        for root in rootCoordinators:
            curLetter = letters[curLetterId]
            curDepth = 0
    
    
            parents = [root]
            while len(parents):
                toDo = set([])
    
                classif = "%s-%s" % (curLetter, curDepth)
                for p in parents:
                    p.setClassification(classif)
                    for child in p.getCoordinatorList():
                        toDo.add(child)
   
                #parents = []
                parents = copy.copy(toDo)
                toDo = []
                curDepth += 1
    
            curLetterId += 1




    def setClassification(self):
        if runtime.DEBUG_MODE & runtime.CORE_METHOD:
            print whoami()

        children = set([])
        allCoordsName = set(self.__coordinatorDic.keys())
    
    
        for coord in self.__coordinatorDic.values():
            childrenCoord = coord.getCoordinatorList()
            for child in childrenCoord:
                children.add(child.agentName)
    
    
        rootCoordinatorNames = allCoordsName - children
        
        print "All coords %s" % allCoordsName
        print  "children %s" % children
        print "The root coordinators are %s" % rootCoordinatorNames
        rootCoordinators = []
        for name in rootCoordinatorNames:
            coord = self.__coordinatorDic[name]
            if coord.isFullyDefined():
                print "%s is fully defined so it is a real root" % name
                rootCoordinators.append(self.__coordinatorDic[name])
    
    
        curLetter = 0
        for root in rootCoordinators:
            root.setClassification("%s"%curLetter)
            curLetter += 1











    def deleteUselessEnvVarFiles(self):
        if runtime.DEBUG_MODE & runtime.CORE_METHOD:
            print whoami()

        for agentName, agent in self.__processAgentDic.items():
            if agent.envVarFileUsed == False:
                print "%s does not use it's envVarFile %s"%(agentName, agent.envVarFile.agentName)
                envFile = agent.envVarFile
                agent.envVarFile = None
                agent.envVarFileUsed = False
                self.deleteEntity(envFile.agentName)
 
#            print whoami()
#            print whataremyargs()
#        
#        if server.isFullyDefined():
#    
#            import coordinatorParser        
#            coordinatorParser.parseAction_coordinator_start(('Coordinator', coord.agentName + "_on_" + server.agentName, coord.agentName))
#            newCoord = self.__coordinatorDic[coord.agentName + "_on_" + server.agentName]
#            newCoord.addAttribute('server', server.agentName)
#            server.addAttachedCoordinator(newCoord)


        #for child in server.getChildrenAgents().values():
        #    self.attachCoordToServer(child, coord)


    def assignServerAttachments(self):
        if runtime.DEBUG_MODE & runtime.CORE_METHOD:
            print whoami()

        for server in self.__serverDic.values():
            print "looking at %s"%server.agentName
            for coordName in server.attachedCoordNames:
                print "%s wants to have %s assigned"%(server.agentName, coordName)
                try:
                    coord = self.__coordinatorDic[coordName]
                except Exception as inst:
                    print "%s"%self.__coordinatorDic.keys()
                    print "(%s) The coordinator called '%s' attached to '%s' cannot be found"%(type(inst), coordName, server.agentName)
                    sys.exit(1)
                #self.attachCoordToServer(server, coord)
                if server.isFullyDefined():
            
                    import coordinatorParser        
                    coordinatorParser.parseAction_coordinator_start(('Coordinator', coord.agentName + "_on_" + server.agentName, coord.agentName))
                    newCoord = self.__coordinatorDic[coord.agentName + "_on_" + server.agentName]
                    newCoord.addAttribute('server', server.agentName)
                    server.addAttachedCoordinator(newCoord)
#                import coordinatorParser        
#                coordinatorParser.parseAction_coordinator_start(('Coordinator', coord.agentName + "_on_" + server.agentName, coordName))
#                newCoord = self.__coordinatorDic[coord.agentName + "_on_" + server.agentName]
#                newCoord.addAttribute('server', server.agentName)
#                server.addAttachedCoordinator(newCoord)
                


    
    def compareWithDb(self):
        if runtime.DEBUG_MODE & runtime.CORE_METHOD:
            print whoami()
            print whataremyargs()

        self.links = []
        for agentName, agent in self.__coordinatorDic.items():
            assignmentDic = agent.getNeedsAssignmentDic()
            for need, assignedAgent in assignmentDic.items():
                occId = agent.getNeedsOccurrence(need)
                self.links.append((self.getNextLink(), occId, agentName, assignedAgent.agentName))



            if not agent.isFullyDefined():
                nonAssigned = agent.getNonAssignedNeeds()
                for ns in nonAssigned:
                    occId = agent.getNeedsOccurrence(ns)
                    import coordinatorParser
                    nouv = coordinatorParser.Coordinator(ns) #it's not necessarily a coordinator, but who cares, sicne they are never used
                    try:
                        nouv.total = agent.getTotalAssignedForNeed(ns)
                    except: #it might fail since the total only goes with a Coordinator need
                        nouv.total = -1
                    nouv.setIsFullyDefined(False)
                    agent.assignNeeds(ns,nouv)
                    self.addCoordinator(nouv,False)
                    self.links.append((self.getNextLink(),occId, agentName, ns))

        self.dependencyRules = [] 
        for agentName, agent in self.__coordinatorDic.items():
            if not agent.isFullyDefined():
                continue
            
            
            requirements = agent.getRequirements()
            agentId = self.__metaAgentDic[agentName].idAgent
            for req in requirements:
                #reqId = self.__metaAgentDic[req.agentName].idAgent
                reqId = self.__metaAgentDic[req].idAgent
                self.dependencyRules.append([self.getNextDependencyRuleId(), reqId, agentId])


        self.tc = TreeComparison.TreeComparison()
        if runtime.compareWithDb:
            self.tc.compare(self.__metaAgentDic, self.links, self.__serverDic, self.dependencyRules)
        else:
            self.tc.fakeDics(self.__metaAgentDic, self.links, self.__serverDic, self.dependencyRules)

        
        


    

#"select a.type, case when b.id then b.id when c.id then c.id when d.id then d.id else NULL END from MetaAgent a  left join FileAgent b on a.id = b.id_metaAgent  left join ProcessAgent c on a.id = c.id_metaAgent left join EnvironmentAgent d on a.id = d.id_metaAgent where a.id =26;"
          
    def exportToSql(self, fileName, recreateDbPath=None):
        if runtime.DEBUG_MODE & runtime.CORE_METHOD:
            print whoami()
            print whataremyargs()

        
        serverIds = {}
        metaAgentIds = {}
        #coordIds = {}
        fileAgentIds = {}
        processAgentIds = {}
        environmentAgentIds = {}
        
        sqlRequest = ""
        
        #path to the phronesisAnalyzer.sql script
        #if recreateDbPath:
            #sqlRequest += "Drop database  IF EXISTS phronesisAnalyzer;\n"
            #sqlRequest += "create database phronesisAnalyzer;\n"
            #sqlRequest += "use phronesisAnalyzer;\n"
            #sqlRequest += "source %s;\n" % recreateDbPath



        if not runtime.compareWithDb:
            sqlRequest += "Drop database  IF EXISTS phronesisAnalyzer;\n"
            sqlRequest += "create database phronesisAnalyzer;\n"
            sqlRequest += "use phronesisAnalyzer;\n"
            sqlRequest += "source %s;\n" % recreateDbPath

        else:            
            sqlRequest += "use phronesisAnalyzer;\n"
            sqlRequest += "\n\n"
            sqlRequest += "Delete from FileContentRules;\n"
            sqlRequest += "Delete from FilenameFilter;\n"
            sqlRequest += "Delete from ProcessLimits;\n"
            sqlRequest += "Delete from RecoveryRule;\n"
            sqlRequest += "Delete from IgnoredFS;\n"
            sqlRequest += "Delete from Veto;\n"
            sqlRequest += "\n\n"


        sqlRequest += "\n\n"
        for occToDel in self.tc.removedLinks:
            sqlRequest += "delete from MetaAgentTree where id = %s;\n"%occToDel.localId

        sqlRequest += "\n\n"
        for depRuleToDel in self.tc.removedDependencyRules:
            sqlRequest += "delete from Dependency where id = %s;\n"%depRuleToDel[0]

        for agentToDel in self.tc.removedAgentIds:
            dbA = self.tc.dbAgents[self.tc.metaADic[agentToDel]]
            try:
                idSub = dbA.idProcess
                sqlRequest += "delete from ProcessAgent where id = %s;\n"%idSub
            except:
                try:
                    idSub = dbA.idEnvironment
                    sqlRequest += "delete from EnvironmentAgent where id = %s;\n"%idSub
                except:
                    try:
                        idSub = dbA.idFile
                        sqlRequest += "delete from FileAgent where id = %s;\n"%idSub
                    except:
                        pass
            
            sqlRequest += "delete from MetaAgent where id = %s;\n"%agentToDel
        
        sqlRequest += "\n\n"
        for totToDel in self.tc.removedTot:
            if totToDel:
                sqlRequest += "delete from Total where id = %s;\n"%totToDel


        sqlRequest += "\n\n"
        for occToDel in self.tc.removedOccs:
            sqlRequest += "delete from Occurrence where id = %s;\n"%occToDel.occId

        sqlRequest += "\n\n"
        for srvToDel in self.tc.removedServers:
            sqlRequest += "delete from Server where id = %s;\n"%srvToDel
        

        sqlRequest += "\n\n"
        for agentName, agent in self.__serverDic.items():
            realSrvId = self.tc.serverEquivalences[agent.idServer]
            serverIds[agentName] = realSrvId
            if realSrvId in self.tc.newServers:
                sqlRequest += "insert into Server(id, name, address) values (%s, '%s', '%s');\n" % (realSrvId, agentName, agent.address)
            else:
                sqlRequest += "update Server set address = '%s' where id = %s;\n"%(agent.address, realSrvId)

        sqlRequest += "\n\n"
        

        alreadyInsertedNewTotal = [] 
        for agentName, agent in self.__coordinatorDic.items():
            realMaId = self.tc.agentIdEquivalences[agent.idAgent] 
            metaAgentIds[agentName] = realMaId
            if realMaId in self.tc.newAgentIds:
                realTotId = self.tc.totEquivalences[agent.total]
                if (realTotId in self.tc.newTot) and (realTotId not in alreadyInsertedNewTotal):
                    sqlRequest += "insert into Total (id) value (%s);\n"%realTotId
                    alreadyInsertedNewTotal.append(realTotId)
                sqlRequest += "insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (%s,'%s', 'Coordinator', %s, '%s',%s, %s);\n" % (realMaId, agentName, self.tc.totEquivalences[agent.total], agent.classification,agent.isFullyDefined(), agent.tolerate)
            else:
                sqlRequest += "update MetaAgent set classification = '%s' ,valid = %s, tolerate = %s where id = %s;\n" % ( agent.classification,agent.isFullyDefined(), agent.tolerate, realMaId)
            
            for veto in agent.veto:
                sqlRequest += "insert into Veto (idCoord, name) value (%s, '%s');\n"%(realMaId, veto)
          

            
        sqlRequest += "\n\n"
        


        for srvName, srv in self.__serverDic.items():
            realSrvId = serverIds[srvName] 
            print "%s %s"%(srvName, srv.attachedCoords)
            for coord in srv.attachedCoords:
                realCoordId = metaAgentIds[coord.agentName]

                #sqlRequest += "insert into ServerAttachedCoord (id_server, id_metaAgent) values (%s,%s);\n"%(realSrvId, realCoordId)
                sqlRequest += "Update MetaAgent set id_server = %s where id = %s;\n"%(realSrvId, realCoordId)


        sqlRequest += "\n\n"



         
        for agentName, agent in self.__fileAgentDic.items():
            realMaId = self.tc.agentIdEquivalences[agent.idAgent]
            metaAgentIds[agentName] = realMaId
            if agent.isFullyDefined() and self.getServer(agent.server).isFullyDefined():

                idSrv = serverIds[agent.server]

                if realMaId in self.tc.newAgentIds:
                    realFaId = self.tc.getNextFile()
                    sqlRequest += "insert into MetaAgent (id, name, type, id_server, valid) values (%s,'%s', 'FileAgent', %s, %s);\n" % (realMaId, agentName, idSrv,agent.isFullyDefined())
                    sqlRequest += "insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (%s, %s, '%s', '%s', '%s', '%s', '%s', '%s');\n" % (realFaId, realMaId, agent.filename, agent.owner, agent.grp, agent.permissions, agent.attributes, agent.md5)

                else:
                    realFaId = self.tc.dbAgents[agentName].idFile 
                    sqlRequest += "update MetaAgent set id_server = %s where id = %s;\n"%(idSrv, realMaId)
                    sqlRequest += "update FileAgent set filename = '%s', owner = '%s', grp = '%s', permissions = '%s', attributes = '%s', md5 = '%s' where id =%s;\n" % (agent.filename, agent.owner, agent.grp, agent.permissions, agent.attributes, agent.md5, realFaId)
                    
                for content in agent.contents:
                    sqlRequest += "insert into FileContentRules (id_file, rule) values (%s, '%s');\n" % (realFaId, content)

                fileAgentIds[agentName] = realFaId

	    else:
                if realMaId in self.tc.newAgentIds:
		    sqlRequest += "insert into MetaAgent (id, name, type, valid) values (%s,'%s', 'FileAgent',%s);\n" % (realMaId, agentName,False)                
 

        sqlRequest += "\n\n"
                 
        for agentName, agent in self.__folderAgentDic.items():
            realMaId = self.tc.agentIdEquivalences[agent.idAgent]
            metaAgentIds[agentName] = realMaId
            if agent.isFullyDefined() and self.getServer(agent.server).isFullyDefined():

                idSrv = serverIds[agent.server]

                if realMaId in self.tc.newAgentIds:
                    realFaId = self.tc.getNextFile()
                    sqlRequest += "insert into MetaAgent (id, name, type, id_server, valid) values (%s,'%s', 'FolderAgent', %s, %s);\n" % (realMaId, agentName, idSrv,agent.isFullyDefined())
                    sqlRequest += "insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5, maxDepth) values (%s, %s, '%s', '%s', '%s', '%s', '%s', '%s', %s);\n" % (realFaId, realMaId, agent.filename, agent.owner, agent.grp, agent.permissions, agent.attributes, agent.md5, agent.maxDepth)

                else:
                    realFaId = self.tc.dbAgents[agentName].idFile 
                    sqlRequest += "update MetaAgent set id_server = %s where id = %s;\n"%(idSrv, realMaId)
                    sqlRequest += "update FileAgent set filename = '%s', owner = '%s', grp = '%s', permissions = '%s', attributes = '%s', md5 = '%s' where id =%s;\n" % (agent.filename, agent.owner, agent.grp, agent.permissions, agent.attributes, agent.md5, realFaId)
                
                for content in agent.contents:
                    sqlRequest += "insert into FileContentRules (id_file, rule) values (%s, '%s');\n" % (realFaId, content)

                for nameFilter in agent.filenameFilters:
                    sqlRequest += "insert into FilenameFilter (id_folder, filter) values (%s, '%s');\n" % (realFaId, nameFilter)

                fileAgentIds[agentName] = realFaId

	    else:
                if realMaId in self.tc.newAgentIds:
		    sqlRequest += "insert into MetaAgent (id, name, type, valid) values (%s,'%s', 'FolderAgent',%s);\n" % (realMaId, agentName,False)                
 
        sqlRequest += "\n\n"
                 
        curProcessLimitId = 1         
        for agent in self.__processAgentList:
            agentName = agent.agentName
            print "AgentName : %s"%agentName
            realMaId = self.tc.agentIdEquivalences[agent.idAgent]
            metaAgentIds[agentName] = realMaId
            if agent.isFullyDefined() and self.getServer(agent.server).isFullyDefined():

                idSrv = serverIds[agent.server]

                if realMaId in self.tc.newAgentIds:
                    realPaId = self.tc.getNextProcess()

                    sqlRequest += "insert into MetaAgent (id, name, type, id_server, valid) values (%s,'%s', 'ProcessAgent', %s, %s);\n" % (realMaId, agentName, idSrv, True)
                    sqlRequest += "insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (%s, %s, '%s', '%s', %s, '%s', '%s', %s, %s);\n" % (realPaId, realMaId, agent.procName, agent.user,agent.multiplicity, agent.service, agent.command, agent.maxCpu, agent.maxMemory)
                else:
                    realPaId = self.tc.dbAgents[agentName].idProcess 
                    sqlRequest += "update MetaAgent set id_server = %s where id = %s;\n"%(idSrv, realMaId)
                    sqlRequest += "update ProcessAgent set procName = '%s', user = '%s', multiplicity = %s, service = '%s', command = '%s', maxCpu = %s, maxMemory = %s where id = %s;\n" % (agent.procName, agent.user, agent.multiplicity, agent.service, agent.command, agent.maxCpu, agent.maxMemory, realPaId)
                    

                processAgentIds[agentName] = realPaId

                if agent.getParentProcessAgent() != None:
                    sqlRequest += "update ProcessAgent set id_parent = %s where id = %s;\n" % (processAgentIds[agent.getParentProcessAgent().agentName], realPaId)
                    
                if agent.envVarFileUsed == True:
                    sqlRequest += "update ProcessAgent set id_procEnvFile = %s where id = %s;\n" % (fileAgentIds[agent.envVarFile.agentName], realPaId)


                for idLimit, valLimit in agent.getLimits().items():
                    sqlRequest += "insert into ProcessLimits(id, id_proc, id_limit, value) values (%s, %s, %s, %s);\n" % (curProcessLimitId, realPaId, idLimit, valLimit)
                    curProcessLimitId += 1

	    else:
                if realMaId in self.tc.newAgentIds:
		    sqlRequest += "insert into MetaAgent (id, name, type, valid) values (%s,'%s', 'ProcessAgent',%s);\n" % (realMaId, agentName,False)                

        sqlRequest += "\n\n"

        for agentName, agent in self.__environmentAgentDic.items():
            realMaId = self.tc.agentIdEquivalences[agent.idAgent]
            if agent.isFullyDefined() and self.getServer(agent.server).isFullyDefined():

                idSrv = serverIds[agent.server]

                if realMaId in self.tc.newAgentIds:
                    realEaId = self.tc.getNextEnvironment()
                    sqlRequest += "insert into MetaAgent (id, name, type, id_server, valid) values (%s,'%s', 'EnvironmentAgent', %s, %s);\n" % (realMaId, agentName, idSrv, agent.isFullyDefined())
                    sqlRequest += "insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (%s, %s, %s, %s, %s, %s);\n" % (realEaId, realMaId, fileAgentIds[agent.getFstab().agentName], agent.maxLoad, agent.maxMemory, agent.maxSwap)

                else:
                    realEaId = self.tc.dbAgents[agentName].idEnvironment 
                    sqlRequest += "update MetaAgent set id_server = %s where id = %s;\n"%(idSrv, realMaId)
                    sqlRequest += "update EnvironmentAgent set maxLoad = %s, maxMemory = %s, maxSwap = %s where id =%s;\n" % (agent.maxLoad, agent.maxMemory, agent.maxSwap, realEaId)
                    
                for fs in agent.ignoredFS:
                    sqlRequest += "insert into IgnoredFS (id_envAgent, fsName) values (%s, '%s');\n"%(realEaId, fs)


                environmentAgentIds[agentName] = realEaId

	    else:
                if realMaId in self.tc.newAgentIds:
		    sqlRequest += "insert into MetaAgent (id, name, type, valid) values (%s,'%s', 'EnvironmentAgent',%s);\n" % (realMaId, agentName,False)                

        alreadyInsertedNewOcc = []
        sqlRequest += "\n\n" 
        for l in self.links:
            print l
            mtId = l[0]
            occId = l[1]
            startName = l[2]
            endName = l[3]
 
            realMtId = self.tc.linkEquivalences[mtId]
            realOccId = self.tc.occEquivalences[occId]
            if realMtId in self.tc.newLinks:
                if (realOccId in self.tc.newOccs) and (realOccId not in alreadyInsertedNewOcc):
                    sqlRequest += "insert into Occurrence(id) value (%s);\n"%realOccId
                    alreadyInsertedNewOcc.append(realOccId)
                
                sqlRequest += "insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (%s, %s, %s, %s);\n" % (realMtId, metaAgentIds[startName], metaAgentIds[endName], realOccId)
            
        sqlRequest += "\n\n" 
        for l in self.dependencyRules:
            print "DEPENDENCY RULE : %s needs %s"%(l[2], l[1])
            depRuleId = l[0]
            reqId = l[1]
            agId = l[2]
 
            realDepRuleId = self.tc.dependencyRuleEquivalences[depRuleId]
            realReqId = self.tc.agentIdEquivalences[reqId]
            realAgId = self.tc.agentIdEquivalences[agId]
            if realDepRuleId in self.tc.newDependencyRules:
                sqlRequest += "insert into Dependency (id, id_prior, id_after, occurrence, validity) values (%s, %s, %s, %s, %s);\n" % (realDepRuleId, realReqId, realAgId, 0, 1)


        sqlRequest += "\n\n"
        idRecoveryRule = 1
       
        superMap = [self.__fileAgentDic, self.__folderAgentDic, self.__processAgentDic] 
        for subMap in superMap:
            for agentName, agent in subMap.items():
                if not (agent.isFullyDefined() and self.getServer(agent.server).isFullyDefined()):
                    continue
                
                triggers = agent.getTriggers()
                realMaId = metaAgentIds[agentName]
                for trigger in triggers:
                    print "\t%s : %s"%(agentName, trigger)
                    triggerSuffix = trigger.latestSuffix
                    if not trigger.useSuffix : 
                        triggerSuffix = ''
                    ma2Id = metaAgentIds[trigger.secondMa + triggerSuffix]
                    #realMa2Id = self.tc.agentIdEquivalences[ma2Id]
                    realMa2Id = ma2Id
                    sqlRequest += "insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (%s, %s, '%s', %s, '%s', '%s');\n" % (idRecoveryRule, realMaId, trigger.firstAction, realMa2Id, trigger.secondAction, trigger.when)
    
                    idRecoveryRule += 1    

        f = open(fileName, 'w')
        f.write(sqlRequest)
        f.close()

                
    



def recListDir(confDir):
    if runtime.DEBUG_MODE & runtime.TOPO_METHOD:
        print whoami()
        print whataremyargs()

    # 13-09-20 : I added this because of one case, let see if it does not break the rest
    confDir = os.path.abspath(confDir)
    print "ABSCONFDIR %s"%confDir

    fileList = []
    for dirname, dirnames, filenames in os.walk(confDir):
        # print path to all subdirectories first.
        print "subdir"
        for subdirname in dirnames:
            print os.path.join(dirname, subdirname)
            pass
    
        print "filE"
        # print path to all filenames.
        for filename in filenames:
            print os.path.join(dirname, filename)
            fileList.append(os.path.join(dirname, filename))
            #fileList.append( filename)
    
        # Advanced usage:
        # editing the 'dirnames' list will stop os.walk() from recursing into there.
        if '.git' in dirnames:
            # don't go into any .git directories.
            dirnames.remove('.git')
    return fileList


#This returns a list of file ordered
def buildSingleCompiledFile(confDir):
    """Make a topological sort of all the files,
     and put them in proper order into one single file"""
    if runtime.DEBUG_MODE & runtime.TOPO_METHOD:
        print whoami()
        print whataremyargs()

    #dirList = os.listdir(confDir)
    dirList = recListDir(confDir)
    print "The file List %s"%dirList
    print "confDir ",confDir
    importDic = {}

    for fnameExt in dirList:
        print "reading ", fnameExt
        if fnameExt[-4:] != '.phr':
            continue
        if fnameExt not in importDic:
            importDic[fnameExt] = []

        #fileContent = open(os.path.join(confDir, fnameExt)).read()
        fileContent = open(fnameExt).read()
        #imp is a token of size 3. The first element is "import", the second the file name, the third is ".phr"
        for imp in importStatment.searchString(fileContent):
            print "import statement ", imp
            #imported = ''.join([imp[1], imp[2]])
            #imported = os.path.join(confDir,''.join([imp[1], imp[2]]))
            #imported = os.path.join('/'.join(fnameExt.split('/')[:-1]),''.join([imp[1], imp[2]]))
            imported = os.path.abspath(os.path.join('/'.join(fnameExt.split('/')[:-1]),imp[1]))
            print "imported %s"%imported
            #if imported not in importDic:
            if imported not in importDic:
                    importDic[imported] = []
            importDic[imported].append(fnameExt)


    
    sortedListTuple = ts.robust_topological_sort(importDic)


    sortedList = []
    for s in sortedListTuple:
        if len(s) != 1 :
            print "Loop in the configuration :"
            for er in s:
                print "\t" + er
            sys.exit(1)

        sortedList.append(s[0])

    return sortedList

#    fileOut = open(config.filename, "w+")
#
#
#    for f in sortedList:
#        if len(f) != 1 :
#            print "Loop in the configuration :"
#            for er in f:
#                print "\t" + er
#            sys.exit(1)
#        f = f[0]
#        fRead = open(os.path.join(config.confDir, f + ".phr"), "r")
#        data = ""
#        for line in fRead:
#            if ((line[:7] == 'import ') or (line[0] == '#') or (line[:2] == '\t#')):
#                continue
#
#            data += line
#
#        fRead.close()
#        fileOut.write(data)
#
#    fileOut.close()


#buildSingleCompiledFile('./testFiles/confWeb')


class MetaAgent:

    agentName = None
    server = None
    realName = None
    suffix = None

    motherAgent = None
    childrenAgents = None
    variables = None
    isFullyDefinedVal = None
    triggers = None
    ANY_TRIGGER = []
    allowedAttributes = []
    specialAttributes = []

    def __init__(self, agentName):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print whoami()  
            print whataremyargs()

        self.agentName = agentName
        self.realName = agentName
        self.childrenAgents = {} #name : agent
        self.variables = {} # variable name, arg that uses it
        self.isFullyDefinedVal = None
        self.triggers = []

    def addTrigger(self,trigger):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print whoami()
            print whataremyargs()

        triggerToAppend = []
        if trigger.firstAction != 'ANY':
            triggerToAppend.append(trigger)
        else:
            for act in self.ANY_TRIGGER:
                copyTrigger = copy.deepcopy(trigger)
                copyTrigger.firstAction = act
                triggerToAppend.append(copyTrigger)


        for trig in triggerToAppend:
            trig.firstAction = 'RECOVERY_' + trig.firstAction
            self.triggers.append(trig)


    def getTriggers(self):
        if runtime.DEBUG_MODE & runtime.OBJ_VERBOSE:
            print whoami()

        return self.triggers

    def renameTriggers(self, suffix):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print whoami()
            print whataremyargs()

        for t in self.triggers:
            t.rename(suffix)

    def recursiveRename(self, suffix):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print whoami()
            print whataremyargs()
        print "RECURSIVERENAME : |%s| + |%s| becomes |%s|"%(self.realName, self.suffix, suffix) 
        self.suffix = suffix
        self.agentName = self.realName + self.suffix

    #TODO
    # * check if attribute is in __allowedAttribute, and if it matches the type
    # * throw exception if variable name already exists -> NO!! can be used by several field
    def addAttribute(self, attribute, value):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print whoami()
            print whataremyargs()

        if (attribute not in self.allowedAttributes):
            if (attribute not in self.specialAttributes):
                raise Exception("%s is not an allowed attribute "%attribute) 
            else:
                raise Exception("%s is a special attribute, and we should not be proccessing it in here. So yes, it is a bug, not a misconfiguration... "%attribute) 

        if (value[0] == '$'):
            if (value in self.variables): #if the variable name already exists
                self.variables[value].append(attribute)

            else:
                self.variables[value] = [attribute]
        else:
            for k,v in self.variables.items():
              if attribute in v:
                print "%s is used for variable %s in agent %s"%(attribute, k, self.agentName)
                self.variables[k].remove(attribute)
              if len(self.variables[k]) == 0: 
                print "The variable does not have any attribut waiting, I delete"
                del self.variables[k]
              else:
                print "The variable still has attributes waiting : %s"%(self.variables[k])
        

        setattr(self, attribute, value)
        
          

     



    #TODO
    # * catch the exception when the mother does not exist 
    def setMotherAgent(self, mother):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print whoami()
            print whataremyargs()


        #if runtime.DEBUG_MODE & runtime.CORE_METHOD:
        #    print self.agentName, " inherits from ", mother.agentName
        self.motherAgent = mother

        #copy of the mother attributes
        self.variables = copy.deepcopy(mother.variables)
        self.triggers = copy.deepcopy(mother.triggers)
        for arg in self.__dir__():
            setattr(self, arg, getattr(mother, arg))
            #retStr += "%s -> %s;\n"%(arg, getattr(self,arg))


        mother.childrenAgents[self.agentName] = self


    #TODO
    # * throw exception if the variable is not in the __variables dic
    def assignVariable(self, varName, value):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print whoami()
            print whataremyargs()

        print "I assign %s ->%s (value[0] = --%s--)" % (varName, value, value[0])
        print self
        if (not self.variables.has_key(varName)):
            print "%s has no variable called %s" % (self.agentName, varName)
            pass
        else:
            if value[0] == '$':
                #print "I replace %s with %s in %s" % (varName, value, self.agentName)
                self.variables[value] = self.variables[varName]
            for att in self.variables[varName]:
                #make sure we did not modify the variable yet
                if getattr(self, att) == varName:
                    setattr(self, att, value)
                else:
                    pass
            #remove the variable from the list
            del self.variables[varName]
            #print "New format"
            #print self
        for t in self.triggers:
            t.assignVariable(varName, value)

    def setIsFullyDefined(self, value):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print whoami()
            print whataremyargs()

        self.isFullyDefinedVal = value

    def setTriggerSuffix(self, suffix):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print whoami()
            print whataremyargs()

        for t in self.triggers:
            t.setSuffix( suffix )

    def __str__(self):
        retStr = ""
        retStr += "\n( %s + %s )\n"%(self.realName, self.suffix)
        for arg in self.__dir__():
            retStr += "%s -> %s;\n" % (arg, getattr(self, arg))

        if (len(self.variables)):
            retStr += "\nvariables\n"
            for key, values in self.variables.items():
                retStr += "%s -> %s;\n" % (key, values)

        if len(self.triggers):
            retStr += "\ntriggers:\n"
            for trigger in self.triggers:
                retStr += "%s\n" % (trigger,)



        return retStr





agentName = Word('$' + alphanums + '_')
metaAgent_inheritance =  COLUMN + agentName("motherAgent")
metaAgent_start =  agentName("agentName") + Optional(metaAgent_inheritance) + LBRACE
#metaAgent_variableAssignment = "set" + Word('$' + alphanums +'_') + OneOrMore(Word(alphanums + '-' + '/' + '_' + '.')) + SEMI
metaAgent_variableAssignment = "set" + Word('$' + alphanums +'_') +  Word(alphanums + '$' + '/' + '*' + '_' + ' ' + '-' + '.' + '=') + SEMI
metaAgent_attribute = Word(alphanums + '_') + ASSIGN + Word(alphanums + '$' + '/' + '*' + '_' + ' ' + '-' + '.' + '=') + SEMI
#metaAgent_attribute = Word(alphanums) + ASSIGN + Word(alphanums + '$' + '/' + '.' + '_' + '-') + SEMI
metaAgent_trigger = oneOf("before after") + Word(alphanums + '_') + "trigger" + Word(alphanums + '_') + "on" + agentName + SEMI
metaAgent_setTriggerSuffix = "triggerSuffix" + ASSIGN + Word('$' + alphanums + '_') + SEMI
metaAgent_middle = Or([metaAgent_setTriggerSuffix, metaAgent_attribute, metaAgent_variableAssignment, metaAgent_trigger])
metaAgent_end = RBRACE

def parseAction_comment(token):
    pass
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print whoami(), token
    #print "COMMENT : %s"%token

#comment.setParseAction(parseAction_comment)


def parseAction_metaAgent_attribute(token):
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print whoami(), token
    runtime.core.currentMetaAgent.addAttribute(token[0], token[1])

metaAgent_attribute.setParseAction(parseAction_metaAgent_attribute)

def parseAction_metaAgent_setTriggerSuffix(token):
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print whoami(), token
    runtime.core.currentMetaAgent.setTriggerSuffix(token[1])

metaAgent_setTriggerSuffix.setParseAction(parseAction_metaAgent_setTriggerSuffix)

def parseAction_metaAgent_variableAssignment(token):
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print whoami(), token
    runtime.core.currentMetaAgent.assignVariable(token[1], token[2])
metaAgent_variableAssignment.setParseAction(parseAction_metaAgent_variableAssignment)

def parseAction_metaAgent_trigger(token):
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print whoami(), token
    runtime.core.currentMetaAgent.addTrigger(Trigger(token[0], token[1], token[3], token[5]))

metaAgent_trigger.setParseAction(parseAction_metaAgent_trigger)


