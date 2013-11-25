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


insert into Server(id, name, address) values (1, 'localhost', 'localhost');


insert into MetaAgent (id, name, type, id_total, classification) values (1,'sub', 'Coordinator', 5, 'B-1');
insert into MetaAgent (id, name, type, id_total, classification) values (2,'top1', 'Coordinator', 2, 'B-0');
insert into MetaAgent (id, name, type, id_total, classification) values (3,'top2', 'Coordinator', 4, 'A-0');


insert into MetaAgent (id, name, type, id_server) values (4,'f', 'FileAgent', 1);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 4, '/dadada', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server) values (5,'localhost_fstab', 'FileAgent', 1);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (2, 5, '/etc/fstab', '', '', '', '', '');




insert into MetaAgent (id, name, type, id_server) values (6,'localhost_env', 'EnvironmentAgent', 1);
insert into EnvironmentAgent (id, id_metaAgent) values (1, 6);


insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (1, 2, 1, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (2, 2, 4, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (3, 3, 1, 3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (4, 3, 4, 4);




