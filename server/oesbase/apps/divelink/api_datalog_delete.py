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
            if 'id' in request.data:
                _log = Datalog.objects.get(id=request.data["id"])
                _log.delete()
                print("Successfully deleted the Log : ", request.data["id"])
                return Response({}, status=status.HTTP_200_OK)
            else:
                return Response({"message":"Invalid Request"}, status=status.HTTP_400_BAD_REQUEST) 

        except Datalog.DoesNotExist:
            return Response({"message":"Log does not exist."}, status=status.HTTP_404_NOT_FOUND)
        except Exception as e:
            print("Exception : Regist new rsu ", str(e))
            return Response({"message":str(e)}, status=status.HTTP_500_INTERNAL_SERVER_ERROR)

