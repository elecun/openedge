
/**
 * @file    
 * @brief   
 * @author  
 */

#ifndef _OPENEDGE_SERVICE_MONGODB_CONNECTOR_HPP_
#define _OPENEDGE_SERVICE_MONGODB_CONNECTOR_HPP_

#include <openedge/core/service.hpp>

class mongodbConnectorService : public oe::core::iService {
    public:
        mongodbConnectorService();
        ~mongodbConnectorService();

        bool initService() override;
};


EXPORT_SERVICE_API

#endif