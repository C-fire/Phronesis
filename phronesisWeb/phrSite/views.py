# Create your views here.
from django.http import HttpResponse
from django.core.urlresolvers import reverse
from django.http import HttpResponseRedirect
from django.shortcuts import render, redirect
from phrSite.models import Metaagent,Recoveryhistory,Server, Dependency
from phrSite.mySearch import get_query
from django import forms
import time
import copy

def index(request):
    context = {}
    return render(request, 'phrSite/index.html', context)

def metaAgent_detail(request, ma_id):
    return HttpResponse("You're looking at MetaAgent %s." % ma_id)


def search(request):  
    query_string = ''
    found_entries = None
    if ('q' in request.GET) and request.GET['q'].strip():
        query_string = request.GET['q']
        
        metaagent_query = get_query(query_string, ['name'])
        metaagent_entries = Metaagent.objects.filter(metaagent_query).order_by('name')

        history_query = get_query(query_string, ['responsible'])
        history_entries = Recoveryhistory.objects.filter(history_query).order_by('date')

        server_query = get_query(query_string, ['name'])
        server_entries = Server.objects.filter(server_query).order_by('name')
    #return render_to_response('search/search_results.html',
    #                      { 'query_string': query_string, 'found_entries': found_entries },
    #                      context_instance=RequestContext(request))
    context = { 'query_string': query_string, 'metaagent_entries': metaagent_entries, 'history_entries' : history_entries , 'server_entries' : server_entries}
    return render(request, 'phrSite/search_results.html', context)


from django.views.decorators.http import condition

#@condition(etag_func=None)
#def stream_response(request):
#    #resp = HttpResponse( stream_response_generator(), mimetype='text/html')
#    resp = HttpResponse( stream_phr_output(), mimetype='text/html')
#    return resp
#
#def stream_response_generator():
#    yield "<html><body>\n"
#    for x in range(1,11):
#        yield "<div>%s</div>\n" % x
#        yield " " * 1024  # Encourage browser to render incrementally
#        time.sleep(1)
#    yield "</body></html>\n"
#
#def stream_phr_output():
#    p = pyPhr.phrApi()
#    yield "<html><body>\n"
#    yield " " * 1024  # Encourage browser to render incrementally
#    thr = threading.Thread(None, p.run, None, ("localhost", "7172",),{})
#    yield "after. Wait a bit..."
#    yield " " * 1024  # Encourage browser to render incrementally
#    for x in range(1,20):
#        time.sleep(1)
#    yield "I will stop it now"
#    yield " " * 1024  # Encourage browser to render incrementally
#    p.quit()
#    yield "I did it :-) " 
#    yield " " * 1024  # Encourage browser to render incrementally
#    yield "</body></html>\n"
#    


def console(request):
    context = {}
    return render(request, 'phrSite/console.html', context)

def metaAgentTreeO(request):
    context = {}
    return render(request, 'phrSite/metaAgentTree.html', context)


def makeListObj(agent):
    retStr = "<li><a href=\"%s\">%s</a>"%(reverse('metaAgent_detail', args=(agent.id,)), agent.name)
    children = agent.metaagenttree_parent.all()
    if len(children):
        retStr += "<ul>"
        sortedChildren = sorted(children, key = lambda x: (x.id_daughter.type, x.id_daughter.name))
        for child in sortedChildren:
            retStr += makeListObj(child.id_daughter)
        retStr += "</ul>" 
    retStr += ("</li>") 
    return retStr

#def makeListObj(agent):
#    retList = []
#    retList.append("<li>%s\n"%agent.name)
#    children = agent.metaagenttree_parent.all()
#    if len(children):
#        retList.append("<ul>\n")
#    for child in children:
#        for ret in makeListObj(child.id_daughter):
#            retList.append(ret)
#    if len(children):
#        retList.append("</ul>\n")
#    retList.append("</li>\n") 
#    return retList

def metaAgentTree(request):

    childrenSet = set([])
    allMaSet = set([])
    allMa = Metaagent.objects.all()
    
    for agent in allMa:
        if not agent.valid :
            continue
        classif = agent.classification
        if classif :
            #old classif system
            #if classif.split('-')[1] != "0":
            #new classif system
            if len(classif.split('-')) != 1:
                continue
        #if agent.type != "Coordinator":
        #    continue
        #we don't want Coord attached
        if agent.id_server: 
            continue
        allMaSet.add(agent)
        childrenAgent = agent.metaagenttree_parent.all()
        for child in childrenAgent:
            childrenSet.add(child.id_daughter)
    
    
    rootCoordinators = allMaSet - childrenSet
    
    retStr = ""
    retList = []
    sortedRoots = sorted(rootCoordinators, key=lambda x: x.name, reverse=False)

    for root in sortedRoots:
        retStr += "%s\n"%makeListObj(root)
    #l = list(rootCoordinators)
    #retStr += "%s\n"%makeListObjStr(l[0])
    #retList = makeListObj(l[0])

    context = {'maTree': retStr}
    return render(request, 'phrSite/metaAgentTree.html', context)
    #return HttpResponse(retStr)
#    print  "children %s" % children
#    print "The root coordinators are %s" % rootCoordinatorNames
#    rootCoordinators = []
#    for name in rootCoordinatorNames:
#        coord = self.__coordinatorDic[name]
#        if coord.isFullyDefined():
#        print "%s is fully defined so it is a real root" % name
#        rootCoordinators.append(self.__coordinatorDic[name])
#    
#    ltrs = string.uppercase
#    letters = [''.join(a) for a in ltrs]
#    letters += [''.join([a, b]) for a in ltrs for b in ltrs]
#    
#    
#    curLetterId = 0
#    curDepth = 0
#    for root in rootCoordinators:
#        curLetter = letters[curLetterId]
#        curDepth = 0
#    
#    
#        parents = [root]
#        while len(parents):
#        toDo = set([])
#    
#        classif = "%s-%s" % (curLetter, curDepth)
#        for p in parents:
#            p.setClassification(classif)
#            for child in p.getCoordinatorList():
#            toDo.add(child)
#   
#        #parents = []
#        parents = copy.copy(toDo)
#        toDo = []
#        curDepth += 1
#    
#        curLetterId += 1
#
#
#
#
#
#    treeDic = {}
#    allMa = Metaagent.objects.all()
#    for agent in allMa:
#        agentTree, agentLevel = '-'.split(agent.classification)
#        if agentTree not in treeDic:
#            treeDic[agentTree] = {}
#        tree = treeDic[agentTree]
#        if agentLevel not in tree:
#            tree[agentLevel] = []
#        tree[agentLevel].append(agent)
#
#    nestedList = []
#    for family in treeDic.values:
#        nestedList.append("<ul>")
#        for level in family.values:

 
    pass

#class ContactForm(forms.Form):
#    #subject = forms.CharField(max_length=100)
#    #message = forms.CharField()
#    content = forms.CharField()
#    #sender = forms.EmailField()
#    #cc_myself = forms.BooleanField(required=False)
#
#
#from django.shortcuts import render
#from django.http import HttpResponseRedirect
#
#def form(request, history_id):
#    rh = Recoveryhistory.objects.get(pk = history_id)
#    if request.method == 'POST': # If the form has been submitted...
#        print "Submitted!" 
#        form = ContactForm(request.POST) # A form bound to the POST data
#        if form.is_valid(): # All validation rules pass
#            print "I pass!"
#            newContent = form.cleaned_data['content']
#            print "The content %s"%newContent
#            rh.userentry = newContent
#            rh.save()
#
#            # Process the data in form.cleaned_data
#            # ...
#            #return HttpResponseRedirect('/thanks/') # Redirect after POST
#    #else:
#    form = ContactForm(initial = {'content' : rh.userentry}) # An unbound form
#
#    return render(request, 'phrSite/contact.html', {
#        'form': form, 'history_id' : history_id
#    })
#



class UpdateHistoryUserEntry(forms.Form):
    userEntry = forms.CharField(max_length = 1000, widget=forms.Textarea)

def history_detail(request, history_id):
    rh = Recoveryhistory.objects.get(pk = history_id)
    if request.method == 'POST': # If the form has been submitted...
        form = UpdateHistoryUserEntry(request.POST) # A form bound to the POST data
        if form.is_valid(): # All validation rules pass
            newUserEntry = form.cleaned_data['userEntry']
            rh.userentry = newUserEntry
            rh.save()

    form = UpdateHistoryUserEntry(initial = {'userEntry' : rh.userentry}) # An unbound form

    return render(request, 'phrSite/history_detail.html', {
        'form': form, 'recoveryhistory' : rh
    })



def dependency_validitySwap(request, dependency_id):
    dependency = Dependency.objects.get(pk = dependency_id)
    print "current validity ", dependency.validity 
    if (dependency.validity):
        dependency.validity = 0
    else :
        dependency.validity = 1
    print "new validity ", dependency.validity 
    dependency.save()

    #return render(request, 'phrSite/dependency.html')
    return HttpResponseRedirect('/phrSite/Dependency')
