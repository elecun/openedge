
from django.urls import path, include
from aop_uvlc import views

# api views
urlpatterns = [
    
    path('control', views.manual_control, name="aop_uvlc_control"),
]