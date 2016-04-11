//
// Created by niklas on 2016-04-07.
//
#include <iostream>

#include "opendavinci/odcore/data/Container.h"

#include "opendavinci/GeneratedHeaders_OpenDaVINCI.h"
#include "automotivedata/GeneratedHeaders_AutomotiveData.h"
#include "parker/Parker.h"

using namespace std;

using namespace odcore::base;
using namespace odcore::base::module;
using namespace odcore::data;
using namespace automotive;
using namespace automotive::miniature;

using namespace parker;

Parker::Parker(const int32_t &argc, char **argv) :
        TimeTriggeredConferenceClientModule(argc, argv, "Overtake") {}

Parker::~Parker() {}

void Parker::setUp(){
    cout << "Parker starts" << endl;
}
void Parker::tearDown(){
    cout << "This is when Parker stops" << endl;
}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode Parker::body() {

    while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
        // 1. Get most recent vehicle data:
        Container containerVehicleData = getKeyValueDataStore().get(automotive::VehicleData::ID());
        VehicleData vd = containerVehicleData.getData<VehicleData> ();

        // 2. Get most recent sensor board data describing virtual sensor data:
        Container containerSensorBoardData = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());
        SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();

        // Create vehicle control data.
        VehicleControl vc;

        if(parking){
            cout << "Now Parking" << endl;
            break;
        }
        cout << "Stopped parking" << endl;

    }
    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}
/**
 * This method sets the bool parking passed from DecisionMaker
 */
void Parker::setParking(std::shared_ptr<bool> parking){
    this->parking = parking;
}