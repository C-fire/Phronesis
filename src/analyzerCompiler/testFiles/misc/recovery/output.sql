Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;












insert into Server(id, name, address) values (1, 'localhost', 'localhost');


insert into Total (id) value (1);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (7,'X', 'Coordinator', 1, 'A-0',True, 0);




insert into MetaAgent (id, name, type, id_server, valid) values (6,'a', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 6, '/tmp/test/a', 'chaen', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (4,'c', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (2, 4, '/tmp/test/c', 'chaen', '', '', '', '');
insert into MetaAgent (id, name, type, valid) values (3,'abs', 'FileAgent',False);
insert into MetaAgent (id, name, type, id_server, valid) values (5,'b', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (3, 5, '/tmp/test/b', 'chaen', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (2,'localhost_fstab', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (4, 2, '/etc/fstab', '', '', '', '', '');






insert into MetaAgent (id, name, type, id_server, valid) values (1,'localhost_env', 'EnvironmentAgent', 1, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (1, 1, 4, 2, 100, 99);


insert into Occurrence(id) value (1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (1, 7, 6, 1);
insert into Occurrence(id) value (3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (2, 7, 4, 3);
insert into Occurrence(id) value (2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (3, 7, 5, 2);




insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (1, 6, 'RECOVERY_FILE_CREATE', 5, 'FILE_CHOWN', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (2, 6, 'RECOVERY_FILE_CHGRP', 5, 'FILE_CHOWN', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (3, 6, 'RECOVERY_FILE_CHOWN', 5, 'FILE_CHOWN', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (4, 6, 'RECOVERY_FILE_CHMOD', 5, 'FILE_CHOWN', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (5, 6, 'RECOVERY_FILE_CHATTR', 5, 'FILE_CHOWN', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (6, 6, 'RECOVERY_FILE_PATTERN', 5, 'FILE_CHOWN', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (7, 6, 'RECOVERY_FILE_MD5', 5, 'FILE_CHOWN', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (8, 5, 'RECOVERY_FILE_CREATE', 4, 'FILE_CHOWN', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (9, 5, 'RECOVERY_FILE_CHGRP', 4, 'FILE_CHOWN', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (10, 5, 'RECOVERY_FILE_CHOWN', 4, 'FILE_CHOWN', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (11, 5, 'RECOVERY_FILE_CHMOD', 4, 'FILE_CHOWN', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (12, 5, 'RECOVERY_FILE_CHATTR', 4, 'FILE_CHOWN', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (13, 5, 'RECOVERY_FILE_PATTERN', 4, 'FILE_CHOWN', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (14, 5, 'RECOVERY_FILE_MD5', 4, 'FILE_CHOWN', 'after');
