/**
 * @file mqtt.hpp
 * @brief	mqtt library wrapper
 * @author	Byunghun Hwang<bh.hwang@iae.re.kr>
 */
#ifndef _MQTT_HPP_
#define _MQTT_HPP_

#include <3rdparty/mosquitto/cpp/mosquittopp.h>
#include <string>

using namespace std;

class mqttClient : public mosqpp::mosquittopp
{
public:
	mqttClient (const char* id, const char* host, int port, const char* topic);
    ~mqttClient();

    void on_connect(int rc);
    void on_disconnect(int rc);
    void on_message(const struct mosquitto_message *message);

    bool connected() { return _connected; }

private:
    string _topic;
    bool _connected = false;
};

#endif