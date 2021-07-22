#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define led 4
#define testButton 2
#define towSwitch 3

#define buffSize 256

char receiveBuffer[buffSize];
uint8_t receiveBufferSize = 0;
bool offCommnadSent = false;
long lastTime;
boolean lampState = false;
RF24 radio(10, 9); // CE, CSN
const byte address[6] = "00001";
boolean buttonState = 0;

const char cmdBuf[][4] = {"!Of", "!On"};

void setup()
{
    //pinMode(12, OUTPUT);
    Serial.begin(115200);
    radio.begin();
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_250KBPS);
    radio.stopListening();
    Serial.println("Setup of Switch finished!");
    sendCommand(0);
    sendCommand(0);
}
void loop()
{
    if (digitalRead(towSwitch))
    {
        offCommnadSent = false;
        if (millis() >= lastTime + 1000)
        {
            setState(true);
            lastTime = millis();
        }
    }
    else
    {
        if (!lampState && !offCommnadSent)
        {
            setState(false);
            offCommnadSent = true;
        }
    }
    if(lampState){
        digitalWrite(led,HIGH);
    }
    else{
        digitalWrite(led,LOW);
    }
    checkState(false);
    delay(50);
}
void setState(boolean state){
    if(!state){
        sendCommand(0);
        sendCommand(0);
        digitalWrite(led,LOW);
    }
    if(state){
        sendCommand(1);
        digitalWrite(led,HIGH);
    }
}
void sendCommand(int state)
{
    radio.stopListening();
    radio.write(&cmdBuf[state], sizeof(cmdBuf[state]));
    Serial.println(cmdBuf[state]);
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
        Serial.println("received: ");
        Serial.print(receiveBuffer);
        if (receiveBuffer[0] == '!')
        {
            if (strcmp(receiveBuffer, "!On") == 0)
            {
                Serial.println("Lamp status: On");
                lampState = true;
                digitalWrite(led, HIGH);
                return 1;
            }
            else if (strcmp(receiveBuffer, "!Of") == 0)
            {
                digitalWrite(led, LOW);
                Serial.println("Lamp status: Off");
                lampState = false;
                return 0;
            }
        }
    }
    return 2;
}