from django.db import models
import uuid

'''
RSU configuration
'''
class RSU(models.Model):
    uid = models.PositiveIntegerField(blank=False, null=False, unique=True)
    tagname = models.CharField(blank=False, null=False, max_length=256)
    lsid = models.PositiveIntegerField(blank=True, null=True, default=0)
    ldid = models.PositiveIntegerField(blank=True, null=True, default=0)
    activate = models.BooleanField(blank=True, null=True, default=False)

    def __str__(self):
        return self.tagname

    
'''
Data Log
'''
class Datalog(models.Model):
    uid = models.CharField(max_length=64, blank=False, null=False, default=uuid.uuid4().hex, unique=True)
    note = models.TextField(blank=True, null=True)
    created_at = models.DateTimeField(auto_now_add=True)
    date_from = models.DateTimeField(auto_now=False, blank=True, null=True)
    data_to = models.DateTimeField(auto_now=False, blank=True, null=True)
    
    def __str__(self):
        return self.uid


'''
Station Setting
'''
class Settings(models.Model):
    lora_id = models.PositiveIntegerField(blank=False, null=False, unique=True)

    def __str__(self):
        return str(lora_id)