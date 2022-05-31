
from django.urls import path, include
from app_aop_uvlc import views

# api views
urlpatterns = [
    
    path('uvlc/control', views.mc_view, name="uvlc_mc"),
]