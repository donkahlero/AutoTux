//
// Created by niklas on 2016-04-08.
//

#include <iostream>

#include "decisionmaker/DecisionMaker.h"
#include "overtaker/Overtaker.h"

using namespace std;

using namespace odcore::base;
using namespace odcore::base::module;
using namespace odcore::data;               // Container
using namespace automotive;                 // Vehicle Control & Vehicle Data
using namespace automotive::miniature;      // Sensor Board Data
using namespace automotive::vehicle;        // For reversing light

using namespace autotux;                    // For own data structures

using namespace decisionmaker;
using namespace overtaker;

enum STATE {DRIVING, PARKING};

VehicleControl vehicleControl;

/**
 * Constructor
 */
DecisionMaker::DecisionMaker(const int32_t &argc, char **argv) :
        TimeTriggeredConferenceClientModule(argc, argv, "DecisionMaker"),
        ovt(), parker(), containerVehicleData(), containerSensorBoardData(), containerDecisionMakerMSG(), laneRecommendation(),
        speed(), isStopLine(false), stopCounter(0){
}

DecisionMaker::~DecisionMaker() {}

void DecisionMaker::setUp(){
    cout << "DecisionMaker is setting up" << endl;
}
void DecisionMaker::tearDown(){
    cout << "DecisionMaker shuts down" << endl;
}
/**
 * Sets wheelangledata to the LaneRecommandation
*/
void DecisionMaker::laneFollowing() {

    if(stopCounter > 0) {

        if(stopCounter == 90) {
            //cout << "WAKING UP" << endl;
            stopCounter = 0;
            isStopLine = false;
            vehicleControl.setBrakeLights(false);
        }

        else {
            //cout << "SLEEPING..." << endl;
            stopCounter++;
        }
    }
    else if(getDistanceToLine() < 50 && getDistanceToLine() != -1) {
        cout << "STOPPING!" << endl;
        vehicleControl.setBrakeLights(true);
        speed = 0;
        stopCounter = 1;
        isStopLine = true;
    }

    else if(getDistanceToLine() < 150 && getDistanceToLine() != -1) {
        cout << "Slowing down..." << endl;
        vehicleControl.setBrakeLights(false);
        speed = 1;
    }

    //cout << "Distance to line: " << getDistanceToLine() << endl;

    vehicleControl.setSpeed(speed);
    vehicleControl.setSteeringWheelAngle(getAngle());
}

/**
 * Gets the angle given by LaneRecommandation
 */
double DecisionMaker::getAngle() {
    return laneRecommendation.getData<LaneRecommendationMSG>().getAngle();
}
/**
 * True if Data sent by the LaneRecommendation are considered reliable.
 */
bool DecisionMaker::isDataQuality(){
    return laneRecommendation.getData<LaneRecommendationMSG>().getQuality();

}
/**
 * Get's the distance from a stop Line
 * @TODO is not found yet
 */
double DecisionMaker::getDistanceToLine() {
    return laneRecommendation.getData<LaneRecommendationMSG>().getDistance_to_line();

}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode DecisionMaker::body() {
    LIFOQueue lifoQueue;
    addDataStoreFor(lifoQueue);

    // Set initial state of the car
    STATE state = PARKING;

    VehicleData vd;
    SensorBoardData sbd;
    DecisionMakerMSG dmMSG;
    OvertakingMSG ovtMSG;
    LightSystem lightSystem;


    // Set initial speed
    vehicleControl.setSpeed(2.0);

    while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {

        // 1. Update sensor board data values
        containerSensorBoardData = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());
        sbd = containerSensorBoardData.getData<SensorBoardData>();

        // 2. Update vehicle data values
        containerVehicleData = getKeyValueDataStore().get(automotive::VehicleData::ID());
        vd = containerVehicleData.getData<VehicleData>();

        laneRecommendation = getKeyValueDataStore().get(autotux::LaneRecommendationMSG::ID());

        containerDecisionMakerMSG = getKeyValueDataStore().get(autotux::DecisionMakerMSG::ID());
        dmMSG = containerDecisionMakerMSG.getData<DecisionMakerMSG>();

        //state = dmMSG.getState();

        cout << "SensorValues BACK RIGHT: " <<  sbd.getValueForKey_MapOfDistances(2) << endl;
        //cout << "Distance: " << vd.getAbsTraveledPath() << endl;

	double frontUsSensor = sbd.getValueForKey_MapOfDistances(4);
   // 	cout << "DecisionMaker US Sensor: " << frontUsSensor << endl;
	cout << "SensorValues FROM 5: " <<  sbd.getValueForKey_MapOfDistances(5) << endl;
	cout << "SensorValues FRONT RIGHT: " <<  sbd.getValueForKey_MapOfDistances(6) << endl;
	cout << "SensorValues FRONT RIGHT: " <<  sbd.getValueForKey_MapOfDistances(0) << endl;
	cout << "SensorValues Back: " <<  sbd.getValueForKey_MapOfDistances(1) << endl;

        if(!ovt.isLeftLane()){
            ovtMSG.setLeftlane(NOTLEFTLANE);
        }
        else
            ovtMSG.setLeftlane(LEFTLANE);

        switch (state){
            case DRIVING:{
                ovt.obstacleDetection(sbd, vd, vehicleControl);

                    // If overtaker is overriding control values...
                if(ovt.getIsOverriding()) {
                    //cout << "DM: OVERTAKER is OVERRIDING" << endl;
                    vehicleControl = ovt.getOvtControl();
                }
                    //... else follow lane-follower instructions...
                else{
                    //cout <<"DM: LANE FOLLOWER Instructions" << endl;
                    laneFollowing();
                }

                break;
            }
            case PARKING:{

                if(parker.getFoundSpot()){
                    if(parker.isReversing()){
                        lightSystem.setReverseLight(true);
                    }
                    else
                        lightSystem.setReverseLight(false);

                    if(!parker.getIsParked()) {
			cout << "HELLO I WILL PARK NOW!" << endl;
                        vehicleControl = parker.parallelPark(sbd, vd);
                    }
                    else {
                        cout << "NOW PARKED!!!" << endl;
                        break;
                    }
                }
                else{
                    if(!isStopLine) {
                        parker.findSpot(sbd, vd, vehicleControl);
                        speed = 1;
                    }
                    laneFollowing();
                }
                break;
            }
        }
        // Pack and send containers
        Container control(vehicleControl);
        Container lane(ovtMSG);
        Container lights(lightSystem);
        getConference().send(control);
        getConference().send(lane);
        getConference().send(lights);
    }

    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}
