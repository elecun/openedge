
from django.urls import path, include
from divelink import views
#from django.contrib.staticfiles.urls import staticfiles_urlpatterns

# api views
urlpatterns = [
    path('', views.index, name="divelink_index"),
]
#urlpatterns += staticfiles_urlpatterns()