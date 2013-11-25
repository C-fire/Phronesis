Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;












insert into Server(id, name, address) values (1, 'localhost', 'localhost');


insert into Total (id) value (1);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (6,'Papa', 'Coordinator', 1, 'A-0',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (5,'Papy', 'Coordinator', 1, 'B-0',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (9,'Son', 'Coordinator', 1, 'C-0',True, 0);


insert into MetaAgent (id, name, type, id_server, valid) values (10,'IwannaBe_Son', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 10, 'IwannaBe', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (8,'TheGuy_Papa', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (2, 8, 'TheGuy', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (7,'IwannaBe_Papa', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (3, 7, 'IwannaBe', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (4,'TheGuy', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (4, 4, 'TheGuy', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (2,'localhost_fstab', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (5, 2, '/etc/fstab', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (11,'TheGuy_Son', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (6, 11, 'TheGuy', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (3,'IwannaBe', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (7, 3, 'IwannaBe', '', '', '', '', '');






insert into MetaAgent (id, name, type, id_server, valid) values (1,'localhost_env', 'EnvironmentAgent', 1, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (1, 1, 5, 2, 100, 99);


insert into Occurrence(id) value (1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (1, 6, 7, 1);
insert into Occurrence(id) value (2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (2, 6, 8, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (3, 5, 3, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (4, 5, 4, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (5, 9, 10, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (6, 9, 11, 2);




