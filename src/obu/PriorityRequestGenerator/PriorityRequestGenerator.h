/*
**********************************************************************************
 © 2019 Arizona Board of Regents on behalf of the University of Arizona with rights
       granted for USDOT OSADP distribution with the Apache 2.0 open source license.
**********************************************************************************
  PriorrityRequestGenerator.h
  Created by: Debashis Das
  University of Arizona   
  College of Engineering
  This code was developed under the supervision of Professor Larry Head
  in the Systems and Industrial Engineering Department.
  Revision History:
  1. This script is the header file for PriorrityRequestGenerator.cpp
*/

#pragma once
#include <list>
#include <string>
#include "BasicVehicle.h"
#include "SignalStatus.h"
#include "SignalRequest.h"
#include "ActiveRequest.h"
#include "MapManager.h"



class PriorityRequestGenerator
{
private:
    std::vector<ActiveRequest>ActiveRequestTable;
    bool bgetActiveMap {false}; //This variables will be used by while checking if vehicle needs to send srm or not. If there is active map the value of this variable will true
    int messageType{};
    int temporaryVehicleID{};
    std::string mapFileDirectory; 
    std::string mapFileName;
    int vehicleLaneID{};
    int vehicleAprroachID{};
	double time2go{};
    int intersectionID{};
    int regionalID{};
    int vehicleIntersectionStatus{};
    int msgCount{};
    int vehicleType{};
    int basicVehicleRole{};
    int priorityRequestType{};
    double tempVehicleSpeed{}; //tempVehicleSpeed store the vehicle speed of last send out srm. use it to check the speed change. will be set vehicle minimum speed when out of the intersection
    
public:
    PriorityRequestGenerator();
    ~PriorityRequestGenerator();

    int getMessageType(std::string jsonString);
    std::string getActiveMapFileName(MapManager mapManager);
    std::string getActiveMapFileDirectory(MapManager mapManager);
    void getVehicleInformationFromMAP(MapManager mapManager, BasicVehicle basicVehicle);
    void setIntersectionID(int vehicleNearByIntersectionId);
    void setRegionalID(int vehicleNearByRegionalId);
    void setLaneID(int laneId);
    void setApproachID(int approachID);
    bool setTime2Go(double distance2go, double vehicleSpeed);
    void setVehicleIntersectionStatus(int vehIntersectionStatus);
    int getIntersectionID();
    int getRegionalID();
    int getVehicleID(BasicVehicle basicVehicle);
    int getLaneID();
    int getApproachID();
    double getTime2Go();
    int getVehicleIntersectionStatus();
    void deleteActiveMapfromList(MapManager mapManager);
    bool addToActiveRequestTable(SignalStatus signalStatus);
    std::vector<ActiveRequest> creatingSignalRequestTable(SignalStatus signalStatus);
    void printART();
    bool shouldSendOutRequest(BasicVehicle basicVehicle, MapManager mapManager);
    int getVehicleType();
    int getBasicVehicleRole();
    int getPriorityRequestType(BasicVehicle basicVehicle);
    int getMinuteOfYear();
    int getMsOfMinute();
    int getMsgCount();
    std::string createSRMJsonObject(BasicVehicle basicVehicle, SignalRequest signalRequest);

};