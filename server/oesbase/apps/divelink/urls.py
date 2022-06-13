
from django.urls import path, include
from divelink import views

# api views
urlpatterns = [
    path('gui', views.show_gui, name="divelink_show_gui"),
]