from django.shortcuts import render
from .models import CommandSet, BrokerAddress
from django.core import serializers

'''
Manual Control for UV Lamp Cleaning System
'''
def manual_control(request):
    commandset = CommandSet.objects.all()
    # addresses = serializers.serialize("json", BrokerAddress.objects.all(), fields=("ip", "name", "info"))
    addresses = BrokerAddress.objects.all().values("ip", "name")
    context = {"addresses":addresses}
    print(context)
    return render(request, "html/control.html", context)