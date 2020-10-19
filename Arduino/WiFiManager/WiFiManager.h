/* Written by JelleWho https://github.com/jellewie
   https://github.com/jellewie/Arduino-WiFiManager

   Basic function:
   1. Load EEPROM data (if not yet done)
   2. while(no data) Set up AP mode and wait for user data
   3. try connecting, if (not) {GOTO 2}
*/
#ifndef WiFiManager_h                               //This prevents including this file more than once
#define WiFiManager_h

//#define WiFiManager_SerialEnabled               	//Disable to not send Serial debug feedback
//#define dnsServerEnabled							//Not stable yet
#include <WebServer.h>								//includes <WiFi.h>
#include "Arduino.h"
#include <EEPROM.h>
#ifdef dnsServerEnabled
#include <DNSServer.h>
DNSServer dnsServer;
#endif //dnsServerEnabled
WebServer server(80);

#ifndef WiFiManagerUser_VariableNames_Defined
const String WiFiManager_VariableNames[] = {"SSID", "Password"};
const byte WiFiManager_Settings = sizeof(WiFiManager_VariableNames) / sizeof(WiFiManager_VariableNames[0]); //Why filling this in if we can automate that? :)
#endif //WiFiManagerUser_VariableNames_Defined

class CWiFiManager {
  private:
	const char EEPROM_Seperator = char(9);          //use 'TAB' as a seperator
    const int ConnectionTimeOutMS = 10000;
    bool SettingsEnabled = false;                   //This holds the flag to enable settings, else it would not responce to settings commands
    bool WaitOnAPMode = true;                       //This holds the flag if we should wait in Apmode for data
	bool TickEveryMS(int);
    byte APMode();                           		//Start a WIFI APmode
    bool Connect(int);
	String LoadEEPROM();                     		//Get raw data from EEPROM
  public:
    int EEPROM_USED = 0;                            //Howmany bytes we have used for data in the EEPROM
	//#define strip_ip, gateway_ip, subnet_mask to use static IP
#ifndef WiFiManagerUser_password_Defined
	char password[16] = "";                         //Also defines howmany characters can be in the SSID
#endif //WiFiManagerUser_password_Defined
#ifndef WiFiManagerUser_ssid_Defined
	char ssid[16] = "";                             //^
#endif //WiFiManagerUser_ssid_Defined
#ifndef WiFiManagerUser_APSSID
	char APSSID[16] = "ESP32";                      //If you want to define the name somewhere else use 'char* APSSID = Name'
#endif //WiFiManagerUser_APSSID_Defined
#ifndef WiFiManagerUser_VariableNames_Defined
	const int EEPROM_size = 33;                     //Set EEPROM size to default: Max Amount of chars of 'SSID + PASSWORD' (+1)
#endif //WiFiManagerUser_VariableNames_Defined
    void Status_Start();
    void Status_Done();
    void Status_Blink();
    void Status_StartAP();
    bool HandleAP();                     			//Called when in the While loop in APMode, this so the user can cancel it
	bool Set_Value(byte, String);
    String Get_Value(byte, bool, bool);
	
    void StartServer();                       		//Start the webserver
    void EnableSetup(bool);            				//Enable/disable setup page
	void RunServer();								//Proces server client commands if needed
	void handle_Connect();
    void handle_Settings();
	bool WriteEEPROM();
	bool ClearEEPROM();
	bool CheckAndReconnectIfNeeded(bool);
	byte Start();                             		//Start all WIFI stuff
	byte LoadData();                          		//Only load data from EEPROM to memory
};

CWiFiManager WiFiManager;

#include "WiFiManager.cpp"
#endif
