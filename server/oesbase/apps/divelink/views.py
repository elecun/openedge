from django.shortcuts import render
from .models import UnitType, RSU


'''
Index page
'''
def index(request):
    types = UnitType.objects.all().values("ip", "name")
    context = {"types":types}
    return render(request, "html/index.html", context)
