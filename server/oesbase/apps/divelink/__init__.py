
# initialization
from django.conf import settings
import os, sys

# [EDIT] Add Static path for this app
#app_staticfiles_path = os.path.join(settings.BASE_DIR, 'apps', 'divelink', 'staticfiles')

# if app_staticfiles_path not in settings.TEMPLATES:
#     settings.TEMPLATES[0]["DIRS"].append(app_staticfiles_path)

# settings.STATICFILES_DIRS.append(app_staticfiles_path)
# print("Added divelink static file path : {}".format(settings.STATICFILES_DIRS))

# if app_staticfiles_path not in settings.TEMPLATES:
#     settings.STATICFILES_DIRS.append(app_staticfiles_path)
#     print("Added divelink static file path : {}".format(settings.STATICFILES_DIRS))