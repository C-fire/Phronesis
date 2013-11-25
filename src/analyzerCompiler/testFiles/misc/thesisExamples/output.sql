Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;












insert into Server(id, name, address) values (2, 'srv1', '192.168.0.50');
insert into Server(id, name, address) values (1, 'abs_srv', 'None');
insert into Server(id, name, address) values (3, 'srv2', '192.168.0.51');


insert into Total (id) value (2);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (35,'default_Data_Site2', 'Coordinator', 2, '0-0',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (40,'n_c_data', 'Coordinator', 2, 'None',False, 0);
insert into Total (id) value (1);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (30,'Site2', 'Coordinator', 1, '0',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (13,'abs_webSite', 'Coordinator', 1, 'None',False, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (21,'Site1', 'Coordinator', 1, '1',True, 0);
insert into Total (id) value (3);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (15,'default_Conf', 'Coordinator', 3, 'None',False, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (39,'n_c_conf', 'Coordinator', 3, 'None',False, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (26,'default_Data_Site1', 'Coordinator', 2, '1-0',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (16,'default_webSite', 'Coordinator', 1, 'None',False, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (28,'default_Conf_Site1', 'Coordinator', 3, '1-1',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (37,'default_Conf_Site2', 'Coordinator', 3, '0-1',True, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (14,'default_Data', 'Coordinator', 2, 'None',False, 0);




insert into MetaAgent (id, name, type, id_server, valid) values (10,'envVariables_abs_httdpChild_c__Site1', 'FileAgent', 2, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 10, '/proc/[pid]/env', '', '', '', '', '');
insert into FileContentRules (id_file, rule) values (1, 'ORACLE_LIB=/usr/lib/oracle/');
insert into MetaAgent (id, name, type, id_server, valid) values (10,'envVariables_abs_httdpChild_c__Site2', 'FileAgent', 3, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (2, 10, '/proc/[pid]/env', '', '', '', '', '');
insert into FileContentRules (id_file, rule) values (2, 'ORACLE_LIB=/usr/lib/oracle/');
insert into MetaAgent (id, name, type, id_server, valid) values (4,'srv1_fstab', 'FileAgent', 2, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (3, 4, '/etc/fstab', '', '', '', '', '');
insert into FileContentRules (id_file, rule) values (3, 'nfs:/nfsexports/sharedFiles /sharedFiles nfs rw,hard,intr,nolock 0 0');
insert into FileContentRules (id_file, rule) values (3, 'nfs:/nfsexports/otherFiles /otherFiles nfs rw,hard,intr 0 0');
insert into MetaAgent (id, name, type, valid) values (10,'envVariables_abs_httdpChild_c__default_webSite', 'FileAgent',False);
insert into MetaAgent (id, name, type, valid) values (2,'abs_srv_fstab', 'FileAgent',False);
insert into MetaAgent (id, name, type, valid) values (10,'envVariables_abs_httdpChild', 'FileAgent',False);
insert into MetaAgent (id, name, type, id_server, valid) values (6,'srv2_fstab', 'FileAgent', 3, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (4, 6, '/etc/fstab', '', '', '', '', '');
insert into FileContentRules (id_file, rule) values (4, 'nfs:/nfsexports/sharedFiles /sharedFiles nfs rw,hard,intr,nolock 0 0');
insert into FileContentRules (id_file, rule) values (4, 'nfs:/nfsexports/otherFiles /otherFiles nfs rw,hard,intr 0 0');
insert into MetaAgent (id, name, type, valid) values (11,'default_httpdConf', 'FileAgent',False);
insert into MetaAgent (id, name, type, valid) values (8,'envVariables_abs_httpd', 'FileAgent',False);
insert into MetaAgent (id, name, type, id_server, valid) values (34,'envVariables_abs_httpd_Site2', 'FileAgent', 3, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (5, 34, '/proc/[pid]/env', '', '', '', '', '');
insert into FileContentRules (id_file, rule) values (5, 'ORACLE_LIB=/usr/lib/oracle/');
insert into MetaAgent (id, name, type, id_server, valid) values (25,'envVariables_abs_httpd_Site1', 'FileAgent', 2, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (6, 25, '/proc/[pid]/env', '', '', '', '', '');
insert into FileContentRules (id_file, rule) values (6, 'ORACLE_LIB=/usr/lib/oracle/');
insert into MetaAgent (id, name, type, id_server, valid) values (38,'default_httpdConf_0_Site2', 'FileAgent', 3, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (7, 38, '/etc/httpd/conf/httpd.conf', 'root', 'root', '', '', '');
insert into FileContentRules (id_file, rule) values (7, 'Listen 80');
insert into FileContentRules (id_file, rule) values (7, 'DocumentRoot');
insert into MetaAgent (id, name, type, valid) values (20,'envVariables_abs_httpd_default_webSite', 'FileAgent',False);
insert into MetaAgent (id, name, type, id_server, valid) values (29,'default_httpdConf_0_Site1', 'FileAgent', 2, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (8, 29, '/etc/httpd/conf/httpd.conf', 'root', 'root', '', '', '');
insert into FileContentRules (id_file, rule) values (8, 'Listen 80');
insert into FileContentRules (id_file, rule) values (8, 'DocumentRoot');


insert into MetaAgent (id, name, type, valid) values (12,'default_documentRoot', 'FolderAgent',False);
insert into MetaAgent (id, name, type, id_server, valid) values (27,'default_documentRoot_2_Site1', 'FolderAgent', 2, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5, maxDepth) values (9, 27, '/var/www/html', '', '', '', '', '', 0);
insert into MetaAgent (id, name, type, id_server, valid) values (36,'default_documentRoot_2_Site2', 'FolderAgent', 3, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5, maxDepth) values (10, 36, '/var/www/html', '', '', '', '', '', 0);


insert into MetaAgent (id, name, type, valid) values (7,'abs_httpd', 'ProcessAgent',False);
insert into MetaAgent (id, name, type, valid) values (9,'abs_httdpChild', 'ProcessAgent',False);
insert into MetaAgent (id, name, type, valid) values (17,'abs_httpd_default_webSite', 'ProcessAgent',False);
insert into MetaAgent (id, name, type, valid) values (19,'abs_httdpChild_c__default_webSite', 'ProcessAgent',False);
insert into MetaAgent (id, name, type, id_server, valid) values (22,'abs_httpd_Site1', 'ProcessAgent', 2, True);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (1, 22, '/usr/sbin/httpd', 'root', 1, 'httpd', '', 100, 100);
update ProcessAgent set id_procEnvFile = 6 where id = 1;
insert into ProcessLimits(id, id_proc, id_limit, value) values (1, 1, 7, 2000);
insert into MetaAgent (id, name, type, id_server, valid) values (24,'abs_httdpChild_c__Site1', 'ProcessAgent', 2, True);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (2, 24, '/usr/sbin/httpd', 'apache', 100, 'httpd', '', 100, 100);
update ProcessAgent set id_parent = 1 where id = 2;
update ProcessAgent set id_procEnvFile = 1 where id = 2;
insert into ProcessLimits(id, id_proc, id_limit, value) values (2, 2, 7, 2000);
insert into MetaAgent (id, name, type, id_server, valid) values (31,'abs_httpd_Site2', 'ProcessAgent', 3, True);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (3, 31, '/usr/sbin/httpd', 'root', 1, 'httpd', '', 100, 100);
update ProcessAgent set id_procEnvFile = 5 where id = 3;
insert into ProcessLimits(id, id_proc, id_limit, value) values (3, 3, 7, 2000);
insert into MetaAgent (id, name, type, id_server, valid) values (33,'abs_httdpChild_c__Site2', 'ProcessAgent', 3, True);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (4, 33, '/usr/sbin/httpd', 'apache', 100, 'httpd', '', 100, 100);
update ProcessAgent set id_parent = 3 where id = 4;
update ProcessAgent set id_procEnvFile = 2 where id = 4;
insert into ProcessLimits(id, id_proc, id_limit, value) values (4, 4, 7, 2000);


insert into MetaAgent (id, name, type, id_server, valid) values (3,'srv1_env', 'EnvironmentAgent', 2, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (1, 3, 3, 9, 100, 99);
insert into IgnoredFS (id_envAgent, fsName) values (1, '/cvmfs');
insert into IgnoredFS (id_envAgent, fsName) values (1, '/castorfs');
insert into MetaAgent (id, name, type, id_server, valid) values (5,'srv2_env', 'EnvironmentAgent', 3, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (2, 5, 4, 6, 100, 99);
insert into IgnoredFS (id_envAgent, fsName) values (2, '/cvmfs');
insert into IgnoredFS (id_envAgent, fsName) values (2, '/castorfs');
insert into MetaAgent (id, name, type, valid) values (1,'abs_srv_env', 'EnvironmentAgent',False);


insert into Occurrence(id) value (4);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (1, 35, 36, 4);
insert into Occurrence(id) value (2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (2, 30, 37, 2);
insert into Occurrence(id) value (3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (3, 30, 31, 3);
insert into Occurrence(id) value (1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (4, 30, 35, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (5, 13, 7, 3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (6, 13, 39, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (7, 13, 40, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (8, 21, 28, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (9, 21, 22, 3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (10, 21, 26, 1);
insert into Occurrence(id) value (5);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (11, 15, 11, 5);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (12, 26, 27, 4);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (13, 16, 15, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (14, 16, 17, 3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (15, 16, 14, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (16, 28, 29, 5);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (17, 37, 38, 5);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (18, 14, 12, 4);




