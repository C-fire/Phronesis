Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;












insert into Server(id, name, address) values (1, 'localhost', 'localhost');


insert into Total (id) value (1);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (4,'myTest', 'Coordinator', 1, 'A-0',True, 0);


insert into MetaAgent (id, name, type, id_server, valid) values (2,'localhost_fstab', 'FileAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 2, '/etc/fstab', '', '', '', '', '');


insert into MetaAgent (id, name, type, id_server, valid) values (3,'fo', 'FolderAgent', 1, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5, maxDepth) values (2, 3, '/tmp/testFolder', '', '', '', '', '', 0);
insert into FileContentRules (id_file, rule) values (2, 'Il etait un .* navire$');
insert into FilenameFilter (id_folder, filter) values (2, '.*\.txt');
insert into FilenameFilter (id_folder, filter) values (2, 'test.*');




insert into MetaAgent (id, name, type, id_server, valid) values (1,'localhost_env', 'EnvironmentAgent', 1, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (1, 1, 1, 2, 100, 99);


insert into Occurrence(id) value (1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (1, 4, 3, 1);




