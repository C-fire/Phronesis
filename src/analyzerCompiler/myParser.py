# 
# dhcpd_leases_parser.py
#
# Copyright 2008, Paul McGuire
#
# Sample parser to parse a dhcpd.leases file to extract leases 
# and lease attributes
#
# format ref: http://www.linuxmanpages.com/man5/dhcpd.leases.5.php
#

srvTest="""
Server WebSrv01 {
        address -> 192.168.30.6;
	location ->dtc;
}
Server WebSrv02 {
        address -> devexpert02;
	location ->dtc2;
}

"""
procTest="""
ProcessAgent MetaHttpd {
        procName -> httpd;
	user -> root;
	server -> myserver;
}
ProcessAgent MetaHttpd2 : MetaHttpd{
        procName -> httpd2;
	user -> root2;
	server -> myserver;
}
"""

from pyparsing import *
import sys
#################### WORKING

LBRACE,RBRACE,SEMI,QUOTE,COLUMN = map(Suppress,'{};":')
ASSIGN=Suppress("->")
wrong_attributes = Word(alphanums) + ASSIGN + Word(alphanums) + SEMI
myPrintable = printables.strip('\n{}/')

ipAddress = Combine(Word(nums) + ('.' + Word(nums))*3)
dnsAddress = Word(alphanums)
address =  Or([ dnsAddress, ipAddress] )
location = Word(alphanums)
serverName = Word(alphanums)
server_start = "Server" + serverName("serverName") + LBRACE
server_address = "address" + ASSIGN + address("address") + SEMI
server_location = "location" + ASSIGN + location("location") + SEMI
server_attributes = Or([server_location, server_address, wrong_attributes])
server_end = RBRACE
serverDef = server_start + OneOrMore(server_attributes) + server_end

def errorMsg(st,locn,tokens):
    print 'Unknown attribute %s at line %d'%(tokens[0],lineno(locn,st))
    sys.exit(1)

wrong_attributes.setParseAction(errorMsg)

#for srv in serverDef.searchString(srvTest):
#    print 'dump ', srv.dump()
#    print srv.serverName," has address ", srv.address
#    print srv.serverName ," has location ", srv.location
#    print


agentName = Word(alphanums)
procName = Word(alphanums)
user = Word(alphanums)
procServer = Word(alphanums)
process_inheritance = COLUMN + agentName("motherAgent")
process_start = "ProcessAgent" + agentName("agentName") + Optional(process_inheritance) + LBRACE
process_procName = "procName" + ASSIGN + procName("procName") + SEMI
process_user = "user" + ASSIGN + user("user") + SEMI
process_server = "server" + ASSIGN + procServer("server") + SEMI
process_attributes = Or([process_procName, process_user, process_server])

process_end = RBRACE

processDef = process_start +  OneOrMore(process_attributes) +  process_end


def haveInheritance(tok):
    print "Youhooo, inheritance : ",tok

process_inheritance.setParseAction(haveInheritance)

def parseAction_process_start(token):
    print "I am creating a process Agent ",token
process_start.setParseAction(parseAction_process_start)

for proc in processDef.searchString(procTest):
    print 'dump ', proc
    print proc.agentName," has procName ", proc.procName
    print proc.agentName ," has server ", proc.server
    print proc.agentName ," has user ", proc.user
    print


######################################################################
