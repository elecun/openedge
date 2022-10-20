
from django.urls import path, include
from divelink import views
from django.contrib.staticfiles.urls import staticfiles_urlpatterns
from divelink import api_rsu_list
from divelink import api_rsu_add
from divelink import api_rsu_delete
from divelink import api_rsu_activate
from divelink import api_rsu_deactivate
from divelink import api_datalog_list
from divelink import api_datalog_add
from divelink import api_datalog_delete
from divelink import api_datalog_close
from divelink import api_datalog_view
from divelink import api_datalog_live
from divelink import api_setting_load
from divelink import api_setting_apply

# api views
urlpatterns = [
    path('', views.index, name="divelink_index"),
    
    # APIs
    path("rsu/list/", api_rsu_list.API.as_view(), name="api_rsu_list"),
    path("rsu/add/", api_rsu_add.API.as_view(), name="api_rsu_add"),
    path("rsu/delete/", api_rsu_delete.API.as_view(), name="api_rsu_delete"),
    path("rsu/activate/", api_rsu_activate.API.as_view(), name="api_rsu_activate"),
    path("rsu/deactivate/", api_rsu_deactivate.API.as_view(), name="api_rsu_deactivate"),

    path("log/list/", api_datalog_list.API.as_view(), name="api_datalog_list"),
    path("log/add/", api_datalog_add.API.as_view(), name="api_datalog_add"),
    path("log/delete/", api_datalog_delete.API.as_view(), name="api_datalog_delete"),
    path("log/close/", api_datalog_close.API.as_view(), name="api_datalog_close"),
    path("log/view/<int:id>/", api_datalog_view.API.as_view(), name="api_datalog_view"),
    path("log/live/<int:time>/", api_datalog_live.API.as_view(), name="api_datalog_live"),

    path("setting/load/", api_setting_load.API.as_view(), name="api_setting_load"),
    path("setting/apply/", api_setting_apply.API.as_view(), name="api_setting_apply"),


]
urlpatterns += staticfiles_urlpatterns()