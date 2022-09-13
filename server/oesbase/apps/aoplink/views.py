from django.shortcuts import render


'''
Show User Interface
'''
def index(request):
    return render(request, "html/index.html")
