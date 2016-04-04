//
// Created by Jonas Kahler on 4/1/16.
//
#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>
#include <opendavinci/odcore/data/Container.h>

#ifndef SERIALCONNECTOR_UDPBROADCASTER_H
#define SERIALCONNECTOR_UDPBROADCASTER_H

namespace packetio {
    class PacketBroadcaster : public odcore::base::module::TimeTriggeredConferenceClientModule {
    public:
        PacketBroadcaster(const int32_t &argc, char **argv);
        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();
        void setSensorBoardDataContainer(odcore::data::Container &);
        virtual ~PacketBroadcaster();
    private:
        virtual void setUp();
        virtual void tearDown();
        bool interrupted;
        odcore::data::Container *sensorBoardDataContainer;
    };
}


#endif //SERIALCONNECTOR_UDPBROADCASTER_H
