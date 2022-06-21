from django.shortcuts import render
from django.conf import settings

'''
OpenEdge Server Base Index
'''
def index(request):
    apps = [app for app in settings.INSTALLED_APPS if not app.startswith("django.")]
    return render(request, "html/index.html", {'apps':apps})
