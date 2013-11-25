Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;












insert into Server(id, name, address) values (1, 'localhost', 'localhost');


insert into Total (id) value (2);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (6,'detached', 'Coordinator', 2, 'A-0',True, 0);
insert into Total (id) value (1);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (5,'Attached', 'Coordinator', 1, 'B-0',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (7,'Attached_on_localhost', 'Coordinator', 1, 'C-0',True, 0);


Update MetaAgent set id_server = 1 where id = 7;


insert into MetaAgent (id, name, type, id_server, valid) values (3,'a', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 3, 'a', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (8,'a_Attached_on_localhost', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (2, 8, 'a', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (4,'b', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (3, 4, 'b', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (2,'localhost_fstab', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (4, 2, '/etc/fstab', '', '', '', '', '');






insert into MetaAgent (id, name, type, id_server, valid) values (1,'localhost_env', 'EnvironmentAgent', 1, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (1, 1, 4, 2, 100, 99);


insert into Occurrence(id) value (2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (1, 6, 4, 2);
insert into Occurrence(id) value (1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (2, 5, 3, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (3, 7, 8, 1);




