Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;












insert into Server(id, name, address) values (1, 'localhost', 'localhost');


insert into Total (id) value (7);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (5,'B_lvl1', 'Coordinator', 7, '0-1',True, 0);
insert into Total (id) value (5);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (6,'Top', 'Coordinator', 5, '0',True, 0);
insert into Total (id) value (6);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (4,'A_lvl1', 'Coordinator', 6, '0-0',True, 0);
insert into Total (id) value (3);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (3,'A_lvl2', 'Coordinator', 3, '0-0-0',True, 0);




insert into MetaAgent (id, name, type, id_server, valid) values (2,'localhost_fstab', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 2, '/etc/fstab', '', '', '', '', '');






insert into MetaAgent (id, name, type, id_server, valid) values (1,'localhost_env', 'EnvironmentAgent', 1, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (1, 1, 1, 2, 100, 99);


insert into Occurrence(id) value (2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (1, 6, 4, 2);
insert into Occurrence(id) value (3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (2, 6, 5, 3);
insert into Occurrence(id) value (1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (3, 4, 3, 1);




