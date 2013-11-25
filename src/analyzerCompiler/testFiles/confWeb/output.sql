Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;












insert into Server(id, name, address) values (1, 'NfsSrv', 'devexpert02');
insert into Server(id, name, address) values (2, 'WebSrv01', 'devexpert03');
insert into Server(id, name, address) values (3, 'WebSrv02', 'devexpert04');
insert into Server(id, name, address) values (4, 'MetaServer', 'None');
insert into Server(id, name, address) values (5, 'inst', '192.168.1.1');


insert into Total (id) value (10);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (43,'Web', 'Coordinator', 10, '0',True, 0);
insert into Total (id) value (3);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (36,'Site1Data', 'Coordinator', 3, '0-0-0',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (29,'Site2Data', 'Coordinator', 3, '0-1-0',True, 0);
insert into Total (id) value (1);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (31,'Site2', 'Coordinator', 1, '0-1',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (26,'Site', 'Coordinator', 1, 'None',False, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (38,'Site1', 'Coordinator', 1, '0-0',True, 0);
insert into Total (id) value (4);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (27,'Nfs', 'Coordinator', 4, '1',True, 0);
insert into Total (id) value (2);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (45,'Conf', 'Coordinator', 2, 'None',False, 0);
insert into Total (id) value (13);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (28,'Mysql', 'Coordinator', 13, '0-2',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (44,'Data', 'Coordinator', 3, 'None',False, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (37,'Site1Conf', 'Coordinator', 2, '0-0-1',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (30,'Site2Conf', 'Coordinator', 2, '0-1-1',True, 0);




insert into MetaAgent (id, name, type, id_server, valid) values (4,'WebSrv01_fstab', 'FileAgent', 2, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 4, '/etc/fstab', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (19,'exportFile', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (2, 19, '/etc/exports', '', '', '', '', '');
insert into MetaAgent (id, name, type, valid) values (8,'MetaServer_fstab', 'FileAgent',False);
insert into MetaAgent (id, name, type, id_server, valid) values (2,'NfsSrv_fstab', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (3, 2, '/etc/fstab', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (20,'s1Index', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (4, 20, '/devweb/site1/index.php', 'root', 'root', '', '', '827406447ac02d10842e4a9fa050d99e');
insert into MetaAgent (id, name, type, id_server, valid) values (24,'Site1Index', 'FileAgent', 2, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (5, 24, '/var/www/html/index.php', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (21,'s2Index', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (6, 21, '/devweb/site2/index.php', 'root', 'root', '', '', '6c88b56016412cb5c4736154a1bbe530');
insert into MetaAgent (id, name, type, id_server, valid) values (22,'Site2Index', 'FileAgent', 3, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (7, 22, '/var/www/html/index.php', '', '', '', '', '6c88b56016412cb5c4736154a1bbe530');
insert into FileContentRules (id_file, rule) values (7, 'This comes from the storage');
insert into MetaAgent (id, name, type, id_server, valid) values (6,'WebSrv02_fstab', 'FileAgent', 3, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (8, 6, '/etc/fstab', '', '', '', '', '');
insert into FileContentRules (id_file, rule) values (8, 'devexpert02:/devweb/site2/ /var/www/html/ nfs ro,hard,intr 0 0');
insert into MetaAgent (id, name, type, id_server, valid) values (25,'Site1CfgFile', 'FileAgent', 2, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (9, 25, '/etc/httpd/conf.d/httpd.conf', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (10,'inst_fstab', 'FileAgent', 5, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (10, 10, '/etc/fstab', '', '', '', '', '');
insert into FileContentRules (id_file, rule) values (10, 'fake mountpoint');
insert into MetaAgent (id, name, type, id_server, valid) values (23,'Site2CfgFile', 'FileAgent', 3, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (11, 23, '/etc/httpd/conf.d/httpd.conf', '', '', '', '', '');




insert into MetaAgent (id, name, type, valid) values (11,'MetaHttpd', 'ProcessAgent',False);
insert into MetaAgent (id, name, type, valid) values (13,'MetaHttdpChild', 'ProcessAgent',False);
insert into MetaAgent (id, name, type, id_server, valid) values (15,'nfsd', 'ProcessAgent', 1, True);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (1, 15, '*nfsd', 'root', -1, '', '', 100, 100);
insert into MetaAgent (id, name, type, id_server, valid) values (17,'mysqld', 'ProcessAgent', 3, True);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (2, 17, '/usr/bin/mysqld', 'None', -1, '', '', 100, 100);
insert into ProcessLimits(id, id_proc, id_limit, value) values (1, 2, 0, 5);
insert into MetaAgent (id, name, type, id_server, valid) values (32,'MetaHttpd_Site2', 'ProcessAgent', 3, True);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (3, 32, '/usr/sbin/httpd', 'root', -1, '', '', 100, 100);
insert into MetaAgent (id, name, type, id_server, valid) values (34,'MetaHttdpChild_c__Site2', 'ProcessAgent', 3, True);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (4, 34, '/usr/sbin/httpd', 'apache', -1, '', '', 100, 100);
update ProcessAgent set id_parent = 3 where id = 4;
insert into MetaAgent (id, name, type, id_server, valid) values (39,'MetaHttpd_Site1', 'ProcessAgent', 2, True);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (5, 39, '/usr/sbin/httpd', 'root', -1, '', '', 100, 100);
insert into MetaAgent (id, name, type, id_server, valid) values (41,'MetaHttdpChild_c__Site1', 'ProcessAgent', 2, True);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (6, 41, '/usr/sbin/httpd', 'apache', -1, '', '', 100, 100);
update ProcessAgent set id_parent = 5 where id = 6;


insert into MetaAgent (id, name, type, id_server, valid) values (3,'WebSrv01_env', 'EnvironmentAgent', 2, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (1, 3, 1, 2, 100, 99);
insert into MetaAgent (id, name, type, valid) values (7,'MetaServer_env', 'EnvironmentAgent',False);
insert into MetaAgent (id, name, type, id_server, valid) values (5,'WebSrv02_env', 'EnvironmentAgent', 3, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (2, 5, 8, 2, 100, 99);
insert into MetaAgent (id, name, type, id_server, valid) values (1,'NfsSrv_env', 'EnvironmentAgent', 1, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (3, 1, 3, 2, 100, 99);
insert into MetaAgent (id, name, type, id_server, valid) values (9,'inst_env', 'EnvironmentAgent', 5, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (4, 9, 10, 2, 100, 99);


insert into Occurrence(id) value (14);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (1, 43, 31, 14);
insert into Occurrence(id) value (13);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (2, 43, 38, 13);
insert into Occurrence(id) value (15);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (3, 43, 28, 15);
insert into Occurrence(id) value (11);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (4, 36, 24, 11);
insert into Occurrence(id) value (9);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (5, 29, 22, 9);
insert into Occurrence(id) value (3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (6, 31, 32, 3);
insert into Occurrence(id) value (2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (7, 31, 29, 2);
insert into Occurrence(id) value (1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (8, 31, 30, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (9, 26, 11, 3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (10, 26, 44, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (11, 26, 45, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (12, 38, 39, 3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (13, 38, 36, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (14, 38, 37, 1);
insert into Occurrence(id) value (6);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (15, 27, 20, 6);
insert into Occurrence(id) value (7);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (16, 27, 21, 7);
insert into Occurrence(id) value (5);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (17, 27, 19, 5);
insert into Occurrence(id) value (4);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (18, 27, 15, 4);
insert into Occurrence(id) value (8);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (19, 28, 17, 8);
insert into Occurrence(id) value (12);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (20, 37, 25, 12);
insert into Occurrence(id) value (10);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (21, 30, 23, 10);


insert into Dependency (id, id_prior, id_after, occurrence, validity) values (1, 28, 31, 0, 1);


insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (1, 22, 'RECOVERY_any', 32, 'something', 'after');
