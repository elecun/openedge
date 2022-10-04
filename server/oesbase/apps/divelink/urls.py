
from django.urls import path, include
from divelink import views
#from django.contrib.staticfiles.urls import staticfiles_urlpatterns
from divelink import api_rsu_list
from divelink import api_rsu_add
from divelink import api_rsu_delete

# api views
urlpatterns = [
    path('', views.index, name="divelink_index"),
    
    # APIs
    path("rsu/list/", api_rsu_list.API.as_view(), name="api_rsu_list"),
    path("rsu/add/", api_rsu_add.API.as_view(), name="api_rsu_add"),
    path("rsu/delete/", api_rsu_delete.API.as_view(), name="api_rsu_delete"),

]
#urlpatterns += staticfiles_urlpatterns()