#include <Wire.h>
#include <string.h>

// Define the I2C slave address
const int slaveAddress = 0x8;

// Define the struct for FireFlyStatus
typedef struct FireFlyStatus {
    unsigned char msgType;         // always  8
    unsigned char msgId;           // increment counter
    unsigned char isReadyToLaunch; // always 1
    unsigned char batteryPercentage; // always 6
    unsigned int BITStatus;       // always 0
    unsigned char cs;              // always 1
} FireFlyStatus;

// Define the struct for RequestMessage
typedef struct RequestMessage {
    unsigned char msgType;      // always 0
    unsigned char requestedType; // should be 1 then 4 then 9 then 10 (repeatedly)
    unsigned char msgSize;      // should be 58 then 6 then 24 then 35 (respectively)
} RequestMessage;

void setup() {
    Wire.begin(); // Initialize I2C communication
    Wire.setClock(400000);
    Serial.begin(115200);
    Serial.println("start simulator");
}

FireFlyStatus status = {8, 0, 1, 6, 0, 1};
// Create a RequestMessage struct and send it
RequestMessage request;
unsigned char requestedTypelist[4] = {1, 4, 9, 10};
unsigned char msgSizelist[4] = {58, 6, 24, 35};
int reqIndex = 0;

void loop() {

    // Send FireFlyStatus struct
    Wire.beginTransmission(slaveAddress);
    Wire.write((uint8_t*)&status, sizeof(FireFlyStatus));
    Wire.endTransmission();

    // Wait for a moment
    delay(10);

    request.msgType = 0;
    request.requestedType = requestedTypelist[reqIndex];
    request.msgSize = msgSizelist[reqIndex];
    reqIndex = (reqIndex + 1) % 4;

    // Send RequestMessage struct
    Wire.beginTransmission(slaveAddress);
    Wire.write((uint8_t*)&request, sizeof(RequestMessage));
    Wire.endTransmission();

    // Wait for a response
    uint32_t startTime = millis();
    while (millis() - startTime < 10) { // Wait for up to 1 second for a response
        if (Wire.available() >= request.msgSize) {
            // Read the response
            uint8_t response[request.msgSize];
            Wire.readBytes(response, request.msgSize);

            // Process the response here (you can print it or do something else)
            Serial.println("Received response to RequestMessage number " + String(request.requestedType));

            break; // Exit the loop after receiving the response
        }
    }

    // Wait for a moment before sending the next RequestMessage
    delay(10);
}
