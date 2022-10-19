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


class API(APIView):

    permission_classes = [AllowAny]

    def get(self, request, *args, **kwargs):
        try :
            _rsu_list = RSU.objects.all().values()

            # append Unittype Name
            for _q in _rsu_list:
                _q['unittype_name'] = UnitType.objects.get(id=_q['unittype_id']).typename
            print(_rsu_list)
            if not _rsu_list.exists():                
                return Response({}, status=status.HTTP_204_NO_CONTENT)
            else:
                return Response({"data":_rsu_list}, status=status.HTTP_200_OK)

        except Exception as e:
            print("Exception : ", str(e))
            return Response({"message":str(e)}, status=status.HTTP_500_INTERNAL_SERVER_ERROR)
        

    def post(self, request, *args, **kwargs):
        return Response({}, status=status.HTTP_400_BAD_REQUEST)

