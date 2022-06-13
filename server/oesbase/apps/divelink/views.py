from django.shortcuts import render


'''
Show User Interface
'''
def show_gui(request):
    return render(request, "html/gui.html")
