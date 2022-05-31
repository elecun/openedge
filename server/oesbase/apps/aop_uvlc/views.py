from django.shortcuts import render


'''
Manual Control for UV Lamp Cleaning System
'''
def manual_control(request):
    return render(request, "html/app_aop_uvlc_mc.html")