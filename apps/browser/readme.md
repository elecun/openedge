
# setup for electron

curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
sudo apt-get install -y nodejs

$ source ./venv/bin/activate
(venv)$ sudo apt-get install curl
(venv)$ curl -fsSL https://deb.nodesource.com/setup_14.x | sudo -E bash -
(venv)$ sudo apt-get install -y nodejs 
(venv)$ sudo apt-get install -y npm

* if it shows problems for package dependency,
(venv)$ sudo apt-get install aptitude
(venv)$ sudo aptitude install -y nodejs npm

(venv)$ sudo apt-get install libmysqlclient-dev git build-essential python3.8-dev mariadb-server mariadb-client libmysqlclient-dev
(venv)$ npm init (create only, do not type this command!!)
(venv)$ npm install --save-dev electron electron-builder electron-packager electron-installer-squirrel-windows
(venv)$ npm install --save python-shell
(venv)$ npm install --save mqtt (/opt/mex/resources/ 디렉토리에서 설치해야 함 $ sudo npm install --save mqtt)
(venv)$ npm install electron-prebuilt


* build for linux64 (ubuntu)
- $ npm run build:linux64