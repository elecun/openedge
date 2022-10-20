run:
	@python3 manage.py runserver 0.0.0.0:8000

migrate:
	@python3 manage.py makemigrations
	@python3 manage.py migrate --run-syncdb

loaddata-aop:
	@python3 manage.py loaddata aop_uvlc_commandset.json
	@python3 manage.py loaddata aop_uvlc_broker.json

loaddata-divelink:
	@python3 manage.py loaddata divelink_type.json
	@python3 manage.py loaddata divelink_broker.json
	@python3 manage.py loaddata divelink_setting.json

superuser:
	@python3 manage.py createsuperuser

run-celery-worker:
	@celery -A django_graph worker -l info

run-celery-beat:
	@celery -A django_graph beat -l info

run-redis:
	redis-server --daemonize yes