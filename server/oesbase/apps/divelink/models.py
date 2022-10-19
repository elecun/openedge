from django.db import models
import uuid

'''
Unit
'''
class UnitType(models.Model):
    typename = models.CharField(blank=False, null=False, max_length=256)

    def __str__(self):
        return self.typename

'''
RSU configuration
'''
class RSU(models.Model):
    uid = models.CharField(max_length=64, blank=False, null=False, default=uuid.uuid4().hex, unique=True)
    location = models.CharField(blank=False, null=False, max_length=256)
    tagname = models.CharField(blank=False, null=False, max_length=256)
    loraid = models.PositiveIntegerField(blank=True, null=True, default=0)
    activate = models.BooleanField(blank=True, null=True, default=False)
    unittype = models.ForeignKey("UnitType", null=True, on_delete=models.SET_NULL, db_column="unittype_id", related_name="unittype")

    def __str__(self):
        return self.tagname

    
'''
Data Log
'''
class Datalog(models.Model):
    uid = models.CharField(max_length=64, blank=False, null=False, default=uuid.uuid4().hex, unique=True)
    note = models.TextField(blank=True, null=True)
    date_from = models.DateTimeField(auto_now_add=True, blank=True, null=True)
    date_to = models.DateTimeField(auto_now=False, blank=True, null=True)
    
    def __str__(self):
        return self.uid


'''
Station Setting
'''
class Settings(models.Model):
    lora_id = models.PositiveIntegerField(blank=False, null=False, unique=True)

    def __str__(self):
        return str(lora_id)


'''
Broker Addresses
Accessable MQTT Broker Address
'''
class BrokerAddress(models.Model):
    ip = models.CharField(max_length=255, blank=False, default="127.0.0.1")
    name = models.CharField(max_length=255, blank=True, default="")
    info = models.TextField(blank=True, default="")
    def __str__(self):
        return self.ip