Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;












insert into Server(id, name, address) values (1, 'localhost', 'localhost');


insert into Total (id) value (1);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (5,'A', 'Coordinator', 1, 'A-0',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (9,'E', 'Coordinator', 1, 'C-1',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (6,'D', 'Coordinator', 1, 'C-1',True, 0);
insert into Total (id) value (2);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (13,'G', 'Coordinator', 2, 'B-0',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (12,'F', 'Coordinator', 2, 'C-0',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (17,'E_G', 'Coordinator', 1, 'B-1',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (14,'D_G', 'Coordinator', 1, 'B-1',True, 0);




insert into MetaAgent (id, name, type, id_server, valid) values (4,'c', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 4, 'c', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (3,'b', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (2, 3, 'b', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (11,'c_E', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (3, 11, 'c', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (8,'c_D', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (4, 8, 'c', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (18,'bn_e_G', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (5, 18, 'b', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (19,'cn_e_G', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (6, 19, 'c', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (7,'b_D', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (7, 7, 'b', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (10,'b_E', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (8, 10, 'b', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (16,'cn_d_G', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (9, 16, 'c', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (2,'localhost_fstab', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (10, 2, '/etc/fstab', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (15,'bn_d_G', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (11, 15, 'b', '', '', '', '', '');






insert into MetaAgent (id, name, type, id_server, valid) values (1,'localhost_env', 'EnvironmentAgent', 1, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (1, 1, 10, 2, 100, 99);


insert into Occurrence(id) value (1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (1, 5, 3, 1);
insert into Occurrence(id) value (2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (2, 5, 4, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (3, 9, 10, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (4, 9, 11, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (5, 6, 7, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (6, 6, 8, 2);
insert into Occurrence(id) value (3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (7, 13, 14, 3);
insert into Occurrence(id) value (4);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (8, 13, 17, 4);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (9, 12, 6, 3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (10, 12, 9, 4);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (11, 17, 18, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (12, 17, 19, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (13, 14, 15, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (14, 14, 16, 2);




