

#include "peak.ethercan.service.hpp"

static ServicePeakEtherCAN* _instance = nullptr;
ServicePeakEtherCAN* create(){ if(!_instance) _instance = new ServicePeakEtherCAN(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}