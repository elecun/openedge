#-*- coding:utf-8 -*-

from django.shortcuts import render
from django.contrib.auth.models import User, Group
from rest_framework import viewsets, status
from rest_framework.response import Response
from rest_framework.decorators import api_view
from rest_framework.views import APIView
from rest_framework.permissions import IsAuthenticated, AllowAny
from django.views.decorators.csrf import csrf_protect
from django.forms.models import model_to_dict
from django.conf import settings
import uuid
import json
from django.http import HttpResponse
import mimetypes
from .models import Datalog
from django.db.models import Q
from datetime import datetime, timedelta
from influxdb_client import InfluxDBClient
from pytz import timezone, utc

def convert(tup, di):
    for a, b in tup:
        di.setdefault(a, []).append(b)
    return di

class API(APIView):

    permission_classes = [AllowAny]

    def get(self, request, *args, **kwargs):
        try :
            if 'id' in kwargs:
                _id = kwargs["id"]
                _log = Datalog.objects.get(id=_id)
                if _log is not None:
                    _log_date_from = _log.date_from.astimezone(utc).strftime("%Y-%m-%dT%H:%M:%SZ")
                    _log_date_to = _log.date_to.astimezone(utc).strftime("%Y-%m-%dT%H:%M:%SZ")

                    client = InfluxDBClient(url=settings.INFLUXDB_V2_URL, token=settings.INFLUXDB_V2_TOKEN, org=settings.INFLUXDB_V2_ORG)
                    query_api = client.query_api()

                    q = '''
                    from(bucket: "divelink_db")
                    |> range(start: {s}, stop: {e})
                    |> filter(fn: (r) => r["_measurement"] == "mqtt_consumer")
                    |> filter(fn: (r) => r["_field"] == "water_depth" or r["_field"] == "water_temperature")
                    |> aggregateWindow(every: {i}s, fn: mean, createEmpty: false)
                    |> yield(name: "mean")
                    '''.format(s=_log_date_from, e=_log_date_to, i=5)
                    
                    tables = query_api.query(q)
                    results = {"water_depth":[], "water_temperature":[], "datetime":[]}
                    for table in tables:
                        depth = []
                        temperature = []
                        time = []
                        for record in table.records:
                            results["datetime"].append(record.get_time().astimezone(timezone('Asia/Seoul')).strftime("%Y-%m-%dT%H:%M:%SZ"))
                            if record.get_field() == "water_depth":
                                results["water_depth"].append(record.get_value())
                            elif record.get_field() == "water_temperature":
                                results["water_temperature"].append(record.get_value())
                    results["datetime"] = list(dict.fromkeys(results["datetime"]))                    

                    client.close()
                    return Response({"data":json.dumps(results)}, status=status.HTTP_200_OK)

            else:
                return Response({}, status=status.HTTP_400_BAD_REQUEST)
        
        except Exception as e:
            print("exception : ", str(e))
            return Response({"message":str(e)}, status=status.HTTP_500_INTERNAL_SERVER_ERROR)

    def post(self, request, *args, **kwargs):
        return Response({}, status=status.HTTP_400_BAD_REQUEST)

