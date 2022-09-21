from django.db import models


'''
RSU configuration
'''
class RSU(models.Model):
    uid = models.CharField(max_length=64, blank=False, null=False, unique=True)
    csid = models.PositiveIntegerField(blank=True, null=True, default=0)
    cdid = models.PositiveIntegerField(blank=True, null=True, default=0)

    def __str__(self):
        return self.uid