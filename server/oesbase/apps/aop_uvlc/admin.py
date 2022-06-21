from django.contrib import admin


from django.contrib import admin
from aop_uvlc.models import CommandSet, BrokerAddress

admin.site.register(CommandSet)
admin.site.register(BrokerAddress)
