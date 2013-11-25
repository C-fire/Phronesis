Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;












insert into Server(id, name, address) values (1, 'localhost', 'localhost');


insert into Total (id) value (1);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (4,'A', 'Coordinator', 1, 'A-0',True, 0);
insert into Veto (idCoord, name) value (4, 'x');
insert into Total (id) value (2);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (5,'X', 'Coordinator', 2, 'B-0',True, 0);
insert into Total (id) value (4);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (6,'C', 'Coordinator', 4, 'C-1',True, 0);
insert into Veto (idCoord, name) value (6, 'u');
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (7,'B', 'Coordinator', 1, 'C-0',True, 0);
insert into Veto (idCoord, name) value (7, 'x');
insert into Veto (idCoord, name) value (7, 'y_y');




insert into MetaAgent (id, name, type, id_server, valid) values (3,'fa', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 3, 'dada', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (2,'localhost_fstab', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (2, 2, '/etc/fstab', '', '', '', '', '');






insert into MetaAgent (id, name, type, id_server, valid) values (1,'localhost_env', 'EnvironmentAgent', 1, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (1, 1, 2, 2, 100, 99);


insert into Occurrence(id) value (1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (1, 6, 3, 1);
insert into Occurrence(id) value (2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (2, 7, 6, 2);




