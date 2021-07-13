#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define led 12

#define buffSize 256

char receiveBuffer[buffSize];
uint8_t receiveBufferSize = 0;
bool towCommnadSent = false;
long lastTime;

RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"TLAMP", "FLAMP"};
boolean buttonState = 0;

const char cmdBuf[][4] = {"!Of", "!On"};

void setup()
{
    pinMode(12, OUTPUT);
    radio.begin();
    radio.openReadingPipe(0, addresses[0]);    // 00000 WritingPipe to Lamp
    radio.openWritingPipe(addresses[1]); // 00001 FeedbackPipe from Lamp
    radio.setPALevel(RF24_PA_MAX);
    radio.setRetries(15,15);
    radio.setPayloadSize(8);
    radio.startListening();
}
void loop()
{
    int state = checkState(false);
    if(state != 2) sendCommand(state);
    delay(10);
}

bool sendCommand(int state)
{
    radio.stopListening();
    radio.write(&cmdBuf[state], sizeof(cmdBuf[state]));
    radio.startListening();
}

int checkState(bool waiting)
{
    if (waiting)
    {
        long timeOut = millis();
        while (radio.available() <= 0)
            if (millis() >= timeOut + 2000)
                return 2;
    }
    if (radio.available() > 0)
    {
        radio.read(receiveBuffer, buffSize);
        Serial.println(receiveBuffer);
        if (receiveBuffer[0] == '!')
        {
            if (strcmp(receiveBuffer, "!On") == 0)
            {
                Serial.println("Lamp status: On");
                digitalWrite(led, HIGH);
                return 1;
            }
            else if (strcmp(receiveBuffer, "!Of") == 0)
            {
                digitalWrite(led, LOW);
                Serial.println("Lamp status: Off");
                return 0;
            }
        }
    }
    return 2;
}