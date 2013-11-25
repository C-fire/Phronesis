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


insert into MetaAgent (id, name, type, id_total, classification) values (1,'MySite', 'Coordinator', 1, 'A-0');
insert into MetaAgent (id, name, type, id_total, classification) values (2,'mySite_configuration', 'Coordinator', 3, 'A-1');
insert into MetaAgent (id, name, type, id_total, classification) values (3,'mySite_data', 'Coordinator', 2, 'A-1');


insert into MetaAgent (id, name, type, id_server) values (4,'mySite_data_file', 'FileAgent', 1);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 4, '/etc/httpd/conf.d', 'apache', '', '', '', '');
insert into MetaAgent (id, name, type, id_server) values (5,'localhost_fstab', 'FileAgent', 1);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (2, 5, '/etc/fstab', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server) values (6,'mySite_conf_folder', 'FolderAgent', 1);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (3, 6, '/etc/httpd/conf.d', 'root', '', '', '', '');


insert into MetaAgent (id, name, type, id_server) values (7,'apache_httpd_MySite', 'ProcessAgent', 1);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (1, 7, '/usr/bin/httpd', 'root', -1, '', '', 100, 100);
insert into MetaAgent (id, name, type, id_server) values (8,'apache_httpdChild_MySite', 'ProcessAgent', 1);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (2, 8, '/usr/bin/httpd', 'apache', -1, '', '', 100, 100);
update ProcessAgent set id_parent = 1 where id = 2;


insert into MetaAgent (id, name, type, id_server) values (9,'localhost_env', 'EnvironmentAgent', 1);
insert into EnvironmentAgent (id, id_metaAgent) values (1, 9);


insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (1, 1, 2, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (2, 1, 3, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (3, 1, 7, 3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (4, 2, 6, 4);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (5, 3, 4, 5);




