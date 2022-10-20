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
from .models import SystemSetting
from django.db.models import Q

class API(APIView):

    permission_classes = [AllowAny]

    def get(self, request, *args, **kwargs):
        try :
            _setting = SystemSetting.objects.get(id=1)
            if _setting is not None:
                _setting_object = model_to_dict(_setting)
                return Response(_setting_object, status=status.HTTP_200_OK)
            return Response({}, status=status.HTTP_204_NO_CONTENT)
        
        except SystemSetting.DoesNotExist:
            return Response({}, status=status.HTTP_404_NOT_FOUND)
        except Exception as e:
            return Response({"message":str(e)}, status=status.HTTP_500_INTERNAL_SERVER_ERROR)

    def post(self, request, *args, **kwargs):
        return Response({}, status=status.HTTP_400_BAD_REQUEST)

