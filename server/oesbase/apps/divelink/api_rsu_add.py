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
from .models import RSU
from django.db.models import Q

class API(APIView):

    permission_classes = [AllowAny]

    def get(self, request, *args, **kwargs):
        return Response({}, status=status.HTTP_400_BAD_REQUEST)
        

    def post(self, request, *args, **kwargs):
        try :

            if 'uid' in request.data:
                _uid = request.data["uid"]
                if RSU.onjects.get(uid=_uid).exists():
                    if 'name' in request.data:
                        # new settings
                        _new_rsu = RSU()
                        _new_rsu.uid = request.data["uid"]
                        _new_rsu.lsid = request.data["lsid"]
                        _new_rsu.ldid = request.data["ldid"]
                        _new_rsu.activate = request.data["activate"]
                        _new_rsu.save()
                        print("Added New RSU Device : ", _new_rsu.uid)
                        _new_rsu_object = model_to_dict(_new_rsu)
                        return Response({"data":_new_rsu_object}, status=status.HTTP_200_OK)
                    else:
                        return Response({}, status=status.HTTP_400_BAD_REQUEST)
            else:
                return Response({}, status=status.HTTP_400_BAD_REQUEST) 


        except Exception as e:
            print("Exception : Regist new rsu ", str(e))
            return Response({"message":str(e)}, status=status.HTTP_500_INTERNAL_SERVER_ERROR)

