import json
import requests
from django.http import JsonResponse
from django.views.decorators.csrf import csrf_exempt

# Correct C++ backend URL
CROW_BACKEND = "[localhost](http://localhost:8080)"


@csrf_exempt
def register_vehicle(request):
    if request.method != "POST":
        return JsonResponse({"error": "POST method only"}, status=405)

    data = json.loads(request.body.decode("utf-8"))

    try:
        response = requests.post(f"{CROW_BACKEND}/register", json=data)
        return JsonResponse(response.json(), status=response.status_code)
    except Exception as e:
        return JsonResponse({"error": "Crow backend unreachable", "details": str(e)}, status=500)


@csrf_exempt
def recognize_plate(request):
    plate = request.GET.get("plate")

    if not plate:
        return JsonResponse({"error": "Missing plate parameter"}, status=400)

    try:
        response = requests.get(f"{CROW_BACKEND}/recognize?plate={plate}")
        return JsonResponse(response.json(), status=response.status_code)
    except Exception as e:
        return JsonResponse({"error": "Crow backend unreachable", "details": str(e)}, status=500)
