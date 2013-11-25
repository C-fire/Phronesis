Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;












insert into Server(id, name, address) values (1, 'localhost', 'localhost');






insert into MetaAgent (id, name, type, id_server, valid) values (2,'localhost_fstab', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 2, '/etc/fstab', '', '', '', '', '');




insert into MetaAgent (id, name, type, id_server, valid) values (3,'parentAbs', 'ProcessAgent', 1, True);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (1, 3, 'dada', 'None', -1, '', '', 100, 100);
insert into MetaAgent (id, name, type, id_server, valid) values (5,'childAbs', 'ProcessAgent', 1, True);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (2, 5, 'dada', 'None', -1, '', '', 100, 100);
update ProcessAgent set id_parent = 1 where id = 2;
insert into MetaAgent (id, name, type, id_server, valid) values (7,'realParent', 'ProcessAgent', 1, True);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (3, 7, 'dada', 'None', -1, '', '', 100, 100);


insert into MetaAgent (id, name, type, id_server, valid) values (1,'localhost_env', 'EnvironmentAgent', 1, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (1, 1, 1, 2, 100, 99);






