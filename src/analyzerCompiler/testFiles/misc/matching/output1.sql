Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;












insert into Server(id, name, address) values (1, 'localhost', 'localhost');


insert into Total (id) value (11);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (25,'A', 'Coordinator', 11, 'A-0',True, 0);
insert into Total (id) value (3);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (18,'Conf2', 'Coordinator', 3, 'C-1',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (14,'Conf1', 'Coordinator', 3, 'D-1',True, 0);
insert into Total (id) value (12);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (23,'B', 'Coordinator', 12, 'A-1',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (26,'MetaConf', 'Coordinator', 3, 'None',False, 0);
insert into Total (id) value (1);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (20,'Site2', 'Coordinator', 1, 'C-0',True, 0);
insert into Total (id) value (13);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (24,'C', 'Coordinator', 13, 'A-1',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (16,'Site1', 'Coordinator', 1, 'D-0',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (13,'MetaSite', 'Coordinator', 1, 'None',False, 0);
insert into Total (id) value (8);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (22,'X', 'Coordinator', 8, 'B-0',True, 0);
insert into Total (id) value (2);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (27,'MetaData', 'Coordinator', 2, 'None',False, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (15,'Data1', 'Coordinator', 2, 'D-1',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (19,'Data2', 'Coordinator', 2, 'C-1',True, 0);




insert into MetaAgent (id, name, type, id_server, valid) values (5,'F1', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 5, 'f1', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (7,'F3', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (2, 7, 'f3', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (8,'F4', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (3, 8, 'f4', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (9,'D', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (4, 9, 'D', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (11,'F', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (5, 11, 'F', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (6,'F2', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (6, 6, 'f2', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (2,'localhost_fstab', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (7, 2, '/etc/fstab', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (10,'E', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (8, 10, 'E', '', '', '', '', '');


insert into MetaAgent (id, name, type, id_server, valid) values (12,'Y', 'FolderAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5, maxDepth) values (9, 12, 'Y', '', '', '', '', '', 0);


insert into MetaAgent (id, name, type, valid) values (3,'MetaHttpd', 'ProcessAgent',False);
insert into MetaAgent (id, name, type, id_server, valid) values (17,'MetaHttpd_Site1', 'ProcessAgent', 1, True);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (1, 17, '/usr/sbin/httpd', 'root', -1, '', '', 100, 100);
insert into MetaAgent (id, name, type, id_server, valid) values (21,'MetaHttpd_Site2', 'ProcessAgent', 1, True);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (2, 21, '/usr/sbin/httpd', 'root', -1, '', '', 100, 100);


insert into MetaAgent (id, name, type, id_server, valid) values (1,'localhost_env', 'EnvironmentAgent', 1, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (1, 1, 7, 2, 100, 99);


insert into Occurrence(id) value (12);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (1, 25, 23, 12);
insert into Occurrence(id) value (13);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (2, 25, 24, 13);
insert into Occurrence(id) value (6);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (3, 18, 7, 6);
insert into Occurrence(id) value (4);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (4, 14, 5, 4);
insert into Occurrence(id) value (9);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (5, 23, 9, 9);
insert into Occurrence(id) value (3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (6, 20, 21, 3);
insert into Occurrence(id) value (2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (7, 20, 18, 2);
insert into Occurrence(id) value (1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (8, 20, 19, 1);
insert into Occurrence(id) value (10);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (9, 24, 10, 10);
insert into Occurrence(id) value (11);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (10, 24, 11, 11);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (11, 16, 17, 3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (12, 16, 14, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (13, 16, 15, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (14, 13, 3, 3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (15, 13, 26, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (16, 13, 27, 1);
insert into Occurrence(id) value (8);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (17, 22, 12, 8);
insert into Occurrence(id) value (5);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (18, 15, 6, 5);
insert into Occurrence(id) value (7);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (19, 19, 8, 7);




