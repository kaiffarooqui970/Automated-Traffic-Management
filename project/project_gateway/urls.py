from django.contrib import admin
from django.urls import path
from traffic_api import views

urlpatterns = [
    path('admin/', admin.site.urls),

    # REST API endpoints
    path('api/gateway/register/', views.register_vehicle),
    path('api/gateway/recognize/', views.recognize_plate),
]
