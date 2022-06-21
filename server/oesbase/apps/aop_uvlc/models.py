
#-*- coding:utf-8 -*-
from django.db import models
from django.utils.translation import gettext as _
import uuid

'''
Commands
'''
class CommandSet(models.Model):
    command = models.CharField(max_length=255, blank=False, default="")
    info = models.TextField(blank=True, default="")

    def __str__(self):
        return self.command

'''
Broker Address
'''
class BrokerAddress(models.Model):
    ip = models.CharField(max_length=255, blank=False, default="127.0.0.1")   
    info = models.TextField(blank=True, default="")
    def __str__(self):
        return self.ip

