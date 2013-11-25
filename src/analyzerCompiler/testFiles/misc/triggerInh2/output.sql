Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;












insert into Server(id, name, address) values (1, 'localhost', 'localhost');


insert into Total (id) value (2);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (6,'A', 'Coordinator', 2, 'None',False, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (7,'real1', 'Coordinator', 2, '0',True, 0);
insert into Total (id) value (3);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (5,'B', 'Coordinator', 3, 'None',False, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (8,'B_real1', 'Coordinator', 3, '0-0',True, 0);




insert into MetaAgent (id, name, type, valid) values (4,'y', 'FileAgent',False);
insert into MetaAgent (id, name, type, valid) values (3,'x', 'FileAgent',False);
insert into MetaAgent (id, name, type, id_server, valid) values (9,'x_0_real1', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 9, 'x', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (10,'y_0_real1', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (2, 10, 'y', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (2,'localhost_fstab', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (3, 2, '/etc/fstab', '', '', '', '', '');






insert into MetaAgent (id, name, type, id_server, valid) values (1,'localhost_env', 'EnvironmentAgent', 1, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (1, 1, 3, 2, 100, 99);


insert into Occurrence(id) value (3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (1, 6, 5, 3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (2, 7, 8, 3);
insert into Occurrence(id) value (1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (3, 5, 3, 1);
insert into Occurrence(id) value (2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (4, 5, 4, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (5, 8, 9, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (6, 8, 10, 2);




insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (1, 10, 'RECOVERY_FILE_CREATE', 9, 'FILE_CREATE', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (2, 10, 'RECOVERY_FILE_CHGRP', 9, 'FILE_CREATE', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (3, 10, 'RECOVERY_FILE_CHOWN', 9, 'FILE_CREATE', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (4, 10, 'RECOVERY_FILE_CHMOD', 9, 'FILE_CREATE', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (5, 10, 'RECOVERY_FILE_CHATTR', 9, 'FILE_CREATE', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (6, 10, 'RECOVERY_FILE_PATTERN', 9, 'FILE_CREATE', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (7, 10, 'RECOVERY_FILE_MD5', 9, 'FILE_CREATE', 'after');
