#include <SoftwareSerial.h>
#include "jsonlib.h"
#include <Wire.h>
#include <TinyGPS.h>
#include <APDS9930.h>
#include <Tone32.h>

uint8_t buzzinger;
#define DEFAULT_TIMEOUT 5000
uint32_t st = 0;
int quantity ;
String t;
// Pins
#define APDS9930_INT    13  // Needs to be an interrupt pin
//#define LED_PIN         22 // LED for showing interrupt

// Constants
#define PROX_INT_HIGH   390 // Proximity level for interrupt
#define PROX_INT_LOW    0  // No far interrupt

APDS9930 apds = APDS9930();
TinyGPS gps;

enum PowerMode {MINIMUM, NORMAL, POW_UNKNOWN, SLEEP, POW_ERROR};
enum NetworkRegistration {NOT_REGISTERED, REGISTERED_HOME, SEARCHING, DENIED, NET_UNKNOWN, REGISTERED_ROAMING, NET_ERROR};
//micro definitions for fingerPrint
const unsigned int IRQ = 3;
uint8_t v = 12;

// Internal memory for the shared buffer
// Used for all reception of message from the module
char *internalBuffer;
uint16_t internalBufferSize = 0;

// Reception buffer
char *recvBuffer;
uint16_t recvBufferSize = 0;
uint16_t dataSize = 0;

/* ====== pins on D1 MINI
   GPS (2, 0) D4, D3
   GSM (12, 14) D5, D6
*/
//Devices access

SoftwareSerial sGps(14, 12);
SoftwareSerial nano(16,17);
//SoftwareSerial gsm(12,14);
SoftwareSerial* stream = new SoftwareSerial(26, 27);



//declared functions.
int16_t strIndex(const char* str, const char* findStr, uint16_t startIdx = 0);
bool readResponse(uint16_t timeout, uint8_t crlfToWait = 2);
bool readResponseCheckAnswer_P(uint16_t timeout, const char* expectedAnswer, uint8_t crlfToWait = 2);
//#define FPSerial Serial1
//DFRobot_ID809 fingerprint;
bool db = true;
//some global var
const char AT_CMD_BASE[] PROGMEM = "AT";                                      // Basic AT command to check the link
const char AT_CMD_ECHO[] PROGMEM = "ATE1&W";                                  // Set command echo mode

const char AT_CMD_CPIN_TEST[] PROGMEM = "AT+CPIN?";                           // Check SIM card status
const char AT_CMD_CPIN_PIN[] PROGMEM = "AT+CPIN=";                            // Configure PIN code

const char AT_CMD_CSQ[] PROGMEM = "AT+CSQ";                                   // Check the signal strengh
const char AT_CMD_ATI[] PROGMEM = "ATI";                                      // Output version of the module
const char AT_CMD_GMR[] PROGMEM = "AT+GMR";                                   // Output version of the firmware
const char AT_CMD_SIM_CARD[] PROGMEM = "AT+CCID";                              // Get Sim Card version

const char AT_CMD_CFUN_TEST[] PROGMEM = "AT+CFUN?";                           // Check the current power mode
const char AT_CMD_CFUN0[] PROGMEM = "AT+CFUN=0";                              // Switch minimum power mode
const char AT_CMD_CFUN1[] PROGMEM = "AT+CFUN=1";                              // Switch normal power mode
const char AT_CMD_CFUN4[] PROGMEM = "AT+CFUN=4";                              // Switch sleep power mode

const char AT_CMD_CREG_TEST[] PROGMEM = "AT+CREG?";// Check the network registration status
const char AT_CMD_CSTT[] PROGMEM = "AT+CSTT=";
const char AT_CMD_CIICR[] PROGMEM = "AT+CIICR=";
const char AT_CMD_CIFSR[] PROGMEM = "AT+CIFSR=";
const char AT_CMD_SAPBR_GPRS[] PROGMEM = "AT+SAPBR=3,1,\"Contype\",\"GPRS\""; // Configure the GPRS bearer
const char AT_CMD_SAPBR_APN[] PROGMEM = "AT+SAPBR=3,1,\"APN\",";              // Configure the APN for the GPRS
const char AT_CMD_SAPBR_USER[] PROGMEM = "AT+SAPBR=3,1,\"USER\",";            // Configure the USER for the GPRS (linked to APN)
const char AT_CMD_SAPBR_PWD[] PROGMEM = "AT+SAPBR=3,1,\"PWD\",";              // Configure the PWD for the GPRS (linked to APN)
const char AT_CMD_CIPSHUT[] PROGMEM = "AT+CIPSHUT";
const char AT_CMD_SAPBR1[] PROGMEM = "AT+SAPBR=1,1";                          // Connect GPRS
const char AT_CMD_SAPBR2[] PROGMEM = "AT+SAPBR=2,1";                          // Check GPRS connection status
const char AT_CMD_SAPBR0[] PROGMEM = "AT+SAPBR=0,1";                          // Disconnect GPRS

const char AT_CMD_HTTPINIT[] PROGMEM = "AT+HTTPINIT";                         // Init HTTP connection
const char AT_CMD_HTTPPARA_CID[] PROGMEM = "AT+HTTPPARA=\"CID\",1";           // Connect HTTP through GPRS bearer
const char AT_CMD_HTTPPARA_URL[] PROGMEM = "AT+HTTPPARA=\"URL\",";            // Define the URL to connect in HTTP
const char AT_CMD_HTTPPARA_USERDATA[] PROGMEM = "AT+HTTPPARA=\"USERDATA\",";  // Define the header(s)
const char AT_CMD_HTTPPARA_CONTENT[] PROGMEM = "AT+HTTPPARA=\"CONTENT\",";    // Define the content type for the HTTP POST
const char AT_CMD_HTTPPARA_REDIR[] PROGMEM = "AT+HTTPPARA=\"REDIR\",1";       // Enable HTTP redirection
const char AT_CMD_HTTPSSL_Y[] PROGMEM = "AT+HTTPSSL=1";                       // Enable SSL for HTTP connection
const char AT_CMD_HTTPSSL_N[] PROGMEM = "AT+HTTPSSL=0";                       // Disable SSL for HTTP connection
const char AT_CMD_HTTPACTION0[] PROGMEM = "AT+HTTPACTION=0";                  // Launch HTTP GET action
const char AT_CMD_HTTPACTION1[] PROGMEM = "AT+HTTPACTION=1";                  // Launch HTTP POST action
const char AT_CMD_HTTPREAD[] PROGMEM = "AT+HTTPREAD";                         // Start reading HTTP return data
const char AT_CMD_HTTPTERM[] PROGMEM = "AT+HTTPTERM";                         // Terminate HTTP connection

const char AT_RSP_OK[] PROGMEM = "OK";                                        // Expected answer OK
const char AT_RSP_DOWNLOAD[] PROGMEM = "DOWNLOAD";                            // Expected answer DOWNLOAD
const char AT_RSP_HTTPREAD[] PROGMEM = "+HTTPREAD: ";                         // Expected answer HTTPREAD
const char AT_RSP_SAPBR[] PROGMEM = "+SAPBR: 1,1";                            // Expected answer SAPBR: 1,1

const char URL[] = "170.187.154.110/api/v1/smartcap/update?";
char ATH_URL[] = "170.187.154.110/api/v1/smartcap/auth?";
String stat = "value=&token=";
const char content_appCoded[] = "application/x-www-form-urlencoded";
const char content_json[]  = "application/json";
const char buzz[] = "170.187.154.110/api/v1/smartcap/check/status?";
const char apn[] ="Internet";
unsigned int fingerPrintAccepted = 0;
float gps_lat;
float gps_lon;
int sv = 0;


//functions prototyping

// void http();
// void clearGsm();
// void gsmRead(uint32_t);
void customDelay(uint32_t);
void net();
void reconnect(const char *);
void updateCap();
void buzzer();
void reuploadZero();
void trigger(void);
void sens();

void customDelay(uint32_t timeout_ms) {
  uint32_t startMil = millis();
  do {
    
  } while (millis() - startMil < timeout_ms);

}

class http{


  public:
    ~http(){
      free(internalBuffer);
      free(recvBuffer);
    }  
    http(){
      internalBuffer = (char*) malloc(200);
      recvBuffer = (char *) malloc(512);    
    }
    void purgeSerial() {
      stream->flush();
      while (stream->available()) {
        stream->read();
      }
      stream->flush();
    }
    
    
    //send simple command
    void sendCommandN(const char* command, const char* parameter) {
      //  //////if(enableDebug) {
      //    ////debugStream->print(F("SIM800L : Send \""));
      //    ////debugStream->print(command);
      //    ////debugStream->print(F("\""));
      //    ////debugStream->print(parameter);
      //    ////debugStream->print(F("\""));
      //    ////debugStream->println(F("\""));
      //  }
    
      purgeSerial();
      stream->write(command);
      stream->write("\"");
      stream->write(parameter);
      stream->write("\"");
      stream->write("\r\n");
      purgeSerial();
    }
    void sendCommandNP(const char* command, const char* parameter1, const char* parameter2) {
      //  //////if(enableDebug) {
      //    ////debugStream->print(F("SIM800L : Send \""));
      //    ////debugStream->print(command);
      //    ////debugStream->print(F("\""));
      //    ////debugStream->print(parameter);
      //    ////debugStream->print(F("\""));
      //    ////debugStream->println(F("\""));
      //  }
    
      purgeSerial();
      stream->write(command);
      stream->write("\"");
      stream->write(parameter1);
      stream->write(parameter2);
      stream->write("\"");
      stream->write("\r\n");
      purgeSerial();
    }    
    
    //send commnad from Program mem with parameter
    void sendCommand_P(const char* command, const char* parameter) {
      char cmdBuff[32];
      strcpy_P(cmdBuff, command);
      sendCommandN(cmdBuff, parameter);
    }
    void sendCommand_PU(const char* command, const char* parameter1, const char* parameter2) {
      char cmdBuff[32];
      strcpy_P(cmdBuff, command);
      sendCommandNP(cmdBuff, parameter1, parameter2);
    }
    void sendCommandN(const char* command) {
      //  //////if(enableDebug) {
      //    ////debugStream->print(F("SIM800L : Send \""));
      //    ////debugStream->print(command);
      //    ////debugStream->println(F("\""));
      //  }
    
      purgeSerial();
      stream->write(command);
      stream->write("\r\n");
      purgeSerial();
    }
    void sendCommand_P(const char* command) {
      char cmdBuff[32];
      strcpy_P(cmdBuff, command);
      sendCommandN(cmdBuff);
    }
    void initInternalBuffer() {
      for (uint16_t i = 0; i < internalBufferSize; i++) {
        internalBuffer[i] = '\0';
      }
    }
    
    void initRecvBuffer() {
      // Cleanup the receive buffer
      for (uint16_t i = 0; i < recvBufferSize; i++) {
        recvBuffer[i] = 0;
      }
    }
    uint16_t getDataSizeReceived() {
      return dataSize;
    }
    
    /**
       Return the buffer of data received after the last successful HTTP connection
    */
    char* getDataReceived() {
      return recvBuffer;
    }
    
    char* getIP() {
      sendCommand_P(AT_CMD_SAPBR2);
      if (readResponse(DEFAULT_TIMEOUT,2)) {
        int16_t idx = strIndex(internalBuffer, "+SAPBR: 1,1",0);
        if (idx > 0) {
          idx = idx + 13;
        } else {
          static char ntc[] = "Not connected";
          return ntc;
        }
        int16_t idxEnd = strIndex(internalBuffer, "\"", idx + 1);
    
        // Store it on the recv buffer (not used at the moment)
        initRecvBuffer();
        for (uint16_t i = 0; i < idxEnd - idx; i++) {
          recvBuffer[i] = internalBuffer[idx + i];
        }
        return getDataReceived();
      } else {
        static char ntc[] ="Not connected";
        return ntc;
      }
    }
    
    /**
       Read from the module for a specific number of CRLF
       True if we have some data
    */
    bool readResponse(uint16_t timeout, uint8_t crlfToWait) {
      uint16_t currentSizeResponse = 0;
      bool seenCR = false;
      uint8_t countCRLF = 0;
    
      // First of all, cleanup the buffer
      initInternalBuffer();
    
      uint32_t timerStart = millis();
    
      while (1) {
        // While there is data available on the buffer, read it until the max size of the response
        if (stream->available()) {
          // Load the next char
          internalBuffer[currentSizeResponse] = stream->read();
    
          // Detect end of transmission (CRLF)
          if (internalBuffer[currentSizeResponse] == '\r') {
            seenCR = true;
          } else if (internalBuffer[currentSizeResponse] == '\n' && seenCR) {
            countCRLF++;
            if (countCRLF == crlfToWait) {
              break;
            }
          } else {
            seenCR = false;
          }
    
          // Prepare for next read
          currentSizeResponse++;
    
          // Avoid buffer overflow
          if (currentSizeResponse == internalBufferSize) {
            break;
          }
        }
    
        // If timeout, abord the reading
        if (millis() - timerStart > timeout) {
          // Timeout, return false to parent function
          return false;
        }
      }
    
      // If we are here, it's OK ;-)
      return true;
    }
    
    int16_t strIndex(const char* str, const char* findStr, uint16_t startIdx) {
      int16_t firstIndex = -1;
      int16_t sizeMatch = 0;
      for (int16_t i = startIdx; i < (int16_t)(strlen(str)); i++) {
        if (sizeMatch >= (int16_t)strlen(findStr)) {
          break;
        }
        if (str[i] == findStr[sizeMatch]) {
          if (firstIndex < 0) {
            firstIndex = i;
          }
          sizeMatch++;
        } else {
          firstIndex = -1;
          sizeMatch = 0;
        }
      }
    
      if (sizeMatch >= (int16_t)strlen(findStr)) {
        return firstIndex;
      } else {
        return -1;
      }
    }
    //read answer from gsm
    bool readResponseCheckAnswer_P(uint16_t timeout, const char* expectedAnswer, uint8_t crlfToWait) {
      if (readResponse(timeout, crlfToWait)) {
        // Prepare the local expected answer
        char rspBuff[16];
        strcpy_P(rspBuff, expectedAnswer);
    
        // Check if it's the expected answer
        int16_t idx = strIndex(internalBuffer, rspBuff,0);
        if (idx > 0) {
          return true;
        }
      }
      return false;
    }
    
    bool isReady() {
      sendCommand_P(AT_CMD_BASE);
      return readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2);
    }
    
    PowerMode getPowerMode() {
      sendCommand_P(AT_CMD_CFUN_TEST);
      if (readResponse(DEFAULT_TIMEOUT,2)) {
        // Check if there is an error
        int16_t errIdx = strIndex(internalBuffer, "ERROR",0 );
        if (errIdx > 0) {
          return POW_ERROR;
        }
    
        // Extract the value
        int16_t idx = strIndex(internalBuffer, "+CFUN: ",0);
        char value = internalBuffer[idx + 7];
    
        // Prepare the clear output
        switch (value) {
          case '0' : return MINIMUM;
          case '1' : return NORMAL;
          case '4' : return SLEEP;
          default  : return POW_UNKNOWN;
        }
      }
      return POW_ERROR;
    }
    // Network registration
    NetworkRegistration getRegistrationStatus() {
      sendCommand_P(AT_CMD_CREG_TEST);
      if (readResponse(DEFAULT_TIMEOUT,2)) {
        // Check if there is an error
        int16_t errIdx = strIndex(internalBuffer, "ERROR",0);
        if (errIdx > 0) {
          return NET_ERROR;
        }
    
        // Extract the value
        int16_t idx = strIndex(internalBuffer, "+CREG: ",0);
        char value = internalBuffer[idx + 9];
    
        // Prepare the clear output
        switch (value) {
          case '0' : return NOT_REGISTERED;
          case '1' : return REGISTERED_HOME;
          case '2' : return SEARCHING;
          case '3' : return DENIED;
          case '5' : return REGISTERED_ROAMING;
          default  : return NET_UNKNOWN;
        }
      }
    
      return NET_ERROR;
    }
    bool setupGPRS(const char* apn) {
      // Prepare the GPRS connection as the bearer
      sendCommand_P(AT_CMD_CIPSHUT);
      if (!readResponseCheckAnswer_P(20000, AT_RSP_OK,2)) {
      }
      sendCommand_P(AT_CMD_CSTT, apn);
      if (!readResponseCheckAnswer_P(20000, AT_RSP_OK,2)) {
      }      
      // Set the config of the bearer with the APN
      sendCommand_P(AT_CMD_CIICR, apn);
      if (!readResponseCheckAnswer_P(20000, AT_RSP_OK,2)) {
      }
      sendCommand_P(AT_CMD_CIFSR);   
      return readResponse(DEFAULT_TIMEOUT,2);
    }
    
    /**
       Setup the GPRS connectivity with user and password
       As input, give the APN string of the operator, the user and the password
    */
    bool setupGPRS(const char* apn, const char* user, const char* password) {
      // Prepare the GPRS connection as the bearer
      sendCommand_P(AT_CMD_SAPBR_GPRS);
      if (!readResponseCheckAnswer_P(20000, AT_RSP_OK,2)) {
        return false;
      }
    
      // Set the config of the bearer with the APN
      sendCommand_P(AT_CMD_SAPBR_APN, apn);
      if (!readResponseCheckAnswer_P(20000, AT_RSP_OK,2)) {
        return false;
      }
    
      // Set the config of the bearer with the USER
      sendCommand_P(AT_CMD_SAPBR_USER, user);
      if (!readResponseCheckAnswer_P(20000, AT_RSP_OK,2)) {
        return false;
      }
    
      // Set the config of the bearer with the PWD
      sendCommand_P(AT_CMD_SAPBR_PWD, password);
      return readResponseCheckAnswer_P(20000, AT_RSP_OK,2);
    }
    
    /**
       Open the GPRS connectivity
    */
    bool connectGPRS() {
      sendCommand_P(AT_CMD_SAPBR1);
      // Timout is max 85 seconds according to SIM800 specifications
      // We will wait for 65s to be within uint16_t
      return readResponseCheckAnswer_P(65000, AT_RSP_OK,2);
    }
    
    /**
       Check if GPRS is connected
    */
    bool isConnectedGPRS() {
      sendCommand_P(AT_CMD_SAPBR2);
      return readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_SAPBR,2);
    }
    
    /**
       Close the GPRS connectivity
    */
    bool disconnectGPRS() {
      sendCommand_P(AT_CMD_SAPBR0);
      // Timout is max 65 seconds according to SIM800 specifications
      return readResponseCheckAnswer_P(65000, AT_RSP_OK,2);
    }
    
    /**
       Define the power mode
       Available : MINIMUM, NORMAL, SLEEP
       Return true is the mode is correctly switched
    */
    bool setPowerMode(PowerMode powerMode) {
      // Check if the power mode requested is not ERROR or UNKNOWN
      if (powerMode == POW_ERROR || powerMode == POW_UNKNOWN) {
        return false;
      }
    
      // Check the current power mode
      PowerMode currentPowerMode = getPowerMode();
    
      // If the current power mode is undefined, abord
      if (currentPowerMode == POW_ERROR || currentPowerMode == POW_UNKNOWN) {
        return false;
      }
    
      // If the current power mode is the same that the requested power mode, say it's OK
      if (currentPowerMode == powerMode) {
        return true;
      }
    
      // If SLEEP or MINIMUM, only NORMAL is allowed
      if ((currentPowerMode == SLEEP || currentPowerMode == MINIMUM) && (powerMode != NORMAL)) {
        return false;
      }
    
      // Send the command
    //  char value;
      switch (powerMode) {
        case MINIMUM :
          sendCommand_P(AT_CMD_CFUN0);
          break;
        case SLEEP :
          sendCommand_P(AT_CMD_CFUN4);
          break;
        case NORMAL :
        default :
          sendCommand_P(AT_CMD_CFUN1);
      }
    
      // Read but don't care about the result
      purgeSerial();
    
      // Check the current power mode
      currentPowerMode = getPowerMode();
    
      // If the current power mode is the same that the requested power mode, say it's OK
      return currentPowerMode == powerMode;
    }
    
    //Get device version
    char* getVersion() {
      sendCommand_P(AT_CMD_ATI);
      if (readResponse(DEFAULT_TIMEOUT,2)) {
        // Extract the value
        int16_t idx = strIndex(internalBuffer, "SIM",0);
        int16_t idxEnd = strIndex(internalBuffer, "\r", idx + 1);
    
        // Store it on the recv buffer (not used at the moment)
        initRecvBuffer();
        for (uint16_t i = 0; i < idxEnd - idx; i++) {
          recvBuffer[i] = internalBuffer[idx + i];
        }
        return getDataReceived();
      } else {
        return NULL;
      }
    }
//      void sendCommandNP(const char* command, const char* url, const char* parameter2)
    uint16_t initiateHTTPU(const char* url, const char* parameter2) {
    //  sendCommand_P(AT_CMD_HTTPTERM);
    //  if(!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)){}
    //  // Init HTTP connection
    
      sendCommand_P(AT_CMD_HTTPINIT);
      if (!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)) {
        //////if(enableDebug) ////debugStream->println(F("SIM800L : initiateHTTP() - Unable to init HTTP"));
        return 701;
      }
        // Use the GPRS bearer
      sendCommand_P(AT_CMD_HTTPPARA_CID);
      if (!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)) {
        //////if(enableDebug) ////debugStream->println(F("SIM800L : initiateHTTP() - Unable to define bearer"));
        return 702;
      }
    
      // Define URL to look for
//      sendCommandNP(AT_CMD_HTTPPARA_URL, url, parameter2);
//      void sendCommand_PU(const char* command, const char* parameter1, const char* parameter2)
      sendCommand_PU(AT_CMD_HTTPPARA_URL, url, parameter2);
      if (!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)) {
        ////////if(enableDebug) ////debugStream->println(F("SIM800L : initiateHTTP() - Unable to define the URL"));
        return 702;
      }
     return 0;
    }
    uint16_t initiateHTTP(const char* url, const char* headers) {
    //  sendCommand_P(AT_CMD_HTTPTERM);
    //  if(!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)){}
    //  // Init HTTP connection
    
      sendCommand_P(AT_CMD_HTTPINIT);
      if (!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)) {
        //////if(enableDebug) ////debugStream->println(F("SIM800L : initiateHTTP() - Unable to init HTTP"));
        return 701;
      }
        // Use the GPRS bearer
      sendCommand_P(AT_CMD_HTTPPARA_CID);
      if (!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)) {
        //////if(enableDebug) ////debugStream->println(F("SIM800L : initiateHTTP() - Unable to define bearer"));
        return 702;
      }
    
      // Define URL to look for
      sendCommand_P(AT_CMD_HTTPPARA_URL, url);
      if (!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)) {
        ////////if(enableDebug) ////debugStream->println(F("SIM800L : initiateHTTP() - Unable to define the URL"));
        return 702;
      }
    
      // Set Headers
      if (headers != NULL) {
        sendCommand_P(AT_CMD_HTTPPARA_USERDATA, headers);
        if (!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)) {
          //////if(enableDebug) ////debugStream->println(F("SIM800L : initiateHTTP() - Unable to define Headers"));
          return 702;
        }
      }
    
      // Enable HTTP redirection if HTTP RC 302
    //  sendCommand_P(AT_CMD_HTTPPARA_REDIR);
    //  if (!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)) {
    //    //////if(enableDebug) ////debugStream->println(F("SIM800L : initiateHTTP() - Unable to enable HTTP redirection"));
    //    return 702;
    //  }
    
      // Check if the firmware support HTTPSSL command
//      bool isSupportSSL = false;
//      char* version = getVersion();
//      int16_t rIdx = strIndex(version, "R",0);
//      if (rIdx > 0) {
//        uint8_t releaseInt = (version[rIdx + 1] - '0') * 10 + (version[rIdx + 2] - '0');
//    
//        // The release should be greater or equals to 14 to support SSL stack
//        if (releaseInt >= 14) {
//          isSupportSSL = true;
//          //////if(enableDebug) ////debugStream->println(F("SIM800L : initiateHTTP() - Support of SSL enabled"));
//        } else {
//          isSupportSSL = false;
//          //////if(enableDebug) ////debugStream->println(F("SIM800L : initiateHTTP() - Support of SSL disabled (SIM800L firware below R14)"));
//        }
//      }
//    
//      // Send HTTPSSL command only if the version is greater or equals to 14
//      if (isSupportSSL) {
//        // HTTP or HTTPS
//        if (strIndex(url, "https://",0) == 0) {
//          sendCommand_P(AT_CMD_HTTPSSL_Y);
//          if (!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)) {
//            //////if(enableDebug) ////debugStream->println(F("SIM800L : initiateHTTP() - Unable to switch to HTTPS"));
//            return 702;
//          }
//        } else {
//          sendCommand_P(AT_CMD_HTTPSSL_N);
//          if (!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)) {
//            //////if(enableDebug) ////debugStream->println(F("SIM800L : initiateHTTP() - Unable to switch to HTTP"));
//            return 702;
//          }
//        }
//      }
    
      return 0;
    }
    
    uint16_t readHTTP(uint16_t serverReadTimeoutMs) {
      // Cleanup the receive buffer
      initRecvBuffer();
      dataSize = 0;
    
      // Wait answer from the server
      if (!readResponse(serverReadTimeoutMs,2)) {
        ////if(enableDebug) ////debugStream->println(F("SIM800L : readHTTP() - Server timeout"));
        return 408;
      }
    
      // Extract status information
      int16_t idxBase = strIndex(internalBuffer, "+HTTPACTION: ",0);
      if (idxBase < 0) {
        ////if(enableDebug) ////debugStream->println(F("SIM800L : readHTTP() - Invalid answer on HTTP read"));
        return 703;
      }
    
      // Get the HTTP return code
      uint16_t httpRC = 0;
      httpRC += (internalBuffer[idxBase + 15] - '0') * 100;
      httpRC += (internalBuffer[idxBase + 16] - '0') * 10;
      httpRC += (internalBuffer[idxBase + 17] - '0') * 1;
    
      ////if(enableDebug) {
      ////debugStream->print(F("SIM800L : readHTTP() - HTTP status "));
      ////debugStream->println(httpRC);
      //  }
    
      if (httpRC == 200) {
        // Get the size of the data to receive
        dataSize = 0;
        for (uint16_t i = 0; (internalBuffer[idxBase + 19 + i] - '0') >= 0 && (internalBuffer[idxBase + 19 + i] - '0') <= 9; i++) {
          if (i != 0) {
            dataSize = dataSize * 10;
          }
          dataSize += (internalBuffer[idxBase + 19 + i] - '0');
        }
    
        ////if(enableDebug) {
        ////debugStream->print(F("SIM800L : readHTTP() - Data size received of "));
        ////debugStream->print(dataSize);
        ////debugStream->println(F(" bytes"));
        //    }
    
        // Ask for reading and detect the start of the reading...
        sendCommand_P(AT_CMD_HTTPREAD);
        if (!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_HTTPREAD, 2)) {
          return 705;
        }
    
        // Read the data and purge the serial if buffer is too small
        if (dataSize < recvBufferSize - 1) {
          stream->readBytes(recvBuffer, dataSize);
        } else {
          uint16_t toRead = dataSize;
          size_t bytesRead = stream->readBytes(recvBuffer, recvBufferSize - 1);
          toRead -= bytesRead;
          while (toRead > 0) {
            stream->read();
            toRead -= 1;
          }
        }
    
        if (recvBufferSize < dataSize) {
          dataSize = recvBufferSize;
          ////if(enableDebug) {
          ////debugStream->println(F("SIM800L : readHTTP() - Buffer overflow while loading data from HTTP. Keep only first bytes..."));
          //      }
        }
    
        // We are expecting a final OK
        if (!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)){}
    
        ////if(enableDebug) {
        ////debugStream->print(F("SIM800L : readHTTP() - Received from HTTP call : "));
        ////debugStream->println(recvBuffer);
        //    }
      }
    
      // Close HTTP connection
      sendCommand_P(AT_CMD_HTTPTERM);
      if (!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)) {
        ////if(enableDebug) ////debugStream->println(F("SIM800L : readHTTP() - Unable to close HTTP session"));
        return 706;
      }
    
      return httpRC;
    }
    
    
    /**
       Do HTTP/S POST to a specific URL with headers
    */
    uint16_t doPost(const char* url,const char* contentType, const char* payload, uint16_t clientWriteTimeoutMs, uint16_t serverReadTimeoutMs) {
      // Define the content type
      uint16_t initRC = initiateHTTP(url, NULL);
      if(initRC > 0) {
        sendCommand_P(AT_CMD_HTTPTERM);
        if(!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)){}
        Serial.println(F("Something wrong with init http"));
        initRC = initiateHTTP(ATH_URL, NULL);
      }
      sendCommand_P(AT_CMD_HTTPPARA_CONTENT, contentType);
      if (!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)) {
        //if(enableDebug) //debugStream->println(F("SIM800L : doPost() - Unable to define the content type"));
        return 702;
      }
    
      // Prepare to send the payload
      sprintf(internalBuffer, "AT+HTTPDATA=%d,%d", strlen(payload), clientWriteTimeoutMs);
      sendCommandN(internalBuffer);
      if (!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_DOWNLOAD,2)) {
        //if(enableDebug) //debugStream->println(F("SIM800L : doPost() - Unable to send payload to module"));
        return 707;
      }
    
      // Write the payload on the module
      //if(enableDebug) {
      //debugStream->print(F("SIM800L : doPost() - Payload to send : "));
      //debugStream->println(payload);
      //  }
    
      purgeSerial();
      stream->write(payload);
      stream->flush();
      delay(500);
    
      // Start HTTP POST action
      sendCommand_P(AT_CMD_HTTPACTION1);
      if (!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)) {
        //if(enableDebug) //debugStream->println(F("SIM800L : doPost() - Unable to initiate POST action"));
        return 703;
      }
    
      // Read data, manage buffers and close HTTP connection
      return readHTTP(serverReadTimeoutMs);
    }

    uint16_t doPostD(const char* url,const char* contentType, uint16_t clientWriteTimeoutMs, uint16_t serverReadTimeoutMs) {
      // Define the content type
      uint16_t initRC = initiateHTTP(url, NULL);
      if(initRC > 0) {
        sendCommand_P(AT_CMD_HTTPTERM);
        if(!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)){}
        Serial.println(F("Something wrong with init http"));
        initRC = initiateHTTP(ATH_URL, NULL);
      }
      sendCommand_P(AT_CMD_HTTPPARA_CONTENT, contentType);
      if (!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)) {
        //if(enableDebug) //debugStream->println(F("SIM800L : doPost() - Unable to define the content type"));
        return 702;
      }
    
      // Start HTTP POST action
      sendCommand_P(AT_CMD_HTTPACTION1);
      if (!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)) {
        //if(enableDebug) //debugStream->println(F("SIM800L : doPost() - Unable to initiate POST action"));
        return 703;
      }
    
      // Read data, manage buffers and close HTTP connection
      return readHTTP(serverReadTimeoutMs);
    }
//    void sendCommandNP(const char* command, const char* parameter1, const char* parameter2)
    
    uint16_t doPostDU(const char* url,const char* parameter2,const char* contentType, uint16_t clientWriteTimeoutMs, uint16_t serverReadTimeoutMs) {
      // Define the content type
      uint16_t initRC = initiateHTTPU(url, parameter2);
      if(initRC > 0) {
        sendCommand_P(AT_CMD_HTTPTERM);
        if(!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)){}
        Serial.println(F("Something wrong with init http"));
        initRC = initiateHTTPU(ATH_URL, parameter2);
      }
      sendCommand_P(AT_CMD_HTTPPARA_CONTENT, contentType);
      if (!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)) {
        //if(enableDebug) //debugStream->println(F("SIM800L : doPost() - Unable to define the content type"));
        return 702;
      }
    
      // Start HTTP POST action
      sendCommand_P(AT_CMD_HTTPACTION1);
      if (!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)) {
        //if(enableDebug) //debugStream->println(F("SIM800L : doPost() - Unable to initiate POST action"));
        return 703;
      }
    
      // Read data, manage buffers and close HTTP connection
      return readHTTP(serverReadTimeoutMs);
    }
    /**
       Do HTTP/S POST to a specific URL
    */
    //uint16_t doPost(const char* url, const char* contentType, const char* payload, uint16_t clientWriteTimeoutMs, uint16_t serverReadTimeoutMs) {
    //  return doPost(url,contentType, payload, clientWriteTimeoutMs , serverReadTimeoutMs);
    //}
    
    uint8_t getSignal() {
      sendCommand_P(AT_CMD_CSQ);
      if (readResponse(DEFAULT_TIMEOUT,2)) {
        int16_t idxBase = strIndex(internalBuffer, "AT+CSQ",0);
        if (idxBase != 0) {
          return 0;
        }
        int16_t idxEnd = strIndex(internalBuffer, ",", idxBase);
        uint8_t value = internalBuffer[idxEnd - 1] - '0';
        if (internalBuffer[idxEnd - 2] != ' ') {
          value += (internalBuffer[idxEnd - 2] - '0') * 10;
        }
        if (value > 31) {
          return 0;
        }
        return value;
      }
      return 0;
    }
};
  
  //read GPS data
  
void readGPS() {
  unsigned long age;
  float glat, glon;
  byte month, day, hour, minute, second, hundredths;
  int year;
  sGps.listen();
  for (int i = 0; i < 1000; i++) {
    gps.encode(sGps.read()); \
    gps.f_get_position(&glat, &glon, &age); delay(2); gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  }

  if (db) {
    Serial.print(F("lat: "));
    Serial.println(glat);
    Serial.print(F(" lon: "));
    Serial.println(glon);
  }
  //   sprintf(tsmp,"%02d/%02/%02d %02d:%02d:%02d ",month,day,year, hour, minute, second);
  // String b = String(month)+"/"+String(day)+"/"+String(year)+" "+String(hour)+":"+String(minute)+":"+String(second);
  // timeStamp = b;
  gps_lat = glat;
  gps_lon = glon;


}
