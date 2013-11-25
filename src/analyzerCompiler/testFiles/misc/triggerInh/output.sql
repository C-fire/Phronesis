Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;












insert into Server(id, name, address) values (1, 'localhost', 'localhost');


insert into Total (id) value (1);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (7,'abs_Nfs', 'Coordinator', 1, 'None',False, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (12,'realNfs2', 'Coordinator', 1, '0',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (8,'realNfs', 'Coordinator', 1, '1',True, 0);




insert into MetaAgent (id, name, type, id_server, valid) values (13,'abs_exportFile_realNfs2', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 13, '/etc/exports', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (2,'localhost_fstab', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (2, 2, '/etc/fstab', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (6,'real_exportFile', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (3, 6, '/etc/exports', '', '', '', '', '');
insert into FileContentRules (id_file, rule) values (3, '/exportedFolder *(ro,root_squash,no_wdelay,fsid=1)');
insert into MetaAgent (id, name, type, valid) values (5,'abs_exportFile', 'FileAgent',False);




insert into MetaAgent (id, name, type, valid) values (3,'abs_nfsd', 'ProcessAgent',False);
insert into MetaAgent (id, name, type, id_server, valid) values (10,'abs_nfsd_realNfs', 'ProcessAgent', 1, True);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (1, 10, 'nfsd', 'None', -1, '', '', 100, 100);
insert into MetaAgent (id, name, type, id_server, valid) values (14,'abs_nfsd_realNfs2', 'ProcessAgent', 1, True);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (2, 14, 'nfsd', 'None', -1, '', '', 100, 100);


insert into MetaAgent (id, name, type, id_server, valid) values (1,'localhost_env', 'EnvironmentAgent', 1, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (1, 1, 2, 2, 100, 99);


insert into Occurrence(id) value (2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (1, 7, 5, 2);
insert into Occurrence(id) value (1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (2, 7, 3, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (3, 12, 13, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (4, 12, 14, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (5, 8, 6, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (6, 8, 10, 1);




insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (1, 13, 'RECOVERY_FILE_CREATE', 14, 'PROCESS_RESTART', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (2, 13, 'RECOVERY_FILE_CHGRP', 14, 'PROCESS_RESTART', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (3, 13, 'RECOVERY_FILE_CHOWN', 14, 'PROCESS_RESTART', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (4, 13, 'RECOVERY_FILE_CHMOD', 14, 'PROCESS_RESTART', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (5, 13, 'RECOVERY_FILE_CHATTR', 14, 'PROCESS_RESTART', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (6, 13, 'RECOVERY_FILE_PATTERN', 14, 'PROCESS_RESTART', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (7, 13, 'RECOVERY_FILE_MD5', 14, 'PROCESS_RESTART', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (8, 6, 'RECOVERY_FILE_CREATE', 3, 'PROCESS_RESTART', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (9, 6, 'RECOVERY_FILE_CHGRP', 3, 'PROCESS_RESTART', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (10, 6, 'RECOVERY_FILE_CHOWN', 3, 'PROCESS_RESTART', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (11, 6, 'RECOVERY_FILE_CHMOD', 3, 'PROCESS_RESTART', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (12, 6, 'RECOVERY_FILE_CHATTR', 3, 'PROCESS_RESTART', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (13, 6, 'RECOVERY_FILE_PATTERN', 3, 'PROCESS_RESTART', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (14, 6, 'RECOVERY_FILE_MD5', 3, 'PROCESS_RESTART', 'after');
