from django.shortcuts import render


'''
Index page
'''
def index(request):
    print("divelink index")
    return render(request, "html/index.html")
