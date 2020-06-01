
/**
 * @file    mqtt.publisher.service.hpp
 * @brief   MQTT publisher service
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_SERVICE_MQTT_PUBLISHER_HPP_
#define _OPENEDGE_SERVICE_MQTT_PUBLISHER_HPP_

#include <openedge/core/service.hpp>

using namespace oe;

class mqttClient;
class mqttPublisherService : public core::iService {
    public:
        mqttPublisherService();
        virtual ~mqttPublisherService();
        
        //common iservice interface
        bool initService(const char* config = nullptr) override;
        bool closeService() override;

        //services APIs
        bool publish(const string& topic, const string& data);

    private:
        string _mqtt_address {""};
        string _topic {""};
        unique_ptr<mqttClient> _mqtt { nullptr };

}; //class


EXPORT_SERVICE_API

#endif