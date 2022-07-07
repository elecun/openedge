# Generated by Django 4.0.5 on 2022-06-20 02:15

from django.db import migrations, models


class Migration(migrations.Migration):

    initial = True

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='BrokerAddress',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('ip', models.CharField(default='127.0.0.1', max_length=255)),
                ('info', models.TextField(blank=True, default='')),
            ],
        ),
        migrations.CreateModel(
            name='CommandSet',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('command', models.CharField(default='', max_length=255)),
                ('info', models.TextField(blank=True, default='')),
            ],
        ),
    ]