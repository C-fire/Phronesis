Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;












insert into Server(id, name, address) values (1, 'localhost', 'localhost');


insert into Total (id) value (2);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (6,'A', 'Coordinator', 2, 'A-0',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (11,'E', 'Coordinator', 2, 'C-1',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (7,'D', 'Coordinator', 2, 'C-1',True, 0);
insert into Total (id) value (3);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (13,'u_E', 'Coordinator', 3, 'C-2',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (9,'u_D', 'Coordinator', 3, 'C-2',True, 0);
insert into Total (id) value (4);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (15,'F', 'Coordinator', 4, 'C-0',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (19,'u_0_G', 'Coordinator', 3, 'B-2',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (16,'G', 'Coordinator', 4, 'B-0',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (5,'u', 'Coordinator', 3, 'A-1',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (23,'u_1_G', 'Coordinator', 3, 'B-2',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (21,'E_G', 'Coordinator', 2, 'B-1',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (17,'D_G', 'Coordinator', 2, 'B-1',True, 0);




insert into MetaAgent (id, name, type, id_server, valid) values (4,'c', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 4, 'c', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (3,'b', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (2, 3, 'b', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (12,'c_E', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (3, 12, 'c', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (8,'c_D', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (4, 8, 'c', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (14,'b_0_E', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (5, 14, 'b', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (10,'b_0_D', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (6, 10, 'b', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (22,'c_1_G', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (7, 22, 'c', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (20,'b_0_0_G', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (8, 20, 'b', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (2,'localhost_fstab', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (9, 2, '/etc/fstab', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (18,'c_0_G', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (10, 18, 'c', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (24,'b_0_1_G', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (11, 24, 'b', '', '', '', '', '');






insert into MetaAgent (id, name, type, id_server, valid) values (1,'localhost_env', 'EnvironmentAgent', 1, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (1, 1, 9, 2, 100, 99);


insert into Occurrence(id) value (2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (1, 6, 5, 2);
insert into Occurrence(id) value (3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (2, 6, 4, 3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (3, 11, 13, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (4, 11, 12, 3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (5, 7, 9, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (6, 7, 8, 3);
insert into Occurrence(id) value (1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (7, 13, 14, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (8, 9, 10, 1);
insert into Occurrence(id) value (4);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (9, 15, 7, 4);
insert into Occurrence(id) value (5);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (10, 15, 11, 5);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (11, 19, 20, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (12, 16, 17, 4);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (13, 16, 21, 5);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (14, 5, 3, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (15, 23, 24, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (16, 21, 23, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (17, 21, 22, 3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (18, 17, 19, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (19, 17, 18, 3);




