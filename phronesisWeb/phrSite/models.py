# This is an auto-generated Django model module.
# You'll have to do the following manually to clean this up:
#     * Rearrange models' order
#     * Make sure each model has one field with primary_key=True
# Feel free to rename the models, but don't rename db_table values or field names.
#
# Also note: You'll have to insert the output of 'django-admin.py sqlcustom [appname]'
# into your database.
from __future__ import unicode_literals

from django.db import models

class Environmentagent(models.Model):
    id = models.IntegerField(primary_key=True)
    id_metaagent = models.ForeignKey('Metaagent', db_column='id_metaAgent') # Field name made lowercase.
    id_fstab = models.ForeignKey('Fileagent', null=True, db_column='id_fstab', blank=True)
    maxload = models.FloatField(null=True, db_column='maxLoad', blank=True) # Field name made lowercase.
    maxmemory = models.FloatField(null=True, db_column='maxMemory', blank=True) # Field name made lowercase.
    maxswap = models.FloatField(null=True, db_column='maxSwap', blank=True) # Field name made lowercase.
    class Meta:
        db_table = 'EnvironmentAgent'

class Fileagent(models.Model):
    id = models.IntegerField(primary_key=True)
    id_metaagent = models.ForeignKey('Metaagent', db_column='id_metaAgent') # Field name made lowercase.
    filename = models.CharField(max_length=500L, blank=True)
    owner = models.CharField(max_length=100L, blank=True)
    grp = models.CharField(max_length=100L, blank=True)
    permissions = models.CharField(max_length=4L, blank=True)
    attributes = models.CharField(max_length=19L, blank=True)
    md5 = models.CharField(max_length=32L, blank=True)
    maxdepth = models.IntegerField(null=True, db_column='maxDepth', blank=True)
    class Meta:
        db_table = 'FileAgent'

class Filecontentrules(models.Model):
    id = models.IntegerField(primary_key=True)
    id_file = models.ForeignKey(Fileagent, db_column='id_file')
    rule = models.CharField(max_length=10000L, blank=True)
    class Meta:
        db_table = 'FileContentRules'


class Filenamefilter(models.Model):                                                                     
    id = models.IntegerField(primary_key=True)                                                          
    id_folder = models.ForeignKey(Fileagent, null=True, db_column='id_folder', blank=True)              
    filter = models.CharField(max_length=10000L)                                                        
    class Meta:                                                                                         
        db_table = 'FilenameFilter' 


class Limits(models.Model):
    id = models.IntegerField(primary_key=True)
    name = models.CharField(max_length=45L, blank=True)
    defvalue = models.BigIntegerField(db_column='defValue') # Field name made lowercase.
    class Meta:
        db_table = 'Limits'

class Metaagent(models.Model):
    id = models.IntegerField(primary_key=True)
    name = models.CharField(max_length=200L, blank=True)
    type = models.CharField(max_length=45L, blank=True)
    id_total = models.ForeignKey('Total', null=True, db_column='id_total', blank=True)
    classification = models.CharField(max_length=45L, blank=True)
    id_server = models.ForeignKey('Server', null=True, db_column='id_server', blank=True)
    valid = models.IntegerField(null=True, blank=True)
    local_total = models.IntegerField(null=True, blank=True)
    tolerate = models.IntegerField(null=True, blank=True)
    class Meta:
        db_table = 'MetaAgent'

class Metaagenttree(models.Model):
    id = models.IntegerField(primary_key=True)
    id_parent = models.ForeignKey(Metaagent, null=True, db_column='id_parent', blank=True, related_name = 'metaagenttree_parent')
    id_daughter = models.ForeignKey(Metaagent, null=True, db_column='id_daughter', blank=True, related_name = 'metaagenttree_daughter')
    id_occurrence = models.ForeignKey('Occurrence', null=True, db_column='id_occurrence', blank=True)
    local_occurrence = models.IntegerField(null=True, blank=True)
    class Meta:
        db_table = 'MetaAgentTree'

class Occurrence(models.Model):
    id = models.IntegerField(primary_key=True)
    occurrence = models.IntegerField(null=True, blank=True)
    class Meta:
        db_table = 'Occurrence'

class Processagent(models.Model):
    id = models.IntegerField(primary_key=True)
    id_metaagent = models.ForeignKey(Metaagent, db_column='id_metaAgent') # Field name made lowercase.
    procname = models.CharField(max_length=200L, db_column='procName', blank=True) # Field name made lowercase.
    user = models.CharField(max_length=100L, blank=True)
    id_procenvfile = models.ForeignKey(Fileagent, null=True, db_column='id_procEnvFile', blank=True) # Field name made lowercase.
    id_parent = models.ForeignKey('self', null=True, db_column='id_parent', blank=True)
    multiplicity = models.IntegerField(null=True, blank=True)
    command = models.CharField(max_length=300L, blank=True)
    service = models.CharField(max_length=100L, blank=True)
    maxcpu = models.FloatField(null=True, db_column='maxCpu', blank=True) # Field name made lowercase.
    maxmemory = models.FloatField(null=True, db_column='maxMemory', blank=True) # Field name made lowercase.
    class Meta:
        db_table = 'ProcessAgent'

class Processlimits(models.Model):
    id = models.IntegerField(primary_key=True)
    id_proc = models.ForeignKey(Processagent, null=True, db_column='id_proc', blank=True)
    id_limit = models.ForeignKey(Limits, null=True, db_column='id_limit', blank=True)
    value = models.FloatField()
    class Meta:
        db_table = 'ProcessLimits'

class Recoveryhistory(models.Model):
    id = models.IntegerField(primary_key=True)
    date = models.CharField(max_length=45L, blank=True)
    time = models.CharField(max_length=45L, blank=True)
    responsible = models.CharField(max_length=200L, blank=True)
    userentry = models.CharField(max_length=1000L, db_column='userEntry', blank=True) # Field name made lowercase.
    class Meta:
        db_table = 'RecoveryHistory'

class RecoveryhistoryAction(models.Model):
    id = models.IntegerField(primary_key=True)
    id_recoveryhistory = models.ForeignKey(Recoveryhistory, null=True, db_column='id_recoveryHistory', blank=True) # Field name made lowercase.
    metaagent = models.CharField(max_length=200L, db_column='metaAgent', blank=True) # Field name made lowercase.
    action = models.CharField(max_length=45L, blank=True)
    class Meta:
        db_table = 'RecoveryHistory_Action'

class RecoveryhistoryProblem(models.Model):
    id = models.IntegerField(primary_key=True)
    id_recoveryhistory = models.ForeignKey(Recoveryhistory, null=True, db_column='id_recoveryHistory', blank=True) # Field name made lowercase.
    metaagent = models.CharField(max_length=200L, db_column='metaAgent', blank=True) # Field name made lowercase.
    before_after = models.CharField(max_length=45L, blank=True)
    class Meta:
        db_table = 'RecoveryHistory_Problem'

class Recoveryrule(models.Model):
    id = models.IntegerField(primary_key=True)
    id_ma1 = models.ForeignKey(Metaagent, null=True, db_column='id_ma1', blank=True, related_name = 'recoveryrule_ma1')
    action1 = models.CharField(max_length=45L, blank=True)
    id_ma2 = models.ForeignKey(Metaagent, null=True, db_column='id_ma2', blank=True, related_name = 'recoveryrule_ma2')
    action2 = models.CharField(max_length=45L, blank=True)
    direction = models.CharField(max_length=45L, blank=True)
    class Meta:
        db_table = 'RecoveryRule'


class Dependency(models.Model):                                                                
    id = models.IntegerField(primary_key=True)                                                
    id_prior = models.ForeignKey('Metaagent', null=True, db_column='id_prior', blank=True, related_name = 'dependency_prior')    
    id_after = models.ForeignKey('Metaagent', null=True, db_column='id_after', blank=True, related_name = 'dependency_after')    
    occurrence = models.IntegerField(null=True, blank=True)                                   
    validity = models.IntegerField(null=True, blank=True)                                     
    class Meta:                                                                               
        db_table = 'Dependency'

class Server(models.Model):
    id = models.IntegerField(primary_key=True)
    name = models.CharField(max_length=200L, blank=True)
    address = models.CharField(max_length=200L, blank=True)
#    id_envagent = models.ForeignKey(Environmentagent, null=True, db_column='id_envAgent', blank=True) # Field name made lowercase.
    occurrence = models.IntegerField(null=True, blank=True)
    class Meta:
        db_table = 'Server'

class Serverattachedcoord(models.Model):
    id = models.IntegerField(primary_key=True)
    id_server = models.ForeignKey(Server, null=True, db_column='id_server', blank=True)
    id_metaagent = models.ForeignKey(Metaagent, null=True, db_column='id_metaAgent', blank=True) # Field name made lowercase.
    class Meta:
        db_table = 'ServerAttachedCoord'

class Total(models.Model):
    id = models.IntegerField(primary_key=True)
    total = models.IntegerField(null=True, blank=True)
    unsolved = models.IntegerField(null=True, blank=True)
    class Meta:
        db_table = 'Total'

