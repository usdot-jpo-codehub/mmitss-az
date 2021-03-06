#include "../TransceiverDecoder.h"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <UdpSocket.h>
#include "json/json.h"

int main()
{
    Json::Value jsonObject_config;
    Json::Reader reader;
    std::ifstream configJson("/nojournal/bin/mmitss-phase3-master-config.json");
    std::string configJsonString((std::istreambuf_iterator<char>(configJson)), std::istreambuf_iterator<char>());
    reader.parse(configJsonString.c_str(), jsonObject_config);

    TransceiverDecoder decoder;
    UdpSocket decoderSocket(static_cast<short unsigned int>(jsonObject_config["PortNumber"]["HostBsmDecoder"].asInt()));
    char receiveBuffer[5120];

    const string LOCALHOST = jsonObject_config["HostIp"].asString();
    int bsmReceiverPortNo = (jsonObject_config["PortNumber"]["PriorityRequestGenerator"]).asInt();
    const int dataCollectorPortNo = (jsonObject_config["PortNumber"]["DataCollector"]).asInt();

    std::string receivedPayload{};
    std::string extractedPayload{};
    std::string bsmJsonString{};
    std::string applicationPlatform = decoder.getApplicationPlatform();

    while (true)
    {
        receivedPayload = decoderSocket.receivePayloadHexString();

        size_t pos = receivedPayload.find("0014");
        if (pos != std::string::npos)
        {
            extractedPayload = receivedPayload.erase(0, pos);
            bsmJsonString = decoder.bsmDecoder(extractedPayload);
            std::cout << "Decoded HostBSM" << std::endl;
            std::cout << bsmJsonString << std::endl;
            decoderSocket.sendData(LOCALHOST, static_cast<short unsigned int>(bsmReceiverPortNo), bsmJsonString);
            decoderSocket.sendData(LOCALHOST, static_cast<short unsigned int>(dataCollectorPortNo), bsmJsonString);
        }

        if (decoder.sendSystemPerformanceDataLog() == true && applicationPlatform == "vehicle")
            decoderSocket.sendData(LOCALHOST, static_cast<short unsigned int>(dataCollectorPortNo), decoder.createJsonStringForSystemPerformanceDataLog("HostBSM"));
    }

    return 0;
}
