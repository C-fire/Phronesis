Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;












insert into Server(id, name, address) values (1, 'absServer', 'None');
insert into Server(id, name, address) values (2, 'realServer1', 'dada');
insert into Server(id, name, address) values (3, 'realServer2', 'dada');






insert into MetaAgent (id, name, type, valid) values (7,'X', 'FileAgent',False);
insert into MetaAgent (id, name, type, id_server, valid) values (4,'realServer1_fstab', 'FileAgent', 2, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 4, '/etc/fstab', '', '', '', '', '');
insert into MetaAgent (id, name, type, valid) values (2,'absServer_fstab', 'FileAgent',False);
insert into MetaAgent (id, name, type, id_server, valid) values (6,'realServer2_fstab', 'FileAgent', 3, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (2, 6, '/etc/fstab', '', '', '', '', '');






insert into MetaAgent (id, name, type, id_server, valid) values (3,'realServer1_env', 'EnvironmentAgent', 2, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (1, 3, 1, 2, 100, 99);
insert into MetaAgent (id, name, type, id_server, valid) values (5,'realServer2_env', 'EnvironmentAgent', 3, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (2, 5, 2, 2, 100, 99);
insert into MetaAgent (id, name, type, valid) values (1,'absServer_env', 'EnvironmentAgent',False);






