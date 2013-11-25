Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;

insert into Server(id, name, address) values (2, 'real_srv_1', '192.168.1.50');
insert into Server(id, name, address) values (1, 'abs_srv', 'None');
insert into Server(id, name, address) values (3, 'real_srv_2', '192.168.1.50');

insert into Total (id) value (2);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (11,'attach2', 'Coordinator', 2, 'None',False, 0);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (17,'attach2_on_real_srv_2', 'Coordinator', 2, '0',True, 0);
insert into Total (id) value (1);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (10,'attach1', 'Coordinator', 1, 'None',False, 1);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (12,'attach1_on_real_srv_1', 'Coordinator', 1, '1',True, 1);
insert into MetaAgent (id, name, type, id_total, classification,valid, tolerate) values (15,'attach2_on_real_srv_1', 'Coordinator', 2, '2',True, 0);

Update MetaAgent set id_server = 2 where id = 12;
Update MetaAgent set id_server = 2 where id = 15;
Update MetaAgent set id_server = 3 where id = 17;

insert into MetaAgent (id, name, type, valid) values (9,'conf2', 'FileAgent',False);
insert into MetaAgent (id, name, type, valid) values (7,'conf1', 'FileAgent',False);
insert into MetaAgent (id, name, type, id_server, valid) values (14,'conf1bis_attach1_on_real_srv_1', 'FileAgent', 2, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 14, '/path/to/conf/bis', '', '', '', '', '');
insert into MetaAgent (id, name, type, valid) values (2,'abs_srv_fstab', 'FileAgent',False);
insert into MetaAgent (id, name, type, id_server, valid) values (4,'real_srv_1_fstab', 'FileAgent', 2, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (2, 4, '/etc/fstab', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (16,'conf2_attach2_on_real_srv_1', 'FileAgent', 2, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (3, 16, '/path/to/conf2/', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (18,'conf2_attach2_on_real_srv_2', 'FileAgent', 3, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (4, 18, '/path/to/conf2/', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (13,'conf1_attach1_on_real_srv_1', 'FileAgent', 2, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (5, 13, '/path/to/conf', '', '', '', '', '');
insert into MetaAgent (id, name, type, id_server, valid) values (6,'real_srv_2_fstab', 'FileAgent', 3, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (6, 6, '/etc/fstab', '', '', '', '', '');
insert into MetaAgent (id, name, type, valid) values (8,'conf1bis', 'FileAgent',False);

insert into MetaAgent (id, name, type, id_server, valid) values (3,'real_srv_1_env', 'EnvironmentAgent', 2, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (1, 3, 2, 2, 100, 99);
insert into MetaAgent (id, name, type, valid) values (1,'abs_srv_env', 'EnvironmentAgent',False);
insert into MetaAgent (id, name, type, id_server, valid) values (5,'real_srv_2_env', 'EnvironmentAgent', 3, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (2, 5, 6, 2, 100, 99);

insert into Occurrence(id) value (3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (1, 11, 9, 3);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (2, 17, 18, 3);
insert into Occurrence(id) value (1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (3, 10, 7, 1);
insert into Occurrence(id) value (2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (4, 10, 8, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (5, 12, 13, 1);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (6, 12, 14, 2);
insert into MetaAgentTree (id, id_parent, id_daughter, id_occurrence) values (7, 15, 16, 3);
