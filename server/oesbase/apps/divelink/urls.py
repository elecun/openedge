
from django.urls import path, include
from divelink import views

# api views
urlpatterns = [
    path('', views.show_gui, name="divelink_view"),
]