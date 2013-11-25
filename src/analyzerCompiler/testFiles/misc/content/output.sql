Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;












insert into Server(id, name, address) values (1, 'localhost', 'localhost');






insert into MetaAgent (id, name, type, valid) values (3,'test', 'FileAgent',False);
insert into MetaAgent (id, name, type, id_server, valid) values (2,'localhost_fstab', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 2, '/etc/fstab', '', '', '', '', '');






insert into MetaAgent (id, name, type, id_server, valid) values (1,'localhost_env', 'EnvironmentAgent', 1, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (1, 1, 1, 2, 100, 99);






