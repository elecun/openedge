#include "mqtt.hpp"
#include <iostream>

using namespace std;

mqttClient::mqttClient(const char* id, const char* host, int port, const char* topic)
: mosquittopp(id)
{
	mosqpp::lib_init();
    int keepalive = 120;	//second
    if(connect(host, port, keepalive)==MOSQ_ERR_SUCCESS)
    	_connected = true;
}

mqttClient::~mqttClient()
{
	disconnect();
}

void mqttClient::on_connect(int rc)
{
	cout << "MQTT connected with code" << rc << endl;
    _connected = true;
}

void mqttClient::on_disconnect(int rc)
{
	cout << "MQTT disconnected" << endl;
	_connected = false;
}

void mqttClient::on_message(const struct mosquitto_message* message)
{

}