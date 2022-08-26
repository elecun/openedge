
from django.urls import path, include
from divelink import views

# api views
urlpatterns = [
    path('', views.index, name="divelink_index"),
]