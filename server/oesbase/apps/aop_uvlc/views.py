from django.shortcuts import render
from .models import CommandSet, BrokerAddress

'''
Manual Control for UV Lamp Cleaning System
'''
def manual_control(request):
    addresses = BrokerAddress.objects.all().values("ip", "name")
    context = {"addresses":addresses}
    return render(request, "html/control.html", context)