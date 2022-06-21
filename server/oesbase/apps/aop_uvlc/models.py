
#-*- coding:utf-8 -*-
from django.db import models
from django.utils.translation import gettext as _
import uuid

'''
Commands
UVLC AC Motor Control Command
'''
class CommandSet(models.Model):
    command = models.CharField(max_length=255, blank=False, default="")
    name = models.CharField(max_length=255, blank=True, default="")
    info = models.TextField(blank=True, default="")
    def __str__(self):
        return self.command

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

