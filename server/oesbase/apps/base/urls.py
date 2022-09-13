
from django.urls import path, include
from base import views

# api views
urlpatterns = [
    path('', views.index, name="base_index"),
]