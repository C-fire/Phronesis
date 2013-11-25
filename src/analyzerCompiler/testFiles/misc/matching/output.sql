use phronesisAnalyzer;


Delete from FileContentRules;
Delete from FilenameFilter;
Delete from ProcessLimits;
Delete from RecoveryRule;
Delete from IgnoredFS;
Delete from Veto;




delete from MetaAgentTree where id = 18;
delete from MetaAgentTree where id = 3;
delete from MetaAgentTree where id = 19;
delete from MetaAgentTree where id = 7;
delete from MetaAgentTree where id = 8;
delete from MetaAgentTree where id = 6;
delete from MetaAgentTree where id = 17;
delete from MetaAgentTree where id = 5;
delete from MetaAgentTree where id = 1;
delete from MetaAgentTree where id = 2;


delete from FileAgent where id = 4;
delete from MetaAgent where id = 9;
delete from FileAgent where id = 9;
delete from MetaAgent where id = 12;
delete from MetaAgent where id = 18;
delete from MetaAgent where id = 19;
delete from MetaAgent where id = 20;
delete from ProcessAgent where id = 2;
delete from MetaAgent where id = 21;
delete from MetaAgent where id = 22;
delete from MetaAgent where id = 23;
delete from MetaAgent where id = 25;


delete from Total where id = 8;
delete from Total where id = 12;
delete from Total where id = 11;


delete from Occurrence where id = 5;
delete from Occurrence where id = 6;
delete from Occurrence where id = 7;
delete from Occurrence where id = 8;
delete from Occurrence where id = 9;
delete from Occurrence where id = 12;
delete from Occurrence where id = 13;




update Server set address = 'localhost' where id = 1;


insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (34,'Conf3', 'Coordinator', 3, 'C-1',True, 0);
update MetaAgent set classification = 'D-1' ,valid = True, tolerate = 0 where id = 14;
update MetaAgent set classification = 'None' ,valid = False, tolerate = 0 where id = 26;
insert into Total (id) value (14);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (29,'G', 'Coordinator', 14, 'B-0',True, 0);
update MetaAgent set classification = 'A-0' ,valid = True, tolerate = 0 where id = 24;
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (30,'Site3', 'Coordinator', 1, 'C-0',True, 0);
update MetaAgent set classification = 'D-0' ,valid = True, tolerate = 0 where id = 16;
update MetaAgent set classification = 'None' ,valid = False, tolerate = 0 where id = 13;
insert into Total (id) value (15);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (32,'U', 'Coordinator', 15, 'E-0',True, 0);
update MetaAgent set classification = 'D-1' ,valid = True, tolerate = 0 where id = 15;
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (28,'Data3', 'Coordinator', 2, 'C-1',True, 0);
update MetaAgent set classification = 'None' ,valid = False, tolerate = 0 where id = 27;




update MetaAgent set id_server = 1 where id = 5;
update FileAgent set filename = 'f1', owner = '', grp = '', permissions = '', attributes = '', md5 = '' where id =1;
update MetaAgent set id_server = 1 where id = 7;
update FileAgent set filename = 'f3', owner = '', grp = '', permissions = '', attributes = '', md5 = '' where id =2;
update MetaAgent set id_server = 1 where id = 8;
update FileAgent set filename = 'f4', owner = '', grp = '', permissions = '', attributes = '', md5 = '' where id =3;
insert into MetaAgent (id, name, type, id_server, valid) values (35,'F5', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (10, 35, 'f5', '', '', '', '', '');
update MetaAgent set id_server = 1 where id = 11;
update FileAgent set filename = 'F', owner = '', grp = '', permissions = '', attributes = '', md5 = '' where id =5;
update MetaAgent set id_server = 1 where id = 6;
update FileAgent set filename = 'f2', owner = '', grp = '', permissions = '', attributes = '', md5 = '' where id =6;
update MetaAgent set id_server = 1 where id = 2;
update FileAgent set filename = '/etc/fstab', owner = '', grp = '', permissions = '', attributes = '', md5 = '' where id =7;
update MetaAgent set id_server = 1 where id = 10;
update FileAgent set filename = 'E', owner = '', grp = '', permissions = '', attributes = '', md5 = '' where id =8;


insert into MetaAgent (id, name, type, id_server, valid) values (33,'V', 'FolderAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5, maxDepth) values (11, 33, 'V', '', '', '', '', '', 0);


update MetaAgent set id_server = 1 where id = 17;
update ProcessAgent set procName = '/usr/sbin/httpd', user = 'root', multiplicity = -1, service = '', command = '', maxCpu = 100, maxMemory = 100 where id = 1;
insert into MetaAgent (id, name, type, id_server, valid) values (31,'MetaHttpd_Site3', 'ProcessAgent', 1, True);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (3, 31, '/usr/sbin/httpd', 'root', -1, '', '', 100, 100);


update MetaAgent set id_server = 1 where id = 1;
update EnvironmentAgent set maxLoad = 2, maxMemory = 100, maxSwap = 99 where id =1;


insert into Occurrence(id) value (18);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (27, 34, 8, 18);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (25, 30, 31, 3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (23, 30, 34, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (24, 30, 28, 1);
insert into Occurrence(id) value (17);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (26, 32, 33, 17);
insert into Occurrence(id) value (15);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (21, 15, 7, 15);
insert into Occurrence(id) value (14);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (20, 15, 6, 14);
insert into Occurrence(id) value (16);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (22, 28, 35, 16);


insert into Dependency (id, id_prior, id_after, occurrence, validity) values (1, 24, 29, 0, 1);


insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (1, 6, 'RECOVERY_FILE_CREATE', 5, 'FILE_CHOWN', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (2, 6, 'RECOVERY_FILE_CHGRP', 5, 'FILE_CHOWN', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (3, 6, 'RECOVERY_FILE_CHOWN', 5, 'FILE_CHOWN', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (4, 6, 'RECOVERY_FILE_CHMOD', 5, 'FILE_CHOWN', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (5, 6, 'RECOVERY_FILE_CHATTR', 5, 'FILE_CHOWN', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (6, 6, 'RECOVERY_FILE_PATTERN', 5, 'FILE_CHOWN', 'after');
insert into RecoveryRule (id, id_ma1, action1, id_ma2, action2, direction) values (7, 6, 'RECOVERY_FILE_MD5', 5, 'FILE_CHOWN', 'after');
