import os
import sys
from pathlib import Path
from django.core.wsgi import get_wsgi_application

# 1. Get the absolute path to your project root
BASE_DIR = Path(__file__).resolve().parent.parent
if str(BASE_DIR) not in sys.path:
    sys.path.append(str(BASE_DIR))

# 2. Tell Django where the settings are
os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'traffic_gateway.settings')

# 3. Try to load the application
try:
    application = get_wsgi_application()
    print("✅ WSGI is alive and running!")
except Exception as e:
    print(f"❌ WSGI failed to load: {e}")
    raise e