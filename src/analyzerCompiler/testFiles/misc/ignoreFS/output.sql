Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;












insert into Server(id, name, address) values (1, 'test', 'dadada');
insert into Server(id, name, address) values (2, 'normal', 'normal');


insert into Total (id) value (1);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (6,'testC', 'Coordinator', 1, 'A-0',True, 0);




insert into MetaAgent (id, name, type, id_server, valid) values (5,'a', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 5, 'da', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (2,'test_fstab', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (2, 2, '/etc/fstab', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (4,'normal_fstab', 'FileAgent', 2, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (3, 4, '/etc/fstab', '', '', '', '', '');






insert into MetaAgent (id, name, type, id_server, valid) values (1,'test_env', 'EnvironmentAgent', 1, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (1, 1, 2, 2, 100, 99);
insert into IgnoredFS (id_envAgent, fsName) values (1, '/daq/da');
insert into IgnoredFS (id_envAgent, fsName) values (1, '/sw');
insert into MetaAgent (id, name, type, id_server, valid) values (3,'normal_env', 'EnvironmentAgent', 2, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (2, 3, 3, 2, 100, 99);


insert into Occurrence(id) value (1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (1, 6, 5, 1);




