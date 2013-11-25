Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;












insert into Server(id, name, address) values (1, 'localhost', 'localhost');


insert into Total (id) value (-1);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (7,'a', 'Coordinator', -1, 'None',False, 0);
insert into Total (id) value (1);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (4,'PapC', 'Coordinator', 1, 'None',False, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (5,'Papb', 'Coordinator', 1, 'None',False, 0);


insert into MetaAgent (id, name, type, id_server, valid) values (3,'fa', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 3, 'fa', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (2,'localhost_fstab', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (2, 2, '/etc/fstab', '', '', '', '', '');






insert into MetaAgent (id, name, type, id_server, valid) values (1,'localhost_env', 'EnvironmentAgent', 1, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (1, 1, 2, 2, 100, 99);


insert into Occurrence(id) value (1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (1, 4, 7, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (2, 5, 7, 1);




