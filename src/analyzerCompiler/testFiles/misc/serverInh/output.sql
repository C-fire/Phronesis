Drop database  IF EXISTS phronesisAnalyzer;
create database phronesisAnalyzer;
use phronesisAnalyzer;
source /home/chaen/phronesis/phronesisAnalyzer.sql;












insert into Server(id, name, address) values (1, 'LHCb_base', 'None');
insert into Server(id, name, address) values (2, 'gw01', 'gw01');




insert into MetaAgent (id, name, type, valid) values (2,'LHCb_base_fstab', 'FileAgent',False);
insert into MetaAgent (id, name, type, id_server, valid) values (4,'gw01_fstab', 'FileAgent', 2, True);
insert into FileAgent (id, id_metaAgent, filename, owner, grp, permissions, attributes, md5) values (1, 4, '/etc/fstab', '', '', '', '', '');
insert into FileContentRules (id_file, rule) values (1, 'nfs:/nfsexports/software /sw nfs rw,hard,intr,nolock 0 0');
insert into FileContentRules (id_file, rule) values (1, 'nfs:/nfsexports/grouphomes /group nfs rw,hard,intr 0 0');
insert into FileContentRules (id_file, rule) values (1, 'homenfs:/nfsexports/userhomes /home nfs rw,hard,intr 0 0');
insert into FileContentRules (id_file, rule) values (1, 'nfs:/nfsexports/admin /admin nfs rw,hard,intr 0 0');
insert into FileContentRules (id_file, rule) values (1, 'nfs:/nfsexports/scratch /scratch nfs rw,hard,intr 0 0');
insert into FileContentRules (id_file, rule) values (1, 'nfs:/nfsexports/calib /calib nfs ro,hard,intr 0 0');
insert into MetaAgent (id, name, type, valid) values (5,'f', 'FileAgent',False);






insert into MetaAgent (id, name, type, valid) values (1,'LHCb_base_env', 'EnvironmentAgent',False);
insert into MetaAgent (id, name, type, id_server, valid) values (3,'gw01_env', 'EnvironmentAgent', 2, True);
insert into EnvironmentAgent (id, id_metaAgent, id_fstab, maxLoad, maxMemory, maxSwap) values (1, 3, 1, 2, 100, 99);






