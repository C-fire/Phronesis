Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;
insert into Total(id) value (1);
insert into Total(id) value (2);
insert into Total(id) value (3);
insert into Total(id) value (4);


insert into Occurrence(id) value (1);
insert into Occurrence(id) value (2);
insert into Occurrence(id) value (3);
insert into Occurrence(id) value (4);


insert into Server(id, name, address) values (1, 'localhost', 'localhost');


insert into MetaAgent (id, name, type, id_total, classification) values (1,'A', 'Coordinator', 1, 'A-0');
insert into MetaAgent (id, name, type, id_total, classification) values (2,'C', 'Coordinator', 3, 'B-0');
insert into MetaAgent (id, name, type, id_total, classification) values (3,'B', 'Coordinator', 2, 'C-0');
insert into MetaAgent (id, name, type, id_total, classification) values (4,'D', 'Coordinator', 4, 'D-0');


insert into MetaAgent (id, name, type, id_server) values (5,'a', 'FileAgent', 1);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 5, '/tmp/bonjour.jpg', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server) values (6,'localhost_fstab', 'FileAgent', 1);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (2, 6, '/etc/fstab', '', '', '', '', '');


insert into MetaAgent (id, name, type, id_server) values (7,'b', 'ProcessAgent', 1);
insert into ProcessAgent (id, id_metaAgent, procName, user, multiplicity, service, command, maxCpu, maxMemory) values (1, 7, 'whoCares', 'None', -1, '', '', 101, 200);


insert into MetaAgent (id, name, type, id_server) values (8,'localhost_env', 'EnvironmentAgent', 1);
insert into EnvironmentAgent (id, id_metaAgent) values (1, 8);


insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (1, 1, 5, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (2, 2, 5, 3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (3, 3, 5, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (4, 4, 7, 4);


insert into Rule (id, id_prior, id_after, occurrence, validity) values (1, 1, 2, 0, 1);
insert into Rule (id, id_prior, id_after, occurrence, validity) values (2, 3, 2, 0, 1);


