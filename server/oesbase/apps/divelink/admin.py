from django.contrib import admin
from divelink.models import RSU, UnitType, BrokerAddress, SystemSetting

admin.site.register(RSU)
admin.site.register(UnitType)
admin.site.register(BrokerAddress)
admin.site.register(SystemSetting)