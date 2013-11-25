Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;
insert into Total(id) value (1);
insert into Total(id) value (2);
insert into Total(id) value (3);
insert into Total(id) value (4);
insert into Total(id) value (5);


insert into Occurrence(id) value (1);
insert into Occurrence(id) value (2);
insert into Occurrence(id) value (3);
insert into Occurrence(id) value (4);
insert into Occurrence(id) value (5);


insert into Server(id, name, address) values (1, 'localhost', 'localhost');


insert into MetaAgent (id, name, type, id_total, classification) values (1,'A', 'Coordinator', 1, 'A-0');
insert into MetaAgent (id, name, type, id_total, classification) values (2,'confCoord', 'Coordinator', 5, 'B-0');
insert into MetaAgent (id, name, type, id_total, classification) values (3,'C', 'Coordinator', 3, 'C-0');
insert into MetaAgent (id, name, type, id_total, classification) values (4,'B', 'Coordinator', 2, 'D-0');
insert into MetaAgent (id, name, type, id_total, classification) values (5,'D', 'Coordinator', 4, 'E-0');


insert into MetaAgent (id, name, type, id_server) values (6,'a', 'FileAgent', 1);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 6, '/tmp/bonjour.jpg', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server) values (7,'localhost_fstab', 'FileAgent', 1);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (2, 7, '/etc/fstab', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server) values (8,'confFolder', 'FolderAgent', 1);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (3, 8, '/tmp/testFolder/', 'chaen', '', '', '', '');


insert into MetaAgent (id, name, type, id_server) values (9,'b', 'ProcessAgent', 1);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (1, 9, 'whoCares', 'None', -1, '', '', 101, 200);


insert into MetaAgent (id, name, type, id_server) values (10,'localhost_env', 'EnvironmentAgent', 1);
insert into EnvironmentAgent (id, id_metaAgent) values (1, 10);


insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (1, 1, 6, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (2, 2, 8, 5);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (3, 3, 6, 3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (4, 4, 6, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (5, 5, 9, 4);


insert into Rule (id, id_prior, id_after, occurrence, validity) values (1, 1, 3, 0, 1);
insert into Rule (id, id_prior, id_after, occurrence, validity) values (2, 4, 3, 0, 1);


insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (1, 8, 'RECOVERY_FILE_CREATE', 9, 'PROCESS_RESTART', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (2, 8, 'RECOVERY_FILE_CHGRP', 9, 'PROCESS_RESTART', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (3, 8, 'RECOVERY_FILE_CHOWN', 9, 'PROCESS_RESTART', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (4, 8, 'RECOVERY_FILE_CHMOD', 9, 'PROCESS_RESTART', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (5, 8, 'RECOVERY_FILE_CHATTR', 9, 'PROCESS_RESTART', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (6, 8, 'RECOVERY_FILE_PATTERN', 9, 'PROCESS_RESTART', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (7, 8, 'RECOVERY_FILE_MD5', 9, 'PROCESS_RESTART', 'after');
