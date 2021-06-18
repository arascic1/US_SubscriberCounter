#define TEMAPROJEKAT "ugradbeni/projekat/youtube"

#include "mbed.h"

#define MQTTCLIENT_QOS2 0

#include "easy-connect.h"
#include "MQTTNetwork.h"
#include "MQTTmbed.h"
#include "MQTTClient.h"
#include <string.h>
#include <vector>
#include <string>

#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"

TS_StateTypeDef TS_State = { 0 };

DigitalIn button1(p5);
DigitalIn button2(p6);

int channelCount = 0;
int currentChannel = 0;
int usernameLength = 0;
char *channelData;

std::vector<std::string> channelsNames;
std::vector<std::string> channelsSubscribers;
std::vector<std::string> channelsViews;
std::vector<std::string> channelsVideos;
std::vector<std::string> channelsCountries;

void getDefaultValues(){
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_FillRect(0, 0, BSP_LCD_GetXSize(), 50);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_RED);
}

void drawChannelNameBanner(){
    int n = 0;
    if(currentChannel > 0)
        n = channelsNames[currentChannel - 1].length();
    else
        n = channelsNames[0].length();
    char pom[n];
    if(n != 0)
        strcpy(pom, channelsNames[currentChannel - 1].c_str());
    else
        strcpy(pom, " ");
    BSP_LCD_FillRect(0, 50, BSP_LCD_GetXSize(), 40);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_DisplayStringAt(0, 65, (uint8_t *)"Channel: ", LEFT_MODE);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(0, 63, (uint8_t *)pom, RIGHT_MODE);
}

void drawYouTubeTriangleLogo(){
    Point points[3];
    points[0].X = 30;   points[0].Y = 15;
    points[1].X = 50;  points[1].Y = 25;
    points[2].X = 30;   points[2].Y = 35;
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_FillPolygon(points, 3);
}

void drawAppTitleBanner(){
    getDefaultValues();
    drawYouTubeTriangleLogo();

    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(0, 15, (uint8_t *)"YouTube", RIGHT_MODE);
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_DisplayStringAt(0, 30, (uint8_t *)"Subscriber Counter", RIGHT_MODE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
}

void drawInfoFrame(){
   BSP_LCD_SetTextColor(LCD_COLOR_RED);
   BSP_LCD_DrawRect(0, 90, BSP_LCD_GetXSize()-1, 109);
}

void setChannelInfo(){
    drawInfoFrame();
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font16);

    char channelNumber[20];
    if(channelCount != 0)
        sprintf(channelNumber, "Channel number: %d", currentChannel);
    else
        sprintf(channelNumber, "Channel number: ");
    ////////////////////////////////////////////////////////////////////
    char subscribers[50];
    if(channelsSubscribers[0].length() != 0)
        sprintf(subscribers, "Subscribers: %s", channelsSubscribers[currentChannel - 1].c_str());
    else
        sprintf(subscribers, "Subscribers: ");
    //////////////////////////////////////////////////////////////////
    char views[50];
    if(channelsViews[0].length() != 0)
        sprintf(views, "Views: %s", channelsViews[currentChannel - 1].c_str());
    else
        sprintf(views, "Views: ");
    //////////////////////////////////////////////////////////////////
    char uploads[20];
    if(channelsVideos[0].length() != 0)
        sprintf(uploads, "Uploads: %s", channelsVideos[currentChannel - 1].c_str());
    else
        sprintf(uploads, "Uploads: ");
    /////////////////////////////////////////////////////////////////
    char country[20];
    if(channelsCountries[0].length() != 0)
        sprintf(country, "Country: %s", channelsCountries[currentChannel - 1].c_str());
    else
        sprintf(country, "Country: ");

    BSP_LCD_DisplayStringAt(0, 95, (uint8_t *)channelNumber, LEFT_MODE);
    BSP_LCD_DisplayStringAt(0, 115, (uint8_t *)subscribers, LEFT_MODE);
    BSP_LCD_DisplayStringAt(0, 135, (uint8_t *)views, LEFT_MODE);
    BSP_LCD_DisplayStringAt(0, 155, (uint8_t *)uploads, LEFT_MODE);
    BSP_LCD_DisplayStringAt(0, 175, (uint8_t *)country, LEFT_MODE);
}

void drawInstructionBanner(){
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_FillRect(0, BSP_LCD_GetYSize()-40, BSP_LCD_GetXSize(), 40);
    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_DisplayStringAt(0, 205, (uint8_t *)"Button 1: Next Channel", LEFT_MODE);
    BSP_LCD_DisplayStringAt(0, 220, (uint8_t *)"Button 2: Previous Channel", LEFT_MODE);
}

void setLCD(){
    BSP_LCD_Init();
    drawAppTitleBanner();
    drawChannelNameBanner();
    setChannelInfo();
    drawInstructionBanner();
}


void getChannelUploadsAndPutIntoArray(char *channelInfo) {
    std::string delimiter = ";";
    size_t pos = 0;
    std::string channelInfoString = channelInfo;
    std::string info;

    pos = channelInfoString.find(delimiter);
    info = channelInfoString.substr(0, pos);
    channelsVideos.push_back(info);
    channelInfoString.erase(0, pos + delimiter.length());
}

void getChannelSubscribersAndPutIntoArray(char *channelInfo) {
    std::string delimiter = ";";
    size_t pos = 0;
    std::string channelInfoString = channelInfo;
    std::string info;

    pos = channelInfoString.find(delimiter);
    info = channelInfoString.substr(0, pos);
    channelsSubscribers.push_back(info);
    channelInfoString.erase(0, pos + delimiter.length());

    char *channelData;
    strcpy(channelData, channelInfoString.c_str());
    getChannelUploadsAndPutIntoArray(channelData);
}

void getChannelViewsAndPutIntoArray(char *channelInfo) {
    std::string delimiter = ";";
    size_t pos = 0;
    std::string channelInfoString = channelInfo;
    std::string info;

    pos = channelInfoString.find(delimiter);
    info = channelInfoString.substr(0, pos);
    channelsViews.push_back(info);
    channelInfoString.erase(0, pos + delimiter.length());

    char *channelData;
    strcpy(channelData, channelInfoString.c_str());
    getChannelSubscribersAndPutIntoArray(channelData);
}

void getChannelCountriesAndPutIntoArray(char *channelInfo) {
    std::string delimiter = ";";
    size_t pos = 0;
    std::string channelInfoString = channelInfo;
    std::string info;

    pos = channelInfoString.find(delimiter);
    info = channelInfoString.substr(0, pos);
    channelsCountries.push_back(info);
    channelInfoString.erase(0, pos + delimiter.length());

    char *channelData;
    strcpy(channelData, channelInfoString.c_str());
    printf("%s", channelData);
    getChannelViewsAndPutIntoArray(channelData);
}

void getChannelNameAndPutIntoArray(char *channelInfo) {
    std::string delimiter = ";";
    size_t pos = 0;
    std::string channelInfoString = channelInfo;
    std::string info;

    pos = channelInfoString.find(delimiter);
    info = channelInfoString.substr(0, pos);
    channelsNames.push_back(info);
    channelInfoString.erase(0, pos + delimiter.length());

    char *channelData;
    strcpy(channelData, channelInfoString.c_str());
    printf("%s", channelData);
    getChannelCountriesAndPutIntoArray(channelData);
}

void removeChannel(int channelNumber) {
    channelCount--;
    if(currentChannel == channelNumber)
        currentChannel--;
    channelsNames.erase(channelsNames.begin() + channelNumber - 1);
    channelsSubscribers.erase(channelsSubscribers.begin() + channelNumber - 1);
    channelsVideos.erase(channelsVideos.begin() + channelNumber - 1);
    channelsViews.erase(channelsViews.begin() + channelNumber - 1);
    channelsCountries.erase(channelsCountries.begin() + channelNumber - 1);
    setLCD();
}

void messageArrived(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    ++channelCount;
    if(currentChannel == 0) ++currentChannel;
    channelData = (char*)message.payload;
    if(!(channelData[0] >= '0' && channelData[0] <= '9')){
        getChannelNameAndPutIntoArray(channelData);
        if(channelCount > 1)
            ++currentChannel;
        setLCD();
    }
    else{
        std::string deleted = channelData;
        int deletedIndex = stoi(deleted);
        removeChannel(deletedIndex);
    }
}

int main() {

    BSP_LCD_Init();
    setLCD();

    printf("Ugradbeni sistemi\r\n");
    printf("Test projekta\r\n\r\n");

    NetworkInterface *network;
    network = NetworkInterface::get_default_instance();

    if (!network) {
        return -1;
    }

    MQTTNetwork mqttNetwork(network);

    MQTT::Client<MQTTNetwork, Countdown> client(mqttNetwork);

    const char* hostname = "broker.hivemq.com";
    int port = 1883;
    printf("Connecting to %s:%d\r\n", hostname, port);
    int rc = mqttNetwork.connect(hostname, port);
    if (rc != 0)
        printf("rc from TCP connect is %d\r\n", rc);

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = "ugradbeni";
    data.username.cstring = "";
    data.password.cstring = "";
    if ((rc = client.connect(data)) != 0)
        printf("rc from MQTT connect is %d\r\n", rc);

    if ((rc = client.subscribe(TEMAPROJEKAT, MQTT::QOS2, messageArrived)) != 0)
        printf("rc from MQTT subscribe is %d\r\n", rc);

    MQTT::Message message;

    char buf[100];

    while (1) {
            BSP_TS_GetState(&TS_State);

            rc = client.subscribe(TEMAPROJEKAT, MQTT::QOS0, messageArrived);

            if(button2){
                if(currentChannel < channelsNames.size())
                    currentChannel++;
                setLCD();
            }

            if(button1){
                if(currentChannel > 1)
                    currentChannel--;
                setLCD();
            }

            wait_ms(10);
        }

}
