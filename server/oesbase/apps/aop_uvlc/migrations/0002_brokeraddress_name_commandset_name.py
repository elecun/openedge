# Generated by Django 4.0.5 on 2022-06-21 02:03

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('aop_uvlc', '0001_initial'),
    ]

    operations = [
        migrations.AddField(
            model_name='brokeraddress',
            name='name',
            field=models.CharField(blank=True, default='', max_length=255),
        ),
        migrations.AddField(
            model_name='commandset',
            name='name',
            field=models.CharField(blank=True, default='', max_length=255),
        ),
    ]
