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
from .models import RSU, UnitType
from django.db.models import Q

class API(APIView):

    permission_classes = [AllowAny]

    def get(self, request, *args, **kwargs):
        return Response({}, status=status.HTTP_400_BAD_REQUEST)
        

    def post(self, request, *args, **kwargs):
        try :
            _unittype = UnitType.objects.get(id=request.data["unittype"])
            _loraid = request.data["loraid"]
            _location = request.data["location"]
            _tagname = request.data["tagname"]
            _activate = request.data["activate"]
            
            
            obj, created = RSU.objects.get_or_create(loraid=_loraid, location=_location, tagname=_tagname, activate=_activate, unittype=_unittype)
            if created:
                print("Added New RSU : ", obj.uid)
                return Response({"message":"New RSU device is created"}, status=status.HTTP_200_OK)
            else:
                print("RSU is already exist : ", obj.id)
                return Response({"message":"RSU is already exist"}, status=status.HTTP_200_OK)

        except Exception as e:
            print("Exception : Regist new rsu ", str(e))
            return Response({"message":str(e)}, status=status.HTTP_500_INTERNAL_SERVER_ERROR)

