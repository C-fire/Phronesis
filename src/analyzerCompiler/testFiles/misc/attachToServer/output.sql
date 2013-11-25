use phronesisAnalyzer;


Delete from FileContentRules;
Delete from FilenameFilter;
Delete from ProcessLimits;
Delete from RecoveryRule;
Delete from IgnoredFS;
Delete from Veto;














insert into Server(id, name, address) values (1, 'hops', 'hops');
insert into Server(id, name, address) values (2, 'abs', 'None');
insert into Server(id, name, address) values (3, 'localhost', 'localhost');


insert into Total (id) value (1);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (1,'EnvCoordb_on_hops', 'Coordinator', 1, '0',True, 0);
insert into Total (id) value (4);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (20,'TestCoord', 'Coordinator', 4, '1',True, 0);
insert into Total (id) value (3);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (3,'EnvCoord_on_localhost', 'Coordinator', 3, '2',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (8,'EnvCoord', 'Coordinator', 3, 'None',False, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (22,'EnvCoordb_on_localhost', 'Coordinator', 1, '3',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (7,'EnvCoord_on_hops', 'Coordinator', 3, '4',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (17,'EnvCoordb', 'Coordinator', 1, 'None',False, 0);


Update MetaAgent set id_server = 1 where id = 7;
Update MetaAgent set id_server = 1 where id = 1;
Update MetaAgent set id_server = 3 where id = 3;
Update MetaAgent set id_server = 3 where id = 22;


insert into MetaAgent (id, name, type, id_server, valid) values (2,'envVariables_fakeProc', 'FileAgent', 3, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 2, '/proc/[pid]/env', '', '', '', '', '');
insert into FileContentRules (id_file, rule) values (1, 'ORACLE_DE_MERDE=/dadada');
insert into MetaAgent (id, name, type, id_server, valid) values (18,'hops_fstab', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (2, 18, '/etc/fstab', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (21,'fb_EnvCoordb_on_hops', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (3, 21, 'anotherfileimportantforthesystem', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (10,'fb_EnvCoordb_on_localhost', 'FileAgent', 3, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (4, 10, 'anotherfileimportantforthesystem', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (11,'envVariables_realProc', 'FileAgent', 3, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (5, 11, '/proc/[pid]/env', '', '', '', '', '');
insert into FileContentRules (id_file, rule) values (5, 'ORACLE_DE_MERDE=/dadada');
insert into FileContentRules (id_file, rule) values (5, 'JE_CONFIRME');
insert into MetaAgent (id, name, type, valid) values (12,'fa', 'FileAgent',False);
insert into MetaAgent (id, name, type, valid) values (13,'fb', 'FileAgent',False);
insert into MetaAgent (id, name, type, id_server, valid) values (6,'fa_EnvCoord_on_hops', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (6, 6, 'afileimportantforthesystem', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (16,'localhost_fstab', 'FileAgent', 3, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (7, 16, '/etc/fstab', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (19,'fa_EnvCoord_on_localhost', 'FileAgent', 3, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (8, 19, 'afileimportantforthesystem', '', '', '', '', '');
insert into MetaAgent (id, name, type, valid) values (4,'abs_fstab', 'FileAgent',False);




insert into MetaAgent (id, name, type, id_server, valid) values (9,'fakeProc', 'ProcessAgent', 3, True);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (1, 9, 'somt', 'None', -1, '', '', 100, 100);
update ProcessAgent set id_procEnvFile = 1 where id = 1;
insert into ProcessLimits(id, id_proc, id_limit, value) values (1, 1, 0, 36);
insert into MetaAgent (id, name, type, id_server, valid) values (23,'realProc', 'ProcessAgent', 3, True);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (2, 23, 'somt', 'None', -1, '', '', 100, 100);
update ProcessAgent set id_procEnvFile = 5 where id = 2;
insert into MetaAgent (id, name, type, id_server, valid) values (24,'emptyProc', 'ProcessAgent', 3, True);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (3, 24, 'empyt', 'None', -1, '', '', 100, 100);


insert into MetaAgent (id, name, type, id_server, valid) values (5,'hops_env', 'EnvironmentAgent', 1, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (1, 5, 2, 2, 100, 99);
insert into MetaAgent (id, name, type, valid) values (14,'abs_env', 'EnvironmentAgent',False);
insert into MetaAgent (id, name, type, id_server, valid) values (15,'localhost_env', 'EnvironmentAgent', 3, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (2, 15, 7, 2, 100, 99);


insert into Occurrence(id) value (1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (1, 1, 21, 1);
insert into Occurrence(id) value (3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (6, 20, 24, 3);
insert into Occurrence(id) value (4);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (7, 20, 23, 4);
insert into Occurrence(id) value (2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (2, 3, 19, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (4, 8, 12, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (8, 22, 10, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (3, 7, 6, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (5, 17, 13, 1);




