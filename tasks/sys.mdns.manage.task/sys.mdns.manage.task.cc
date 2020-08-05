

#include "sys.mdns.manage.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <3rdparty/tinyxml2/tinyxml2.h>
#include <openedge/core/registry.hpp>
#include <any>
#include <3rdparty/json.hpp>
#include <openedge/core/global.hpp>

using json = nlohmann::json;
using namespace std;
using namespace tinyxml2;


//implemented create/release singleton instance
static sysMdnsManageTask* _instance = nullptr;
oe::core::task::runnable* create(){
    if(!_instance)
        _instance = new sysMdnsManageTask();
    return _instance;
}

void release(){
    if(_instance){
        delete _instance;
        _instance = nullptr;
    }
}

// <service-group>

//   <name replace-wildcards="yes">Printer on %h</name>

//   <service>
//     <type>_printer._tcp</type>
//     <port>515</port>
//   </service>

//   <service>
//     <type>_ipp._tcp</type>
//     <subtype>_colour._sub._ipp._tcp</subtype>
//     <port>631</port>
//   </service>

// </service-group>


bool sysMdnsManageTask::configure(){

    if(!registry->find(__CONFIG_NETWORK_KEY__)){
        spdlog::warn("Cannot find {} key in the system registry",__CONFIG_NETWORK_KEY__);
        return false;
    }
    
    try {
        json network = json::parse(registry->get<std::string>(__CONFIG_NETWORK_KEY__));
        if(network.find("discovery")!=network.end()){
            //save xml into the avahi service path
            XMLDocument* doc = new XMLDocument;
            XMLDeclaration* decl = doc->NewDeclaration();
            doc->LinkEndChild(decl);

            XMLElement* root = doc->NewElement("service-group");
            doc->LinkEndChild(root);

            XMLElement* node = doc->NewElement("name");
            node->SetAttribute("replace-wildcards","yes");
            node->SetText(network["discovery"]["name"].get<string>().c_str());
            root->LinkEndChild(node);

            node = doc->NewElement("service");
            root->LinkEndChild(node);

            XMLElement* element = nullptr;
            element = doc->NewElement("type");
            element->SetText(network["discovery"]["type"].get<string>().c_str());
            node->LinkEndChild(element);

            element = doc->NewElement("port");
            element->SetText(network["discovery"]["port"].get<int>());
            node->LinkEndChild(element);

            element = doc->NewElement("txt-record");
            element->SetText(network["discovery"]["txt-record"].get<string>().c_str());
            node->LinkEndChild(element);

            string xmlfile = fmt::format("{}device.xml",registry->get<string>("MDNS_SERVICE_DIR"));
            doc->SaveFile(xmlfile.c_str());
            spdlog::info("Saved");
            
            delete doc;
            
        }
        else {
            spdlog::warn("{} has no Device/Service Discovery Configuration.", registry->get<std::string>(__CONFIG_HOSTNAME_KEY__));
        }

    }
    catch(const std::bad_any_cast& e){
        spdlog::error("{}", e.what());
        return false;
    }
    
    return true;
}

void sysMdnsManageTask::execute(){
    
}

void sysMdnsManageTask::cleanup(){
    
}