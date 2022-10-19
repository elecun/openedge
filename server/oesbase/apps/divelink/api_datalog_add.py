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

class API(APIView):

    permission_classes = [AllowAny]

    def get(self, request, *args, **kwargs):
        return Response({}, status=status.HTTP_400_BAD_REQUEST)
        

    def post(self, request, *args, **kwargs):
        try :
            _new_datalog = Datalog()
            _new_datalog.uid = uuid.uuid4().hex
            _new_datalog.save()
            print("Added new Datalog")
            return Response({"message":"New Datalog is created"}, status=status.HTTP_200_OK)

        except Exception as e:
            print("Exception : Regist new rsu ", str(e))
            return Response({"message":str(e)}, status=status.HTTP_500_INTERNAL_SERVER_ERROR)

