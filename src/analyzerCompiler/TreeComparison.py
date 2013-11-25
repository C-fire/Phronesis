import MySQLdb, sys

class TreeComparison:

    dbAgents = {}
    dbLinks = []
    dbDependencyRules = []
    dbServers = {}
    fileAgents = {}
    fileLinks = []
    fileDependencyRules = []
    fileServers = {}


    #agentEquivalences = {}
    #newAgent = []

    
    totEquivalences = {}
    newTot = []
    linkEquivalences = {}
    newLinks = []
    removedLinks = []
    removedTot = []

    occEquivalences = {}
    newOccs = []
    removedOccs = []

    agentIdEquivalences = {}
    newAgentIds = []
    removedAgentIds = []
    #removedFileAgentIds = []
    #removedProcessAgentIds = []
    #removedEnvironmentAgentIds = []

    serverEquivalences = {}
    newServers = []
    removedServers = []

    dependencyRuleEquivalences = {}
    newDependencyRules = []
    removedDependencyRules = []


    metaADic = {}

    maxOcc = 0
    counterOcc = 1
    maxLink = 0
    counterLink = 1
    maxTot = 0
    counterTot = 1
    maxAgentId = 0
    counterAgentId = 1
    maxServer = 0
    counterServer = 1
    maxFile = 0
    counterFile = 1
    maxProcess = 0
    counterProcess = 1
    maxEnvironment = 0
    counterEnvironment = 1
    maxDependencyRule = 0
    counterDependencyRule = 1

    #self.allDbEntities = ['MetaSite','MetaConf','MetaData','MetaHttpd','Site1','Conf1','http1','Data1','F1','F2','Site2','Conf2','http2','Data2','F3','F4','X','Y','A','B','C','D','E','F']
    #self.allFileEntities = ['U','V','MetaSite','MetaConf','MetaData','MetaHttpd','Site1','Conf1','http1','Data1','F1','F2','F3','Site3','Conf3','http3','Data3','F4','F5','C','E','F']
    #self.metaFileEntities = ['MetaSite','MetaConf','MetaData','MetaHttpd']

    allDbEntities = []
    allFileEntities = []
    metaFileEntities = []

    graphVizDb = """digraph finite_state_machine {
        rankdir=LR;
        size="18,18"
        node [shape = circle];"""

    graphVizFile = """digraph finite_state_machine {
        rankdir=LR;
        size="18,18"
        node [shape = circle];"""

    def __init__(self):
        pass

    def compare(self, metaAFileDic, linksFileTuples, serverDic, dependencyRulesFileList):
        self.connectToDb('localhost','root','','phronesisAnalyzer',3306)
       
        try: 
            self.maxOcc = int(self.executeRequest("select max(id) from Occurrence;")[0][0])
        except:
            pass
        try:
            self.maxLink = int(self.executeRequest("select max(id) from MetaAgentTree;")[0][0]) 
        except:
            pass
        try:
            self.maxTot = int(self.executeRequest("select max(id) from Total;")[0][0])
        except:
            pass
        try:
            self.maxAgentId = int(self.executeRequest("select max(id) from MetaAgent;")[0][0])
        except:
            pass
        try:
            self.maxServer = int(self.executeRequest("select max(id) from Server;")[0][0])
        except:
            pass
        try:
           self.maxFile = int(self.executeRequest("select max(id) from FileAgent;")[0][0])
        except:
            pass
        try:
            self.maxProcess = int(self.executeRequest("select max(id) from ProcessAgent;")[0][0])
        except:
            pass
        try:
            self.maxEnvironment = int(self.executeRequest("select max(id) from EnvironmentAgent;")[0][0])
        except:
            pass
        try:
            self.maxDependencyRule = int(self.executeRequest("select max(id) from Dependency;")[0][0])
        except:
            pass


        result = self.executeRequest("select id, name, id_total from MetaAgent;")
#(7L, 'f1', None,,
        for a in result:
            id_total = None
            if a[2]:
                id_total = a[2]
            self.createDbAgent(a[0], id_total,a[1]) 
            self.metaADic[a[0]] = a[1]
         
        result = self.executeRequest("select id, id_occurrence, id_parent, id_daughter from MetaAgentTree;")
        for l in result:
            self.createDbLink(int(l[0]), int(l[1]), self.metaADic[l[2]], self.metaADic[l[3]])


        result = self.executeRequest("select a.name, a.type, case when b.id then b.id when c.id then c.id when d.id then d.id END from MetaAgent a  left join FileAgent b on a.id = b.id_metaAgent  left join ProcessAgent c on a.id = c.id_metaAgent left join EnvironmentAgent d on a.id = d.id_metaAgent;")
        for l in result:
            if not l[2]:
                continue
            if l[1] == "EnvironmentAgent":
                print "%s is a environment agent and its id is %s"%(l[0], int(l[2])) 
                self.dbAgents[l[0]].idEnvironment = int(l[2])
            elif l[1] == "ProcessAgent":
                print "%s is a Process agent and its id is %s"%(l[0], int(l[2])) 
                self.dbAgents[l[0]].idProcess = int(l[2])
            else:
                print "%s is a File/Folder agent and its id is %s"%(l[0], int(l[2])) 
                self.dbAgents[l[0]].idFile = int(l[2])


        result = self.executeRequest("select id, id_prior, id_after from Dependency;")
        for l in result:
            self.dbDependencyRules.append([int(l[0]), int(l[1]),int(l[2])])

        self.fileDependencyRules = dependencyRulesFileList

        result = self.executeRequest("select id, name from Server;")
       
        for l in result:
            self.dbServers[l[1]] = TreeComparison.Server(int(l[0]), l[1])

        for s in serverDic.values():
            self.fileServers[s.agentName] = TreeComparison.Server(s.idServer, s.agentName)
 
        dbServerSet = set(self.dbServers.keys())
        fileServerSet = set(self.fileServers.keys())
        newServerSet = fileServerSet - dbServerSet
        removeServerSet = dbServerSet - fileServerSet
        stillPresentServer = dbServerSet.intersection(fileServerSet)

        for s in newServerSet:
            sId = self.getNextServer()
            self.newServers.append(sId)
            self.serverEquivalences[self.fileServers[s].id] = sId

        for s in removeServerSet:
            self.removedServers.append(self.dbServers[s].id)

        for s in stillPresentServer:
            self.serverEquivalences[self.fileServers[s].id] = self.dbServers[s].id
        


        for a in metaAFileDic.values():
            try:
                self.createFileAgent(a.idAgent, a.total, a.agentName)
            except:
                self.createFileAgent(a.idAgent, None, a.agentName)
            if not a.isFullyDefined():
                print "%s is NOT fully defined"%a.agentName
                if a.agentName in self.dbAgents.keys():
                    print "AND is in the dbAgents"
                    self.metaFileEntities.append(a.agentName)
            else:
                print "%s is fully defined"%a.agentName


         

        for l in linksFileTuples:
            self.createFileLink(l[0], l[1], l[2], l[3])

        self.graphVizDb += "}\n"
        f = open("/tmp/db.dot", 'w')
        f.write(self.graphVizDb)
        f.close()

        print "-------  DB  -------\n"
        for a,b in self.dbAgents.items():
            print b

        for l in self.dbLinks:
            print l

        for s in self.dbServers.values():
            print s

       
        print "------- FILE -------\n"

        for a,b in self.fileAgents.items():
            print b

        for l in self.fileLinks:
            print l

        for s in self.dbServers.values():
            print s

        self.graphVizFile += "}\n"
        f = open("/tmp/file.dot", 'w')
        f.write(self.graphVizFile)
        f.close()

        print "-------MetaFileEntities ----------\n"
        for f in self.metaFileEntities:
            print f


        print "----------------------\n"


        self.analyzeList(self.metaFileEntities)
    
        stillInPlace = []
        newElements = []
        for e in self.allFileEntities:
            if e in self.allDbEntities:
                stillInPlace.append(e)
            else:
                newElements.append(e)
    
        self.analyzeList(stillInPlace)
    
        for e in newElements:
            children = self.getFileLinksFrom(e)
            print "new element : %s with children %s"%(e, children)
            self.setTotalEquivalent(e)
            self.setAgentIdEquivalent(e)
            for c in children:
                self.setOccEquivalent(e, c)
                self.setLinkEquivalent(e, c)
    
        for e in self.allDbEntities:
            print e," dispears"
            self.invalidateDbTreeFrom(e)
            agentId = self.dbAgents[e].idAgent
            # I wonder if this test makes sense...
            if agentId not in self.agentIdEquivalences.values():
                self.removedAgentIds.append(agentId)

                children = self.getDbLinksFrom(e)
                for c in children:
                    dbLinkId, dbOccId = self.linkIdInDb(e, c)
                    #self.removeDbLink(dbLinkId)
                    self.removeDbLink(dbLinkId)
            else:
                print "THE HELL!!! When does this happen??? %s"%agentId
                

#                if self.dbAgents[e].idFile:
#                    self.removedFileAgentIds.append(self.dbAgents[e].idFile)
#                if self.dbAgents[e].idProcess:
#                    self.removedProcessAgentIds.append(self.dbAgents[e].idProcess)
#                if self.dbAgents[e].idEnvironment:
#                    self.removedFileAgentIds.append(self.dbAgents[e].idEnvironment)
    

        #Updating the rules now
        for r in self.dbDependencyRules:
            if (r[1] in self.removedAgentIds) or (r[2] in self.removedAgentIds):
                print "One of the agent is to be removed, so I remove the dependency rule : %s"%r
                self.removedDependencyRules.append(r)
            
        for r in self.fileDependencyRules:
            print "current fileDependencyRule : %s"%r
            r1 = self.agentIdEquivalences[r[1]]
            r2 = self.agentIdEquivalences[r[2]]
            newDependencyRule = True
            for rdb in self.dbDependencyRules:
                if (r1 == rdb[1]) and (r2 == rdb[2]):
                    print "We have a correspondance for dependency rule %s : %s"%(r,rdb)
                    self.dependencyRuleEquivalences[r[0]] = rdb[0]
                    newDependencyRule = False
                    break
            if newDependencyRule:
                print "Cool a new dependency rule : %s"%r
                newRId = self.getNextDependencyRule()
                self.newDependencyRules.append(newRId)
                self.dependencyRuleEquivalences[r[0]] = newRId
                   



        dbAgentIds = {}
        for f in self.dbAgents.values():
            dbAgentIds[f.idAgent] = f.name
 
        fileAgentIds = {}
        for f in self.fileAgents.values():
            fileAgentIds[f.idAgent] = f.name
            
        
      
        print "----- SUMMARY -----\n"
        print "LINKS TO DELETE\n"
        for o in self.removedLinks:
            print "%s"%(o)

        print "\nNEW LINKS\n"
        for o in self.newLinks:
            print "%s"%o

        print "\nLINKS CORRESPONDANCE\n"
        for a,b in self.linkEquivalences.items():
            print "%s -> %s"%(a,b)


        print "OCCURRENCES TO DELETE\n"
        for o in self.removedOccs:
            print "%s"%(o)

        print "\nNEW OCCURRENCES\n"
        for o in self.newOccs:
            print "%s"%o

        print "\nOCCURRENCES CORRESPONDANCE\n"
        for a,b in self.occEquivalences.items():
            print "%s -> %s"%(a,b)
        print "\nTOTAL TO DELETE\n"
        for o in self.removedTot:
            print "%s"%o
        print "\nNEW TOTAL\n"
        for t in self.newTot:
            print "%s"%t

        print "\nTOTAL CORRESPONDANCE\n"
        for a,b in self.totEquivalences.items():
            print "%s -> %s"%(a,b)

 
        print "\nAGENT ID TO DELETE\n"
        for o in self.removedAgentIds:
            print "%s (%s)"%(o,dbAgentIds[o])
        print "\nNEW AGENT ID\n"
        for t in self.newAgentIds:
            print "%s "%(t)

        print "\nAGENT ID CORRESPONDANCE\n"
        for a,b in self.agentIdEquivalences.items():
            dbName = ""
            fileName = ""
            try:
                fileName = fileAgentIds[a]
                dbName = dbAgentIds[b]
            except:
                pass
            print "%s (%s) -> %s (%s)"%(a,fileName,b,dbName)

        print "SERVERS TO DELETE\n"
        for o in self.removedServers:
            print "%s"%(o)

        print "\nNEW SERVERS\n"
        for o in self.newServers:
            print "%s"%o

        print "\nServers CORRESPONDANCE\n"
        for a,b in self.serverEquivalences.items():
            print "%s -> %s"%(a,b)

        print "\nRULE TO DELETE\n"
        for o in self.removedDependencyRules:
            print "%s"%o
        print "\nNEW RULE\n"
        for t in self.newDependencyRules:
            print "%s"%t

        print "\nRULE CORRESPONDANCE\n"
        for a,b in self.dependencyRuleEquivalences.items():
            print "%s -> %s"%(a,b)

    def fakeDics(self, metaAFileDic, linksFileTuples, serverDic, dependencyRulesFileList):
        for s in serverDic.values():
            self.serverEquivalences[s.idServer] = s.idServer
            self.newServers.append(s.idServer)
        for a in metaAFileDic.values():
            self.agentIdEquivalences[a.idAgent] = a.idAgent
            self.newAgentIds.append(a.idAgent)
            try:
                self.totEquivalences[a.total] = a.total
                self.newTot.append(a.total)
            except: #only coords have a total
                pass
        for l in linksFileTuples:
            self.linkEquivalences[l[0]] = l[0]
            self.newLinks.append(l[0])
            self.occEquivalences[l[1]] = l[1]
            self.newOccs.append(l[1])
        for r in dependencyRulesFileList:
            self.dependencyRuleEquivalences[r[0]] = r[0]
            self.newDependencyRules.append(r[0])


    def connectToDb(self, url, user,passwd,name,port):
        try:
            self.database = MySQLdb.connect(url, user, passwd, name, port)
        except Exception, err:
            print "Connection to the database failed : " + str(err)
            sys.exit(1)
        else:
            self.cursor = self.database.cursor()   # creation du curseur


    def executeRequest(self,req):
        """Function that executes a request given in parameter and return the results"""
        try:
            self.cursor.execute(req)
            return self.cursor.fetchall()
        except Exception, err:
            #The SQL request has an error
            print "Error in the SQL request : " + req + " " + str(err)
            sys.exit(1)

    def close(self):
        try:
            self.database.close()
        except Exception, err:
            print "Closing database failed : " + str(err)
            sys.exit(1)




    class Agent:
        total = None
        name = None
        changed = None
        valid = None
        idAgent = None
        
        def __init__(self, idAgent, total, name):
            self.idAgent = idAgent
            self.total = total
            self.name = name
    
        def __str__(self):
            return "%s (%s) %s"%(self.name, self.idAgent, self.total)
    
    
    class Link:
        occId = None
        localId = None
        start = None
        end = None
    
    
        def __init__(self,localId, occId, start,end):
            self.localId = localId
            self.occId = occId
    	    self.start = start
    	    self.end = end
    
        def __str__(self):
    	    return "%s (%s): %s -> %s"%(self.localId, self.occId, self.start, self.end)
    
    class Server:
        id = None
        name = None
        
        def __init__(self, id, name):
            self.id = id
            self.name = name

        def __str__(self):
            return "%s (%s)"%(self.name, self.id)
    
    def createAgent(self, agentDic, idAgent, total, name):
        a =TreeComparison.Agent(idAgent, total,name)
        agentDic[name] = a
    
    def createLink(self, linkDic, localId, occId, startName, endName):
        l = TreeComparison.Link(localId, occId, startName, endName)
        linkDic.append(l)
    
    def createDbAgent(self, idAgent, tot, name):
        self.allDbEntities.append(name)
        self.createAgent(self.dbAgents, idAgent, tot, name)
    
    def createDbLink(self, localId, occId,startName, endName):
        self.createLink(self.dbLinks, localId, occId, startName, endName)
	self.graphVizDb += "%s_%s_%s -> %s_%s_%s [ label = \"%s_%s\" ];\n"%(startName,self.dbAgents[startName].idAgent, self.dbAgents[startName].total, endName,self.dbAgents[endName].idAgent, self.dbAgents[endName].total, localId, occId); 
    
    def createFileAgent(self, idAgent, tot, name):
        self.allFileEntities.append(name)
        self.createAgent(self.fileAgents, idAgent, tot,name)
    
    def createFileLink(self, localId, occId,startName, endName):
        self.createLink(self.fileLinks, localId, occId, startName, endName)
        self.graphVizFile += "%s_%s_%s -> %s_%s_%s [ label = \"%s_%s\" ];\n"%(startName,self.fileAgents[startName].idAgent, self.fileAgents[startName].total, endName,self.fileAgents[endName].idAgent, self.fileAgents[endName].total, localId, occId);

    
    
    def getLinksFrom(self, linkList, entityName):
        links = []
        for l in linkList:
            if l.start == entityName:
                links.append(l.end)
        links.sort()
        return links
    
    def getDbLinksFrom(self, entityName):
        return self.getLinksFrom(self.dbLinks, entityName)
    
    def getFileLinksFrom(self, entityName):
        return self.getLinksFrom(self.fileLinks, entityName)
        
    
    def linkIdIn(self, linkList, startName, endName):
        retId = retOcc = -1
        for l in linkList:
            if l.start == startName and l.end == endName:
                retId = l.localId
                retOcc = l.occId
                break
        return retId, retOcc
    
    
    def linkIdInDb(self, startName, endName):
        return self.linkIdIn(self.dbLinks, startName, endName)
    
    def linkIdInFile(self, startName, endName):
        return self.linkIdIn(self.fileLinks, startName, endName)
    
    def hasChanged(self, entityName):
        changed = False
        print "Checking if %s has changed"%entityName
        entity = self.fileAgents[entityName]
        if entity.changed != None:
            changed = entity.changed
        else:
            dbLinks = self.getDbLinksFrom(entityName)
            fileLinks = self.getFileLinksFrom(entityName)
            if dbLinks != fileLinks:
                print "dbLinks != fileLinks : %s"%(dbLinks != fileLinks)
                print "dbLinks %s"%dbLinks
                print "fileLinks %s"%fileLinks
                changed = True
            else :
                for l in fileLinks:
                    if not changed:
                        changed = changed or self.hasChanged(l)
        entity.changed = changed
        return changed
            
    
    
    
    def getNextOccId(self, ):
        ret = self.maxOcc + self.counterOcc
        self.counterOcc += 1
        return ret
    
    def getNextLinkId(self, ):
        ret = self.maxLink + self.counterLink
        self.counterLink += 1
        return ret
    
    def getNextTot(self, ):
        ret = self.maxTot + self.counterTot
        self.counterTot += 1
        return ret

    def getNextAgentId(self):
        ret = self.maxAgentId + self.counterAgentId
        self.counterAgentId += 1
        return ret
    
    def getNextServer(self):
        ret = self.maxServer + self.counterServer
        self.counterServer += 1
        return ret
    
    def getNextFile(self):
        ret = self.maxFile + self.counterFile
        self.counterFile += 1
        return ret

    def getNextProcess(self):
        ret = self.maxProcess + self.counterProcess
        self.counterProcess += 1
        return ret

    def getNextEnvironment(self):
        ret = self.maxEnvironment + self.counterEnvironment
        self.counterEnvironment += 1
        return ret

    def getNextDependencyRule(self, ):
        ret = self.maxDependencyRule + self.counterDependencyRule
        self.counterDependencyRule += 1
        return ret

    def setOccEquivalent(self, startName, endName):
        fileLinkId, fileOccId = self.linkIdInFile(startName, endName)
        dbLinkId, dbOccId = self.linkIdInDb(startName, endName) 
        for l in self.removedOccs:
            if l.occId == dbOccId:
                print "The occ in db %s will be removed, so I assign a new one"%dbOccId
                dbOccId = -1
                break
        if fileOccId not in self.occEquivalences:
            if dbOccId == -1:
                dbOccId = self.getNextOccId()
                self.newOccs.append(dbOccId)
                print "new db occ id : %s"%dbOccId
            print "%s -> %s : %s -> %s"%(startName, endName, fileOccId, dbOccId)
            self.occEquivalences[fileOccId] = dbOccId
        else:
            print "%s -> %s : already taken care of"%(startName, endName)
    
    def setLinkEquivalent(self, startName, endName):
        fileLinkId, fileOccId = self.linkIdInFile(startName, endName)
        dbLinkId, dbOccId = self.linkIdInDb(startName, endName) 
        for l in self.removedLinks:
            if l.localId == dbLinkId:
                print "The link in db %s will be removed, so I assign a new one"%dbLinkId
                dbLinkId = -1
                break
        if dbLinkId == -1 :
            dbLinkId = self.getNextLinkId()
            self.newLinks.append(dbLinkId)
            print "New db link id : %s"%dbLinkId
        else:
            print "Link already exists %s"%dbLinkId
        self.linkEquivalences[fileLinkId] = dbLinkId
    
    def setTotalEquivalent(self, entityName):
        fileTot = self.fileAgents[entityName].total
        if fileTot not in self.totEquivalences:
            try:
                dbTot = self.dbAgents[entityName].total
            except:
                dbTot = self.getNextTot()
                self.newTot.append(dbTot)
                print "New total : %s"%dbTot
            print "Total of %s goes from %s to %s"%(entityName, fileTot,dbTot)
            self.totEquivalences[fileTot] = dbTot
        else:
            print "Total of %s already taken care of"%entityName 
            
    
    def setAgentIdEquivalent(self, entityName):
        fileAgentId = self.fileAgents[entityName].idAgent
        if fileAgentId not in self.agentIdEquivalences:
            try:
                dbAgentId = self.dbAgents[entityName].idAgent
            except:
                dbAgentId = self.getNextAgentId()
                self.newAgentIds.append(dbAgentId)
                print "New agentId : %s"%dbAgentId
            print "AgentId of %s goes from %s to %s"%(entityName, fileAgentId,dbAgentId)
            self.agentIdEquivalences[fileAgentId] = dbAgentId
        else:
            print "AgentId of %s already taken care of"%entityName 
            
    
     
    #def removeLink(self, linkList, elId):
    #    toPop = None
    #    for i in range(len(linkList)):
    #        if linkList[i].id == elId:
    #            toPop = linkList.pop(i)
    #            break
    #    return toPop
    
    def removeDbLink(self, dbId):
        print "remove from db link id %s"%dbId
        if dbId in self.linkEquivalences.values():
            print "seems that it is needed, I don't touch it"
            return 
        poped = None
        for i in range(len(self.dbLinks)):
            if self.dbLinks[i].localId == dbId:
                #poped = self.dbLinks.pop(i)
                poped = self.dbLinks[i]
                break
        #self.removedLinks.append(dbId)
        if not poped:
            print "NOT FOUND localId %s in dbLinks"%dbId
        if poped not in self.removedLinks:
            self.removedLinks.append(poped)
        else:
            print "ALREADY IN REMOVED LINK %s"%poped
    
    def removeDbOcc(self, dbId):
        print "remove from db Occ id %s"%dbId
        poped = None
        for i in range(len(self.dbLinks)):
            if self.dbLinks[i].occId == dbId:
                poped = self.dbLinks[i]
                #poped = self.dbLinks.pop(i)
                break
        #self.removedLinks.append(dbId)
        self.removedOccs.append(poped)
    
    def invalidateDbTreeFrom(self, entityName):
        print "Invalidate db tree from ",entityName
        fileChildren = []
        children = self.getDbLinksFrom(entityName)
        tot = self.dbAgents[entityName].total


        # BUG ???
        # This test is to make sure that we are not removing shared experience
        # But how about cases when we remove the total, but then realize that we need it?
        # --> This cannot happen since we first check all the abstract entity that are in files AND db.
        if tot not in self.totEquivalences.values():
            self.removedTot.append(tot)
        #else:
        #    if entityName in self.fileAgents:
        #        print "HERE setAgentIdEquivalent ",entityName
        #        self.setAgentIdEquivalent(entityName)
        #        fileChildren = self.getFileLinksFrom(entityName)
        #        print "fileChildren %s"%fileChildren

        
        if entityName in self.fileAgents:
            print "HERE setAgentIdEquivalent ",entityName
            self.setAgentIdEquivalent(entityName)
            fileChildren = self.getFileLinksFrom(entityName)
            print "fileChildren %s"%fileChildren

        for c in children:
            dbLinkId, dbOccId = self.linkIdInDb(entityName, c)
            #self.removeDbLink(dbLinkId)
            if dbOccId not in self.occEquivalences.values():
                self.removeDbOcc(dbOccId)
                self.removeDbLink(dbLinkId)
                
            else:
                print "Cannot remove link from %s to %s because used"%(entityName, c)
        for c in fileChildren:
           self.setLinkEquivalent(entityName,c)
           self.setOccEquivalent(entityName, c)
    
    def validateTreeFrom(self, entityName):
        print "Validate db tree from ",entityName
        children = self.getFileLinksFrom(entityName)
        self.setTotalEquivalent(entityName)
        self.setAgentIdEquivalent(entityName)
        for c in children:
            fileLinkId, fileOccId = self.linkIdInFile(entityName,c)
            if fileOccId not in self.occEquivalences.keys():
                self.setOccEquivalent(entityName, c)
            self.setLinkEquivalent(entityName,c)
            #validateTreeFrom(c)
        
    
    
    
    
    
    
    def analyzeList(self, listName):
       for e in listName:
           changed = self.hasChanged(e)
           print "%s changed %s"%(e,changed)
           if not changed:
               self.validateTreeFrom(e)
           else:
               self.invalidateDbTreeFrom(e)
           try:
               self.allDbEntities.remove(e)
           except:
               print "%s does not exist in the db"%e
           try:
               self.allFileEntities.remove(e)
           except:
               print "%s does not exist in the File"%e
    
#    analyzeList(self.metaFileEntities)
#    
#    stillInPlace = []
#    newElements = []
#    for e in self.allFileEntities:
#        if e in self.allDbEntities:
#            print e," is still in place"
#            stillInPlace.append(e)
#        else:
#            print e," is new"
#            newElements.append(e)
#    
#    analyzeList(stillInPlace)
#    
#    for e in newElements:
#        children = getFileLinksFrom(e)
#        print "new element : %s with children %s"%(e, children)
#        setTotalEquivalent(entityName)
#        for c in children:
#            setEquivalent(e, c)
#    
#    for e in self.allDbEntities:
#        print e," dispears"
#        invalidateDbTreeFrom(e)


