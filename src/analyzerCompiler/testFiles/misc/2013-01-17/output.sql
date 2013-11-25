Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;
insert into Total(id) value (1);
insert into Total(id) value (2);
insert into Total(id) value (3);
insert into Total(id) value (4);
insert into Total(id) value (5);


insert into Occurrence(id) value (1);
insert into Occurrence(id) value (2);
insert into Occurrence(id) value (3);
insert into Occurrence(id) value (4);


insert into Server(id, name, address) values (1, 'localhost', 'localhost');


insert into MetaAgent (id, name, type, id_total, classification,valid) values (1,'Lvl1_MyLvl', 'Coordinator', 5, 'A-1',True);
insert into MetaAgent (id, name, type, id_total, classification,valid) values (2,'Lvl2_MyLvl', 'Coordinator', 3, 'A-2',True);
insert into MetaAgent (id, name, type, id_total, classification,valid) values (3,'MyLvl', 'Coordinator', 4, 'A-0',True);
insert into MetaAgent (id, name, type, id_total, classification,valid) values (4,'Lvl2', 'Coordinator', 3, 'None',False);
insert into MetaAgent (id, name, type, id_total, classification,valid) values (5,'Lvl0', 'Coordinator', 4, 'None',False);
insert into MetaAgent (id, name, type, id_total, classification,valid) values (6,'Lvl1', 'Coordinator', 5, 'None',False);


insert into MetaAgent (id, name, type, valid) values (7,'f1', 'FileAgent',False);
insert into MetaAgent (id, name, type, valid) values (8,'f2', 'FileAgent',False);
insert into MetaAgent (id, name, type, id_server, valid) values (9,'f2_MyLvl', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 9, 'f2', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (10,'f1_MyLvl', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (2, 10, 'f1', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (11,'localhost_fstab', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (3, 11, '/etc/fstab', '', '', '', '', '');




insert into MetaAgent (id, name, type, id_server, valid) values (12,'localhost_env', 'EnvironmentAgent', 1, True);
insert into EnvironmentAgent (id, id_metaAgent) values (1, 12);


insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (1, 1, 10, 3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (2, 1, 2, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (3, 2, 9, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (4, 3, 1, 4);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (5, 4, 8, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (6, 5, 6, 4);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (7, 6, 7, 3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (8, 6, 4, 2);




