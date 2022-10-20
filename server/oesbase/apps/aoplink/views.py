from django.shortcuts import render


'''
Show User Interface
'''
def index(request):
    addresses = [{"ip":"192.168.3.130", "name":"aop"}]
    context = {"addresses":addresses}
    return render(request, "html/index.html", context)
