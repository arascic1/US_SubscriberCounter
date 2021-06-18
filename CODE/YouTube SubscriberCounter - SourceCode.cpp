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

int channelCount = 0;
int usernameLength = 0;
char *channelLink;
char *option;
char *channelUsername;
std::vector<std::string> channelsNames;

void getDefaultValues(){
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_FillRect(0, 0, BSP_LCD_GetXSize(), 50);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_RED);
}

void drawChannelNameBanner(){
    /*channelsNames.push_back("Dnevnjak");
    channelsNames.push_back("MrBeast");
    int n = channelsNames[1].length();
    char pom[n];
    strcpy(pom, channelsNames[1].c_str());*/
    BSP_LCD_FillRect(0, 50, BSP_LCD_GetXSize(), 40);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_DisplayStringAt(0, 65, (uint8_t *)"Channel: ", LEFT_MODE);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(0, 63, (uint8_t *)/*pom*/"Drzavni posao", RIGHT_MODE);
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
    BSP_LCD_DisplayStringAt(0, 95, (uint8_t *)"Channel Number: 1", LEFT_MODE);
    BSP_LCD_DisplayStringAt(0, 115, (uint8_t *)"Subscribers: 318000", LEFT_MODE);
    BSP_LCD_DisplayStringAt(0, 135, (uint8_t *)"Views: 572214256", LEFT_MODE);
    BSP_LCD_DisplayStringAt(0, 155, (uint8_t *)"Uploads: 1615", LEFT_MODE);
    BSP_LCD_DisplayStringAt(0, 175, (uint8_t *)"Country: Serbia", LEFT_MODE);
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

void findChannelUsername(char *linkYT){
    char *username = linkYT + 24;
    usernameLength -= 28;
    if(username[0] == 'c'){
        username = username + 8;
        usernameLength -= 8;
        printf("ChannelId: %.*s\n", usernameLength, username);
        //ovdje pozvati funkciju za JSON; bez drugog parametra
    }
    else if(username[0] == 'u'){
        username = username + 5;
        usernameLength -= 5;
        printf("Username: %.*s\n", usernameLength, username);
        //ovdje pozvati funkciju za JSON; drugi parametar = true
    }
}

void messageArrived(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    ++channelCount;
    option = (char*)message.payload;
    option[3] = '\n';
    channelLink=(char*)message.payload + 4;
    usernameLength = message.payloadlen;
    if(option[0] == 'a'){
        findChannelUsername(channelLink);
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

            wait_ms(10);
        }

}
