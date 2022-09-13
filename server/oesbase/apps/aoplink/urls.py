
from django.urls import path, include
from aoplink import views

# api views
urlpatterns = [
    
    path('', views.index, name="aoplink_index"),
]