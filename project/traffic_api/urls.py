from django.urls import path
from . import views

urlpatterns = [
    # These create the exact endpoints for Postman
    path('register/', views.register_vehicle_gateway, name='gateway_register'),
    path('recognize/', views.recognize_plate_gateway, name='gateway_recognize'),
]