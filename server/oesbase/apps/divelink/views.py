from django.shortcuts import render
from .models import UnitType, RSU, BrokerAddress


'''
Index page
'''
def index(request):
    brokers = BrokerAddress.objects.all().values("id", "ip", "name", "info")
    types = UnitType.objects.all().values("id", "typename")
    context = {"brokers":brokers, "types":types}
    print(context)

    return render(request, "html/index.html", context)
