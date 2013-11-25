from pyparsing import *
import copy
from utils import *
import utils
import runtime
import sys

coordTest = """

Coordinator abc {
}

Coordinator xyz{

}

Coordinator Site {
        needs Coordinator File = abc;
        needs Coordinator Conf;
}

Coordinator Mother {
        needs Coordinator A;
}

Coordinator assignA{
}
Coordinator Child : Mother{
        A = assignA;
}

Coordinator Site1 : Site {

        Conf = xyz;
}

"""

    

class Coordinator:

    agentName = None
    total = None
    totalSet = None
    classification = None
    tolerate = None

    
    #Test to see if we can make the agentName a bit better than 10k characters ...
    __realName = None
    __suffix = None


    allowedAttributes = ['agentName']


    #name of the need, type looked for
    #this one is emptied when a need is assigned -> NOT YET, BUT WHY DID I WANT THAT??
    __needsDic = None # name, type

    #name of the need, assigned entity
    __needsAssignmentDic = None 


    #This will assign a total ID to the needs.
    #Thus when having inheritance, we can give the proper totalId to the coordinator
    __needsTotal = None

    #the occurrence is a value assigned to a link, so we store it there, so it is conserved through inheritance
    __needsOccurrence = None


    __fileAgentList = None
    __folderAgentList = None
    __processAgentList = None 
    __coordinatorList = None
 
    __motherAgent = None 
    __childrenAgents = None

    __variables = None
    
    __requiredCoordinator = None


    veto = None


    #Boolean used to test whether the Agent is completely defined or not
    #the variable is used as a 'memory'
    __isFullyDefined = None

    def __init__(self, agentName):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        self.agentName = agentName
        self.__realName = agentName
        self.__childrenAgents = {} #name : agent
        self.__variables = {} # variable name, arg that uses it
        self.__needsDic = {}
        self.__needsTotal = {}
        self.__needsOccurrence = {}
        self.__needsAssignmentDic = {}
        self.__fileAgentList = []
        self.__folderAgentList = []
        self.__processAgentList = []
        self.__coordinatorList = []
        self.__isFullyDefined = None
        self.totalSet = False
        self.__classification = ""
        self.__requiredCoordinator = []
        self.tolerate = 0
        self.veto = []

    def __dir__(self):
        return ['agentName', 'total', 'classification', 'tolerate']
    
    def setClassification(self, classification):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        self.classification = classification
        cId = 0
        for c in self.__coordinatorList:
            c.setClassification("%s-%s"%(classification,cId))
            cId += 1
        
    def addRequirement(self, requiredCoord):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        self.__requiredCoordinator.append(requiredCoord)

    def getChildrenAgents(self):
        if runtime.DEBUG_MODE & runtime.OBJ_VERBOSE:
            print utils.whoami()

        return self.__childrenAgents
    
    def setChildrenAgents(self, children):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        self.__childrenAgents = children

    def setTolerate(self, tolerate):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()
        self.tolerate = tolerate


    def __selfRename(self, suffix):
#        print "I rename myself (%s, %s) + %s -> %s" % (self.__realName, self.__suffix, suffix, self.__realName + suffix)
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        self.__suffix = suffix
        self.agentName = self.__realName + suffix


    def getNeedName(self, entity):
      """Return the need name fulfilled by the entity given in parameter"""
      i=0
      for k,v in self.__needsAssignmentDic.items():
        if v == entity:
          return str(i)
        i+=1
          #return k

      raise Exception("I don't find any needs that the entity %s fulfils!"%entity.agentName)
      


    def recursiveRename(self, suffix):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()
            
        for fa in self.__fileAgentList:
            #fa.agentName += suffix
            nn = self.getNeedName(fa)
            fa.recursiveRename(suffix)

        for fa in self.__folderAgentList:
            nn = self.getNeedName(fa)
            fa.recursiveRename(suffix)

        for pa in self.__processAgentList:
            nn = self.getNeedName(pa)
            #pa.agentName += suffix
            pa.recursiveRename(suffix)
        for co in self.__coordinatorList:
            #if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            #    print "I change name of ", co.agentName
            nn = self.getNeedName(co)
            co.__selfRename(suffix)
            co.recursiveRename("_" + nn + suffix)
            
        #experimental and probably not the good solution : 
        # rename also the destination of the triggers
        for fa in self.__fileAgentList:
            fa.renameTriggers(suffix)

        for fa in self.__folderAgentList:
            fa.renameTriggers(suffix)

        for pa in self.__processAgentList:
            pa.renameTriggers(suffix)
        


    #TODO
    # * catch the exception when the mother does not exist 
    def setMotherAgent(self, mother, realName):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()
        
        self.agentName = realName
        self.__realName = realName
        self.totalSet = True
        #if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
        #    print self.agentName, " inherits from ", mother.agentName
        self.__motherAgent = mother
        mother.__childrenAgents[self.agentName] = self


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


        self.recursiveRename('_' + self.agentName)

    #TODO
    # * test if needs does not exist yet

    #total is the total Id we assign if the type is coordinator 

    def addNeeds(self, name, typeOfNeed, occurrence, total=None):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()
            
        self.__needsDic[name] = typeOfNeed
        self.__needsOccurrence[name] = occurrence       
        if total:
            #total should be != None only if we have a coordinator
            self.__needsTotal[name] = total

    #TODO :
    # * test if the need exist
    def assignNeeds(self, needName, needValue):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()
        
        # if there was already a need, writing on top of the needsAssignmentDic is not enough
        # we need to remove also the object in the process/file/folder/coord list
        print "needsAssignmentDic : %s"%self
        if ( needName in self.__needsAssignmentDic ):
            listFromWhichToDel = None
            agentToDel = self.__needsAssignmentDic[needName]
            print "STEP 1 agent to del : --%s--"%agentToDel
            if self.__needsDic[needName] == "Coordinator":
                listFromWhichToDel = self.__coordinatorList
            elif (self.__needsDic[needName] == "ProcessAgent"):
                listFromWhichToDel = self.__processAgentList
            elif self.__needsDic[needName] == "FileAgent":
                listFromWhichToDel = self.__fileAgentList
            elif self.__needsDic[needName] == "FolderAgent":
                listFromWhichToDel = self.__folderAgentList
            
            listFromWhichToDel.remove(agentToDel)

    
        self.__needsAssignmentDic[needName] = needValue
        if self.__needsDic[needName] == "Coordinator":
            self.__coordinatorList.append(needValue)
            if needValue.totalSet == False:
                needValue.total = self.__needsTotal[needName]    
        elif (self.__needsDic[needName] == "ProcessAgent"):
            self.__processAgentList.append(needValue)
        elif self.__needsDic[needName] == "FileAgent":
            self.__fileAgentList.append(needValue)
        elif self.__needsDic[needName] == "FolderAgent":
            self.__folderAgentList.append(needValue)

    def getTotalAssignedForNeed(self, needName):
        if runtime.DEBUG_MODE & runtime.OBJ_VERBOSE:
            print utils.whoami()
            print utils.whataremyargs()

        return self.__needsTotal[needName]

    def getNonAssignedNeeds(self):
        if runtime.DEBUG_MODE & runtime.OBJ_VERBOSE:
            print utils.whoami()

        needList = self.__needsDic.keys()
        needL=set(needList)
        needAssigned = self.__needsAssignmentDic.keys()
        needA = set(needAssigned)
        print "Non assign needs for %s : %s "%(self.agentName, needL - needA )
        return needL - needA


    #TODO
    # * throw exception if the variable is not in the __variables dic --> NO!!!! We can use the variables in the children

    def assignVariable (self, varName, value):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()
        
        #is this first if statement ever true??
        if (self.__variables.has_key(varName)):

            # test to see if we could use : "requires $var"
            # Conclusion : does not work yet because what is append to __requiredCoordinator
            # are object and not text
            #if varName in self.__requiredCoordinator:
            #    del self.__requiredCoordinator[varName]
            #    self.__requiredCoordinator.append(value)
                
            for att in self.__variables[varName]: 
                #make sure we did not modify the variable yet
                if getattr(self, att) == varName:
                    setattr(self, att, value)
                else:
                    pass 
            #remove the variable from the list
            del self.__variables[varName]

        for fa in self.__fileAgentList:
            #print "Coord forward (%s,%s) to %s" % (varName, value, fa.agentName)
            fa.assignVariable(varName, value)

        for fa in self.__folderAgentList:
            #print "Coord forward (%s,%s) to %s" % (varName, value, fa.agentName)
            fa.assignVariable(varName, value)

        for pa in self.__processAgentList:
            #print "Coord forward (%s,%s) to %s" % (varName, value, pa.agentName)

            pa.assignVariable(varName, value)

        for co in self.__coordinatorList:
            #print "Coord forward (%s,%s) to %s" % (varName, value, co.agentName)
            co.assignVariable(varName, value)

    def addVeto(self, vetoName):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        self.veto.append(vetoName)

    def addAttribute (self, attribute, value):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()
        

        for fa in self.__fileAgentList:
            #print "Coord forward (%s,%s) to %s" % (attribute, value, fa.agentName)
            fa.addAttribute(attribute, value)

        for fa in self.__folderAgentList:
            #print "Coord forward (%s,%s) to %s" % (attribute, value, fa.agentName)
            fa.addAttribute(attribute, value)

        for pa in self.__processAgentList:
            #print "Coord forward (%s,%s) to %s" % (attribute, value, pa.agentName)

            pa.addAttribute(attribute, value)

        for co in self.__coordinatorList:
            #print "Coord forward (%s,%s) to %s" % (attribute, value, co.agentName)
            co.addAttribute(attribute, value)


    def getFileAgentList(self):
        if runtime.DEBUG_MODE & runtime.OBJ_VERBOSE:
            print utils.whoami()

        return self.__fileAgentList

    def getFolderAgentList(self):
        if runtime.DEBUG_MODE & runtime.OBJ_VERBOSE:
            print utils.whoami()

        return self.__folderAgentList

    def getProcessAgentList(self):
        if runtime.DEBUG_MODE & runtime.OBJ_VERBOSE:
            print utils.whoami()

        return self.__processAgentList

    def getCoordinatorList(self):
        if runtime.DEBUG_MODE & runtime.OBJ_VERBOSE:
            print utils.whoami()
            
        return self.__coordinatorList

    def getNeedsAssignmentDic(self):
        if runtime.DEBUG_MODE & runtime.OBJ_VERBOSE:
            print utils.whoami()
          
        return self.__needsAssignmentDic
    
    def getNeedsOccurrence(self, needName):
        if runtime.DEBUG_MODE & runtime.OBJ_VERBOSE:
            print utils.whoami()
            print utils.whataremyargs()

        return self.__needsOccurrence[needName]



    def isFullyDefined(self):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            
        if self.__isFullyDefined is None :
            if len(self.__needsDic) == len(self.__needsAssignmentDic):
                self.__isFullyDefined = True
                #Now we have to check if all the tree is defined
                for needAssign in self.__needsAssignmentDic.values():
                    needDefined = needAssign.isFullyDefined()
                    print"I check for %s if fully defined because son %s " % (self.agentName, needAssign.agentName)
                    if not needDefined :
                        print  "%s Not fully defined because son %s not defined" % (self.agentName, needAssign.agentName)
                        self.__isFullyDefined = False
                        break  
            else:
                print "%s Not fully defined because len(self.__needsDic) != len(self.__needsAssignmentDic)" % (self.agentName)
                print self.__needsDic.keys()
                print self.__needsAssignmentDic.keys()
                self.__isFullyDefined = False             
        
            #if we are not fully defined, our children (not inherited classes, but children in teh tree) cannot be defined
            if not self.__isFullyDefined:
                for needAssign in self.__needsAssignmentDic.values():
                    print "I set isFullyDefined to False to %s" % needAssign.agentName
                    needAssign.setIsFullyDefined(False)

        return self.__isFullyDefined
    
    def setIsFullyDefined(self, value):
        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            print utils.whoami()
            print utils.whataremyargs()

        self.__isFullyDefined = value
        
    def getRequirements(self):
        if runtime.DEBUG_MODE & runtime.OBJ_VERBOSE:
            print utils.whoami()
            
        return self.__requiredCoordinator


    def __str__(self):
        retStr = "\n------------------------------\n"
        for arg in self.__dir__():
            retStr += "%s -> %s;\n" % (arg, getattr(self, arg))


        if (len(self.__needsDic)):
            retStr += "\nneedsDic\n"
            for k, v in self.__needsDic.items() :
                retStr += "%s -> %s;\n" % (v, k)

        if (len(self.__needsAssignmentDic)):
            retStr += "\nneedsAssignmentDic\n"
            for k, v in self.__needsAssignmentDic.items() :    
                retStr += "%s -> %s; %s\n" % (k, v.agentName, id(v))


        if (len(self.__needsOccurrence)):
            retStr += "\nneedsOccurrence\n"
            for k, v in self.__needsOccurrence.items():
                if k in self.__needsAssignmentDic:
                    retStr += "%s -> %s\n" % (self.__needsAssignmentDic[k].agentName, v)
        
        if (len(self.__requiredCoordinator)):
            retStr += "\nrequiredCoordinator\n"
            for k in self.__requiredCoordinator:
                #retStr += "\t%s\n" % (k.agentName)
                retStr += "\t%s\n" % (k)
        
        if (len(self.veto)):
            retStr += "\nVeto\n"
            for k in self.veto:
                retStr += "\t%s\n" % (k)

        if runtime.DEBUG_MODE & runtime.OBJ_METHOD:
            if(len(self.__fileAgentList)):
                retStr += "\nfileAgentList\n"
                for f in self.__fileAgentList:
                    retStr += f.agentName + "\n"
    
            if(len(self.__folderAgentList)):
                retStr += "\nfolderAgentList\n"
                for f in self.__folderAgentList:
                    retStr += f.agentName + "\n"

            if(len(self.__processAgentList)):
                retStr += "\nprocessAgentList\n"
                for f in self.__processAgentList:
                    retStr += f.agentName + "\n"
    
            if(len(self.__coordinatorList)):
                retStr += "\ncoordinatorList\n"
                for f in self.__coordinatorList:
                    retStr += f.agentName + " %s" % id(f) + "\n"
    

            #print "OBJECT CoordinatorList"
            #for f in self.__coordinatorList:
            #    print f.agentName
            #      print repr(f)
            #   print "OBJECT needsAssignmentDic"
            #   for f in self.__needsAssignmentDic.values():
#               print f.agentName
#               print repr(f)

    #__motherAgent = None
    #__childrenAgents = None

    #__variables = None

       
        if (len(self.__variables)): 
            retStr += "\nvariables\n"
            for key, values in self.__variables.items():
                retStr += "%s -> %s;\n" % (key, values)

        retStr += "\n------------------------------\n"

        return retStr   



#core = Core()
coordName = Word(alphanums + '_')

coordinator_inheritance = COLUMN + coordName("motherAgent")
coordinator_start = "Coordinator" + coordName("agentName") + Optional(coordinator_inheritance) + LBRACE
coordinator_variableAssignment = "set" + Word('$' + alphanums + '_') + OneOrMore(Word('$' + alphanums + '_' + '/' + '.' + '-')) + SEMI
coordinator_needs_assign = Word(alphanums + '_') + EQUAL + Word(alphanums + '_') + SEMI
coordinator_veto = "veto" + Word(alphanums + '_') + SEMI 
coordinator_needs = "needs" + oneOf ("Coordinator FileAgent FolderAgent ProcessAgent") + Word(alphanums + '_') + Optional (EQUAL + Word(alphanums + '_')) + SEMI
coordinator_requires = "requires" + Word(alphanums + '_') + SEMI
coordinator_tolerate = "tolerate " + Word(nums) + SEMI
coordinator_end = RBRACE

#coordinatorDef = coordinator_start +  Dict(Group(ZeroOrMore(coordinator_needs))).setResultsName("myneeds") +   Dict(Group(ZeroOrMore(coordinator_needs_assign))).setResultsName("myneedsAssign") + Dict(Group(ZeroOrMore(coordinator_variableAssignment))).setResultsName("varAssignment") +  coordinator_end
coordinatorMiddle = Or([coordinator_tolerate, coordinator_veto, coordinator_needs, coordinator_needs_assign, coordinator_variableAssignment, coordinator_requires])
coordinatorDef = coordinator_start + ZeroOrMore(coordinatorMiddle) + coordinator_end
#coordinatorDef = coordinator_start +  Dict(Group(ZeroOrMore(coordinator_needs))).setResultsName("myneeds") + coordinator_end
coordinatorDef.ignore(utils.comment)



def parseAction_coordinator_start(token):
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print utils.whoami(), token
    #probably useless test... otherwise the token would not be matched and we would not be here
    if len(token) >= 2: # only "processAgent" and the name of the agent
        ag = Coordinator(token[1])
    
    #if no inheritance
    if len(token) == 2:
        ag.total = runtime.core.getNextTotal()


    if len(token) == 3:
        # If just making a deepcopy, we copy everything, including the __childrenAgents attribute,
        # which is huge and useless (and wrong I think...). The memory grows exponentially,
        # and the parsing is stuck. So we save the __childrenAgents attribute of the mother,
        # empty it, copy the mother, and put it back to her
        motherChildren = runtime.core.getCoordinator(token[2]).getChildrenAgents()
        runtime.core.getCoordinator(token[2]).setChildrenAgents({})
        ag = copy.deepcopy(runtime.core.getCoordinator(token[2]))
        runtime.core.getCoordinator(token[2]).setChildrenAgents(motherChildren)
        
        ag.setMotherAgent(runtime.core.getCoordinator(token[2]), token[1])
        
    runtime.core.addCoordinator(ag)
        

coordinator_start.setParseAction(parseAction_coordinator_start)


def parseAction_coordinator_variableAssignment(token):
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print utils.whoami(), token
    runtime.core.currentCoordinator.assignVariable(token[1], token[2])
coordinator_variableAssignment.setParseAction(parseAction_coordinator_variableAssignment)

def parseAction_coordinator_tolerate(token):
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print utils.whoami(), token
    runtime.core.currentCoordinator.setTolerate(token[1])
coordinator_tolerate.setParseAction(parseAction_coordinator_tolerate)


def parseAction_coordinator_veto(token):
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print utils.whoami(), token
    runtime.core.currentCoordinator.addVeto(token[1])
coordinator_veto.setParseAction(parseAction_coordinator_veto)

def parseAction_coordinator_needs(token):
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print utils.whoami(), token

    if len(token) >= 2: # only "processAgent" and the name of the agent
        occurrence = runtime.core.getNextOccurrence()
        total = None
        if token[1] == "Coordinator":
            total = runtime.core.getNextTotal()
        runtime.core.currentCoordinator.addNeeds(token[2], token[1], occurrence, total)

    if len(token) >= 4:
        assigned = runtime.core.getEntity(token[3])
        runtime.core.currentCoordinator.assignNeeds(token[2], assigned)


coordinator_needs.setParseAction(parseAction_coordinator_needs)



#[NeedName, assigned entity]
def parseAction_coordinator_needs_assign(token):
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print utils.whoami(), token

    assigned = runtime.core.getEntity(token[1])


    # 2013-03-21 
    # When we reassign a need, I destroy all the part of the tree that I replace
    # Problem : we need to give different total and occurrences probably...
    # actually no, it's okay... the sum of the probability is just not one anymore...
    assignmentDic = runtime.core.currentCoordinator.getNeedsAssignmentDic()
    if (token[0] in assignmentDic):
        runtime.core.deleteEntity(assignmentDic[token[0]].agentName)  

    runtime.core.currentCoordinator.assignNeeds(token[0], assigned)

coordinator_needs_assign.setParseAction(parseAction_coordinator_needs_assign)

def parseAction_coordinator_requires(token):
    if runtime.DEBUG_MODE & runtime.OBJ_TRIGGER:
        print utils.whoami(), token
    
    reqCoord = None
    try:
        reqCoord = runtime.core.getCoordinator(token[1])
    except Exception as inst :
        print "Coordinator '%s' required by %s does not exist"%(token[1],runtime.core.currentCoordinator.agentName)
        sys.exit(1)
 
    #runtime.core.currentCoordinator.addRequirement(runtime.core.getCoordinator(token[1]))
    runtime.core.currentCoordinator.addRequirement(reqCoord.agentName)

coordinator_requires.setParseAction(parseAction_coordinator_requires)


#def parseAction_coordinatorDef(token):
#    print "--------------------------------------------"
#    print " I have just created the following entity : "
#    print runtime.core.currentCoordinator
#    print "needsAssignment ", id(runtime.core.currentCoordinator.getNeedsAssignmentDic())
#    print "coordinatorDic ", id(runtime.core.currentCoordinator.getCoordinatorList())
#    print "--------------------------------------------"

#coordinatorDef.setParseAction(parseAction_coordinatorDef)

#for coord in coordinatorDef.searchString(coordTest):
#    print 'dump ', coord

#runtime.core.listCoordinators()
