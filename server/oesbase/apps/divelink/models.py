from django.db import models


'''
RSU configuration
'''
class RSU(models.Model):
    uid = models.PositiveIntegerField(blank=False, null=False, unique=True)
    lsid = models.PositiveIntegerField(blank=True, null=True, default=0)
    ldid = models.PositiveIntegerField(blank=True, null=True, default=0)
    activate = models.BooleanField(blank=True, null=True, default=False)

    def __str__(self):
        return self.uid