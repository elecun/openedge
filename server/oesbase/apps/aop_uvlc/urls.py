
from django.urls import path, include
from aop_uvlc import views

# api views
urlpatterns = [
    
    path('', views.manual_control, name="aop_uvlc_view"),
]