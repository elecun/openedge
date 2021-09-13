
/**
 * @file    avahi.mdns.service.hpp
 * @brief   mDNS/DNS-SD service with avahi daemon
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_SERVICE_AVAHI_MDNS_HPP_
#define _OPENEDGE_SERVICE_AVAHI_MDNS_HPP_

#include <openedge/core/service.hpp>

using namespace oe;

class avahiMdnsService : public core::iService {
    public:
        avahiMdnsService();
        virtual ~avahiMdnsService();
        
        //common iservice interface
        bool initService(const char* config = nullptr) override;
        bool closeService() override;

        //services APIs
        bool publish(const string& data);
        bool drop();

    private:
        // string _mqtt_address {""};
        // string _topic {""};
        // unique_ptr<mqttClient> _mqtt { nullptr };

}; //class


EXPORT_SERVICE_API

#endif