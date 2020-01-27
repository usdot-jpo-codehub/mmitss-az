#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include "AsnJ2735Lib.h"
#include "dsrcConsts.h"
#include "json/json.h"
#include "BasicVehicle.h"
#include "SignalRequest.h"
#include "SignalStatus.h"
#include "TransceiverEncoder.h"

const double MPS_TO_KPH_CONVERSION = 3.6;

TransceiverEncoder::TransceiverEncoder()
{
}

int TransceiverEncoder::getMessageType(std::string jsonString)
{
    Json::Value jsonObject;
    Json::Reader reader;
    reader.parse(jsonString.c_str(), jsonObject);

    if ((jsonObject["MsgType"]).asString() == "MAP")
    {
        messageType = MsgEnum::DSRCmsgID_map;
    }

    else if ((jsonObject["MsgType"]).asString() == "BSM")
    {
        messageType = MsgEnum::DSRCmsgID_bsm;
    }

    else if ((jsonObject["MsgType"]).asString() == "SRM")
    {
        messageType = MsgEnum::DSRCmsgID_srm;
    }

    else if ((jsonObject["MsgType"]).asString() == "SPAT")
    {
        messageType = MsgEnum::DSRCmsgID_spat;
    }

    else if ((jsonObject["MsgType"]).asString() == "SSM")
    {
        messageType = MsgEnum::DSRCmsgID_ssm;
    }

    else
    {
        messageType = MsgEnum::DSRCmsgID_unknown;
    }

    return messageType;
}

std::string TransceiverEncoder::TransceiverEncoder::BSMEncoder(std::string jsonString)
{
    BasicVehicle basicVehicle;
    std::stringstream payloadstream{};
    std::string bsmMessagePayload{};
    /// buffer to hold message payload
    size_t bufSize = DsrcConstants::maxMsgSize;
    std::vector<uint8_t> buf(bufSize, 0);
    basicVehicle.json2BasicVehicle(jsonString);
    /// dsrcFrameIn to store input to UPER encoding function
    Frame_element_t dsrcFrameIn;
    dsrcFrameIn.reset();

    /// manual input bsmIn
    dsrcFrameIn.dsrcMsgId = MsgEnum::DSRCmsgID_bsm;
    BSM_element_t &bsmIn = dsrcFrameIn.bsm;
    bsmIn.msgCnt = 1;
    bsmIn.id = basicVehicle.getTemporaryID();
    bsmIn.timeStampSec = static_cast<int16_t>(basicVehicle.getSecMark_Second());
    bsmIn.latitude = DsrcConstants::unit2damega<int32_t>(basicVehicle.getLatitude_DecimalDegree());
    bsmIn.longitude = DsrcConstants::unit2damega<int32_t>(basicVehicle.getLongitude_DecimalDegree());
    bsmIn.elevation = DsrcConstants::unit2deca<int32_t>(basicVehicle.getElevation_Meter());
    bsmIn.yawRate = 0;
    bsmIn.vehLen = 1200;
    bsmIn.vehWidth = 300;
    bsmIn.speed = DsrcConstants::kph2unit<uint16_t>(basicVehicle.getSpeed_MeterPerSecond() * MPS_TO_KPH_CONVERSION);
    bsmIn.heading = DsrcConstants::heading2unit<uint16_t>(basicVehicle.getHeading_Degree());

    /// encode BSM payload
    size_t payload_size = AsnJ2735Lib::encode_msgFrame(dsrcFrameIn, &buf[0], bufSize);
    if (payload_size > 0)
    {
        for (size_t i = 0; i < payload_size; i++)
        {
            payloadstream << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << static_cast<unsigned int>(buf[i]);
        }
    }

    bsmMessagePayload = payloadstream.str();

    return bsmMessagePayload;
}

std::string TransceiverEncoder::TransceiverEncoder::SRMEncoder(std::string jsonString)
{
    SignalRequest signalRequest;
    std::stringstream payloadstream{};
    std::string srmMessagePayload{};
    size_t bufSize = DsrcConstants::maxMsgSize;
    std::vector<uint8_t> buf(bufSize, 0);
    signalRequest.json2SignalRequest(jsonString);

    /// dsrcFrameIn to store input to UPER encoding function
    Frame_element_t dsrcFrameIn;
    dsrcFrameIn.reset();

    /// manual input ssmIn
    dsrcFrameIn.dsrcMsgId = MsgEnum::DSRCmsgID_srm;
    SRM_element_t &srmIn = dsrcFrameIn.srm;
    srmIn.timeStampMinute = signalRequest.getMinuteOfYear();
    srmIn.timeStampSec = static_cast<int16_t>(signalRequest.getMsOfMinute());
    srmIn.msgCnt = static_cast<int8_t>(signalRequest.getMsgCount());
    srmIn.regionalId = static_cast<int16_t>(signalRequest.getRegionalID());
    srmIn.intId = static_cast<int16_t>(signalRequest.getIntersectionID());
    srmIn.reqId = static_cast<int8_t>(signalRequest.getRegionalID());
    srmIn.inApprochId = static_cast<int8_t>(signalRequest.getInBoundApproachID());
    srmIn.inLaneId = static_cast<int8_t>(signalRequest.getInBoundLaneID());
    srmIn.ETAsec = static_cast<int16_t>(signalRequest.getETA_Second());
    srmIn.ETAminute = signalRequest.getETA_Minute();
    srmIn.duration = static_cast<int16_t>(signalRequest.getETA_Duration());
    srmIn.vehId = signalRequest.getTemporaryVehicleID();
    srmIn.latitude = DsrcConstants::unit2damega<int32_t>(signalRequest.getLatitude_DecimalDegree());
    srmIn.longitude = DsrcConstants::unit2damega<int32_t>(signalRequest.getLongitude_DecimalDegree());
    srmIn.elevation = DsrcConstants::unit2deca<int32_t>(signalRequest.getElevation_Meter());
    srmIn.heading = DsrcConstants::heading2unit<uint16_t>(signalRequest.getHeading_Degree());
    srmIn.speed = DsrcConstants::kph2unit<uint16_t>(signalRequest.getSpeed_MeterPerSecond() * MPS_TO_KPH_CONVERSION);
    srmIn.reqType = static_cast<MsgEnum::requestType>(signalRequest.getPriorityRequestType());
    srmIn.vehRole = static_cast<MsgEnum::basicRole>(signalRequest.getBasicVehicleRole());
    srmIn.vehType = static_cast<MsgEnum::vehicleType>(signalRequest.getVehicleType());
    /// encode SSM payload
    size_t payload_size = AsnJ2735Lib::encode_msgFrame(dsrcFrameIn, &buf[0], bufSize);
    if (payload_size > 0)
    {
        for (size_t i = 0; i < payload_size; i++)
        {
            payloadstream << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << static_cast<unsigned int>(buf[i]);
        }
    }

    srmMessagePayload = payloadstream.str();

    return srmMessagePayload;
}

std::string TransceiverEncoder::SPaTEncoder(std::string jsonString)
{
    Json::Value jsonObject;
    Json::Reader reader;

    reader.parse(jsonString.c_str(), jsonObject);

    size_t bufSize = DsrcConstants::maxMsgSize;
    std::vector<uint8_t> buf(bufSize, 0);
    Frame_element_t dsrcFrameIn;
    dsrcFrameIn.dsrcMsgId = MsgEnum::DSRCmsgID_spat;
    SPAT_element_t &spatIn = dsrcFrameIn.spat;
    std::stringstream payloadstream;
    std::string spatMessagePayload;

    spatIn.regionalId = jsonObject["Spat"]["IntersectionState"]["regionalID"].asInt();
    spatIn.id = jsonObject["Spat"]["IntersectionState"]["intersectionID"].asInt();
    spatIn.msgCnt = jsonObject["Spat"]["msgCnt"].asInt();
    spatIn.timeStampMinute = jsonObject["Spat"]["minuteOfYear"].asInt();
    spatIn.timeStampSec = jsonObject["Spat"]["msOfMinute"].asInt();
    std::bitset<16> intersectionStatus(jsonObject["Spat"]["status"].asString());
    spatIn.status = intersectionStatus;
    spatIn.permittedPhases.set(); // all 8 phases permitted
    spatIn.permittedPedPhases.set();

    for (int i = 0; i < 8; i++)
    {
        spatIn.phaseState[i].currState = static_cast<MsgEnum::phaseState>(jsonObject["Spat"]["phaseState"][i]["currState"].asInt());
        spatIn.phaseState[i].startTime = jsonObject["Spat"]["phaseState"][i]["startTime"].asInt();
        spatIn.phaseState[i].minEndTime = jsonObject["Spat"]["phaseState"][i]["minEndTime"].asInt();
        spatIn.phaseState[i].maxEndTime = jsonObject["Spat"]["phaseState"][i]["maxEndTime"].asInt();

        spatIn.pedPhaseState[i].currState = static_cast<MsgEnum::phaseState>(jsonObject["Spat"]["pedPhaseState"][i]["currState"].asInt());
        spatIn.pedPhaseState[i].startTime = jsonObject["Spat"]["pedPhaseState"][i]["startTime"].asInt();
        ;
        spatIn.pedPhaseState[i].minEndTime = jsonObject["Spat"]["pedPhaseState"][i]["minEndTime"].asInt();
        spatIn.pedPhaseState[i].maxEndTime = jsonObject["Spat"]["pedPhaseState"][i]["maxEndTime"].asInt();
    }
    size_t payload_size = AsnJ2735Lib::encode_msgFrame(dsrcFrameIn, &buf[0], bufSize);

    if (payload_size > 0)
    {
        for (size_t i = 0; i < payload_size; i++)
        {
            payloadstream << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << static_cast<unsigned int>(buf[i]);
        }
    }
    spatMessagePayload = payloadstream.str();
    return spatMessagePayload;
}

std::string TransceiverEncoder::SSMEncoder(std::string jsonString)
{
    SignalStatus signalStatus;
    std::stringstream payloadstream{};
    std::string ssmMessagePayload{};
    size_t bufSize = DsrcConstants::maxMsgSize;
    std::vector<uint8_t> buf(bufSize, 0);
    signalStatus.json2SignalStatus(jsonString);

    int *vehicleID = signalStatus.getTemporaryVehicleID();
    int *requestID = signalStatus.getRequestID();
    int *msgCount = signalStatus.getMsgCount();
    int *inBoundLaneID = signalStatus.getInBoundLaneID();
    int *inBoundApproachID = signalStatus.getInBoundApproachID();
    int *basicVehicleRole = signalStatus.getBasicVehicleRole();
    int *expectedTimeOfArrival_Minute = signalStatus.getETA_Minute();
    double *expectedTimeOfArrival_Second = signalStatus.getETA_Second();
    double *expectedTimeOfArrival_Duration = signalStatus.getETA_Duration();
    int *priorityRequestStatus = signalStatus.getPriorityRequestStatus();

    /// dsrcFrameIn to store input to UPER encoding function
    Frame_element_t dsrcFrameIn;
    dsrcFrameIn.reset();

    //Get the values of the variables
    dsrcFrameIn.dsrcMsgId = MsgEnum::DSRCmsgID_ssm;
    SSM_element_t &ssmIn = dsrcFrameIn.ssm;
    ssmIn.timeStampMinute = signalStatus.getMinuteOfYear();
    ssmIn.timeStampSec = static_cast<int16_t>(signalStatus.getMsOfMinute());
    ssmIn.msgCnt = static_cast<int8_t>(signalStatus.getPRSSequenceNumber());
    ssmIn.updateCnt = static_cast<int8_t>(signalStatus.getPRSUpdateCount());
    ssmIn.regionalId = static_cast<int16_t>(signalStatus.getRegionalID());
    ssmIn.id = static_cast<int16_t>(signalStatus.getIntersectionID());

    for (int i = 0; i < signalStatus.getNoOfRequest(); i++)
    {
        SignalRequetStatus_t requestStatus;
        requestStatus.reset();
        requestStatus.vehId = vehicleID[i];
        requestStatus.reqId = static_cast<int8_t>(requestID[i]);
        requestStatus.sequenceNumber = static_cast<int8_t>(msgCount[i]);
        requestStatus.vehRole = static_cast<MsgEnum::basicRole>((basicVehicleRole[i]));
        requestStatus.inLaneId = static_cast<int8_t>(inBoundLaneID[i]);
        requestStatus.inApprochId = static_cast<int8_t>(inBoundApproachID[i]); //It is not present in TestDecoder
        requestStatus.ETAminute = expectedTimeOfArrival_Minute[i];
        requestStatus.ETAsec = static_cast<int16_t>(expectedTimeOfArrival_Second[i]);
        requestStatus.duration = static_cast<int16_t>(expectedTimeOfArrival_Duration[i]);
        requestStatus.status = static_cast<MsgEnum::requestStatus>(priorityRequestStatus[i]);

        ssmIn.mpSignalRequetStatus.push_back(requestStatus);
    }
    /// encode SSM payload
    size_t payload_size = AsnJ2735Lib::encode_msgFrame(dsrcFrameIn, &buf[0], bufSize);

    if (payload_size > 0)
    {
        for (size_t i = 0; i < payload_size; i++)
        {
            payloadstream << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << static_cast<unsigned int>(buf[i]);
        }
    }
    ssmMessagePayload = payloadstream.str();

    return ssmMessagePayload;
}

TransceiverEncoder::~TransceiverEncoder()
{
}
