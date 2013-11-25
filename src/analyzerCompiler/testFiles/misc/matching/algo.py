class Agent:
    total = None
    name = None
    changed = None
    
    def __init__(self, total, name):
        self.total = total
        self.name = name

    def __str__(self):
        return "%s"%(self.name)


class Link:
    id = None
    start = None
    end = None


    def __init__(self,id, start,end):
        self.id = id
	self.start = start
	self.end = end

    def __str__(self):
	return "%s : %s -> %s"%(self.id, self.start, self.end)



def createAgent(agentDic, total, name):
    a = Agent(total,name)
    agentDic[name] = a

def createLink(linkDic, id, startName, endName):
    l = Link(id, startName, endName)
    linkDic.append(l)

def createDbAgent(tot, name):
    createAgent(dbAgents,tot, name)

def createDbLink(id,startName, endName):
    createLink(dbLinks, id, startName, endName)

def createFileAgent(tot, name):
    createAgent(fileAgents, tot,name)

def createFileLink(id,startName, endName):
    createLink(fileLinks, id, startName, endName)


def getLinksFrom(linkList, entityName):
    links = []
    for l in linkList:
        if l.start == entityName:
            links.append(l.end)
    links.sort()
    return links

def getDbLinksFrom(entityName):
    return getLinksFrom(dbLinks, entityName)

def getFileLinksFrom(entityName):
    return getLinksFrom(fileLinks, entityName)
    

def linkIdIn(linkList, startName, endName):
    ret = -1
    for l in linkList:
        if l.start == startName and l.end == endName:
            ret = l.id
            break
    return ret


def linkIdInDb(startName, endName):
    return linkIdIn(dbLinks, startName, endName)

def linkIdInFile(startName, endName):
    return linkIdIn(fileLinks, startName, endName)

def hasChanged(entityName):
    changed = False
    entity = fileAgents[entityName]
    if entity.changed != None:
        changed = entity.changed
    else:
        dbLinks = getDbLinksFrom(entityName)
        fileLinks = getFileLinksFrom(entityName)
        if dbLinks != fileLinks:
            changed = True
        else :
            for l in fileLinks:
                if not changed:
                    changed = changed or hasChanged(l)
    entity.changed = changed
    print "%s changed? %s"%(entityName, changed)
    return changed
        



dbAgents = {}
dbLinks = []


createDbAgent(0, 'MetaSite')
createDbAgent(1, 'MetaConf')
createDbAgent(2, 'MetaData')
createDbAgent(-1, 'MetaHttpd')
createDbLink(0, 'MetaSite','MetaConf')
createDbLink(1, 'MetaSite','MetaHttpd')
createDbLink(2, 'MetaSite','MetaData')


createDbAgent(0, 'Site1')
createDbAgent(1, 'Conf1')
createDbLink (0, 'Site1', 'Conf1')
createDbAgent(-1, 'http1')
createDbLink(1,'Site1', 'http1')
createDbAgent(2, 'Data1')
createDbLink(2, 'Site1', 'Data1')
createDbAgent(-1, 'F1')
createDbLink(3, 'Conf1', 'F1')
createDbAgent(-1, 'F2')
createDbLink(4, 'Data1', 'F2')


createDbAgent(0, 'Site2')
createDbAgent(1, 'Conf2')
createDbLink (0, 'Site2', 'Conf2')
createDbAgent(-1, 'http2')
createDbLink(1,'Site2', 'http2')
createDbAgent(2, 'Data2')
createDbLink(2, 'Site2', 'Data2')
createDbAgent(-1, 'F3')
createDbLink(6, 'Conf2', 'F3')
createDbAgent(-1, 'F4')
createDbLink(7, 'Data2', 'F4')

createDbAgent(3, 'X')
createDbAgent(-1, 'Y')
createDbLink(5,'X','Y')

createDbAgent(4, 'A')
createDbAgent(5, 'B')
createDbLink(8, 'A', 'B')
createDbAgent(6, 'C')
createDbLink(9, 'A','C')
createDbAgent(-1, 'D')
createDbLink(10, 'B', 'D')
createDbAgent(-1, 'E')
createDbLink(11, 'C', 'E')
createDbAgent(-1, 'F')
createDbLink(12, 'C', 'F')

#for a,b in dbAgents.items():
#    print a, b

#for l in dbLinks:
#    print l


fileAgents = {}
fileLinks = []


createFileAgent(0, 'U')
createFileAgent(-1, 'V')
createFileLink(0,'U','V')


createFileAgent(1, 'MetaSite')
createFileAgent(2, 'MetaConf')
createFileAgent(3, 'MetaData')
createFileAgent(-1, 'MetaHttpd')
createFileLink(1, 'MetaSite','MetaConf')
createFileLink(2, 'MetaSite','MetaHttpd')
createFileLink(3, 'MetaSite','MetaData')


createFileAgent(1,'Site1')
createFileAgent(2,'Conf1')
createFileLink (2, 'Site1', 'Conf1')
createFileAgent(-1,'http1')
createFileLink(3,'Site1', 'http1')
createFileAgent(3, 'Data1')
createFileLink(3, 'Site1', 'Data1')
createFileAgent(-1,'F1')
createFileLink(4, 'Conf1', 'F1')
createFileAgent(-1,'F2')
createFileLink(5, 'Data1', 'F2')
createFileAgent(-1,'F3')
createFileLink(6, 'Data1', 'F3')

createFileAgent(1, 'Site3')
createFileAgent(2, 'Conf3')
createFileLink (1, 'Site3', 'Conf3')
createFileAgent(-1, 'http3')
createFileLink(2,'Site3', 'http3')
createFileAgent(3, 'Data3')
createFileLink(3, 'Site3', 'Data3')
createFileAgent(-1, 'F4')
createFileLink(7, 'Conf3', 'F4')
createFileAgent(-1, 'F5')
createFileLink(8, 'Data3', 'F5')


createFileAgent(4, 'C')
createFileAgent(-1, 'E')
createFileLink(9, 'C', 'E')
createFileAgent(-1, 'F')
createFileLink(10, 'C', 'F')


maxDb = 10
counter = 0
def getNextId():
    global counter
    ret = maxDb + counter
    counter += 1
    return ret


maxTot = 10
counterTot = 0
def getNextTot():
    global counterTot
    ret = maxTot + counterTot
    counterTot += 1
    return ret

totEquivalences = {}
newTot = []


linkEquivalences = {}
newLinks = []
def setEquivalent(startName, endName):
    fileId = linkIdInFile(startName, endName)
    dbId = linkIdInDb(startName, endName) 
    if fileId not in linkEquivalences:
        if dbId == -1:
            dbId = getNextId()
            newLinks.append(dbId)
            print "new db id : %s"%dbId
        print "%s -> %s : %s -> %s"%(startName, endName, fileId, dbId)
        linkEquivalences[fileId] = dbId
    else:
        print "%s -> %s : already taken care of"%(startName, endName)


def setTotalEquivalent(entityName):
    fileTot = fileAgents[entityName].total
    if fileTot not in totEquivalences:
        try:
            dbTot = dbAgents[entityName].total
        except:
            dbTot = getNextTot()
            newTot.append(dbTot)
            print "New total : %s"%dbTot
        print "Total of %s goes from %s to %s"%(entityName, fileTot,dbTot)
        totEquivalences[fileTot] = dbTot
    else:
        print "Total of %s already taken care of"%entityName 
        


 
def removeLink(linkList, elId):
    for i in range(len(linkList)):
        if linkList[i].id == elId:
            linkList.pop(i)
            break

def removeDbLink(dbId):
    print "remove from db link id %s"%dbId
    removeLink(dbLinks, dbId)


def invalidateDbTreeFrom(entityName):
    print "Invalidate db tree from ",entityName
    children = getDbLinksFrom(entityName)
    for c in children:
        dbId = linkIdInDb(entityName, c)
        if dbId not in linkEquivalences.values():
            removeDbLink(dbId)
        else:
            print "Cannot remove link from %s to %s because used"%(entityName, c)
        #invalidateDbTreeFrom(c)

def validateTreeFrom(entityName):
    children = getFileLinksFrom(entityName)
    setTotalEquivalent(entityName)
    for c in children:
        fileId = linkIdInFile(entityName,c)
        if fileId not in linkEquivalences.keys():
            setEquivalent(entityName, c)
        #validateTreeFrom(c)
    




allDbEntities = ['MetaSite','MetaConf','MetaData','MetaHttpd','Site1','Conf1','http1','Data1','F1','F2','Site2','Conf2','http2','Data2','F3','F4','X','Y','A','B','C','D','E','F']
allFileEntities = ['U','V','MetaSite','MetaConf','MetaData','MetaHttpd','Site1','Conf1','http1','Data1','F1','F2','F3','Site3','Conf3','http3','Data3','F4','F5','C','E','F']

metaFileEntities = ['MetaSite','MetaConf','MetaData','MetaHttpd']


def analyzeList(listName):
   for e in listName:
       changed = hasChanged(e)
       if not changed:
           validateTreeFrom(e)
       else:
           invalidateDbTreeFrom(e)
       allDbEntities.remove(e)
       allFileEntities.remove(e)

analyzeList(metaFileEntities)

stillInPlace = []
newElements = []
for e in allFileEntities:
    if e in allDbEntities:
        print e," is still in place"
        stillInPlace.append(e)
    else:
        print e," is new"
        newElements.append(e)

analyzeList(stillInPlace)

for e in newElements:
    children = getFileLinksFrom(e)
    print "new element : %s with children %s"%(e, children)
    setTotalEquivalent(e)
    for c in children:
        setEquivalent(e, c)

for e in allDbEntities:
    print e," dispears"
    invalidateDbTreeFrom(e)
