Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;












insert into Server(id, name, address) values (1, 'localhost', 'localhost');


insert into Total (id) value (1);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (5,'Coord', 'Coordinator', 1, 'A-0',True, 0);


insert into MetaAgent (id, name, type, id_server, valid) values (3,'f1', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 3, 'f1', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (4,'f2', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (2, 4, 'f2', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (2,'localhost_fstab', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (3, 2, '/etc/fstab', '', '', '', '', '');






insert into MetaAgent (id, name, type, id_server, valid) values (1,'localhost_env', 'EnvironmentAgent', 1, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (1, 1, 3, 2, 100, 99);


insert into Occurrence(id) value (1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (1, 5, 3, 1);
insert into Occurrence(id) value (2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (2, 5, 4, 2);


insert into RecoveryHistory (date, time, responsible, userEntry) values ('14-12-1987','13:37', 'f1', 'a comment on this');
insert into RecoveryHistory_Action (id_recoveryHistory, metaAgent, action) values (1, 'f1', 'ACT_1'), (1, 'f2', 'ACT_2');
insert into RecoveryHistory_Problem (id_recoveryHistory, metaAgent, before_after) values (1, 'LeBefore', 'before'), (1, 'leAfter', 'after');

