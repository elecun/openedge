from django.shortcuts import render


'''
OpenEdge Server Base Index
'''
def index(request):
    print("index")
    return render(request, "html/index.html")
