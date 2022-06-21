from django.shortcuts import render
from models import CommandSet, BrokerAddress

'''
Manual Control for UV Lamp Cleaning System
'''
def manual_control(request):
    commandset = CommandSet.objects.all()
    
    return render(request, "html/control.html")