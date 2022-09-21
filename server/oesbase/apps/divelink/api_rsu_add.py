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

class API(APIView):

    permission_classes = [AllowAny]

    def get(self, request, *args, **kwargs):
        return Response({}, status=status.HTTP_400_BAD_REQUEST)
        

    def post(self, request, *args, **kwargs):
        try :
            if 'name' in request.data:
                # new settings
                _new_rsu = RSU()
                _new_rsu.uid = uuid.uuid4().hex
                _new_rsu.csid = request.data["csid"]
                _new_rsu.cdid = request.data["cdid"]
                _new_rsu.save()
                print("Added New RSU Device : ", _new_rsu.uid)
                _new_rsu_object = model_to_dict(_new_rsu)
                return Response({"data":_new_rsu_object}, status=status.HTTP_200_OK)
            else:
                return Response({}, status=status.HTTP_400_BAD_REQUEST)

        except Exception as e:
            print("Exception : Regist new rsu ", str(e))
            return Response({"message":str(e)}, status=status.HTTP_500_INTERNAL_SERVER_ERROR)

