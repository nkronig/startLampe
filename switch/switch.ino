#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define led 12
#define testButton 2
#define towSwitch 3

#define buffSize 256

char receiveBuffer[buffSize];
uint8_t receiveBufferSize = 0;
bool towCommnadSent = false;
long lastTime;

RF24 radio(10, 9); // CE, CSN
const byte address[6] = "00001";
boolean buttonState = 0;

const char cmdBuf[][4] = {"!Of", "!On"};

void setup()
{
    pinMode(12, OUTPUT);
    Serial.begin(115200);
    radio.begin();
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_250KBPS);
    radio.stopListening();
    Serial.println("Setup of Switch finished!");
}
void loop()
{
    sendCommand(0);
    delay(1000);
    sendCommand(1);
    /*if (digitalRead(testButton))
    {
        bool test = false;
        while (test = !sendCommand(1) && digitalRead(testButton))
            ;
        if (test)
        {
            int testTry = 0;
            while (!sendCommand(0))
            {
                testTry++;
                if (testTry >= 3)
                    break;
            }
        }
        else if (!test)
        {
            while (digitalRead(testButton))
                ;
            int testTry = 0;
            while (!sendCommand(0))
            {
                testTry++;
                if (testTry >= 3)
                    break;
            }
        }
    }
    if (digitalRead(towSwitch))
    {
        if (millis() >= lastTime + 1000)
        {
            sendCommand(1);
        }
    }
    else
    {
        if (!towCommnadSent)
        {
            long timeOut = millis();
            while (!sendCommand(0) && millis() <= timeOut + 5000)
                ;
            towCommnadSent = true;
        }
    }*/
    delay(1000);
    //checkState(false);
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