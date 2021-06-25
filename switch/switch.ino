#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define led 12
#define testButton 2
#define towButton 3

#define buffSize 256

char receiveBuffer[buffSize];
uint8_t receiveBufferSize = 0;

RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"TLAMP", "FLAMP"};
boolean buttonState = 0;

const char cmdBuf[][4] = {"!Of", "!On"};

void setup()
{
    pinMode(12, OUTPUT);
    radio.begin();
    radio.openWritingPipe(addresses[0]);    // 00000 WritingPipe to Lamp
    radio.openReadingPipe(0, addresses[1]); // 00001 FeedbackPipe from Lamp
    radio.setPALevel(RF24_PA_MAX);
}
void loop()
{
    delay(5);
    if (digitalRead(testButton))
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
            while (digitalRead(testButton));
            int testTry = 0;
            while (!sendCommand(0))
            {
                testTry++;
                if (testTry >= 3)
                    break;
            }
        }
    }
    delay(5);

    while (!radio.available())
        ;
    radio.read(&buttonState, sizeof(buttonState));
    if (buttonState == HIGH)
    {
        digitalWrite(led, HIGH);
    }
    else
    {
        digitalWrite(led, LOW);
    }
}

bool sendCommand(int state)
{
    radio.stopListening();
    radio.write(&cmdBuf[state], sizeof(cmdBuf[state]));
    radio.startListening();
    int feedback = checkState();
    if (feedback == state)
    {
        return true;
    }
    else if (feedback == 2)
    {
        return false;
    }
}

int checkState(bool waiting = true)
{
    if (waiting)
    {
        int timeOut = millis();
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
                return 1;
            }
            else if (strcmp(receiveBuffer, "!Of") == 0)
            {
                Serial.println("Lamp status: Off");
                return 0;
            }
        }
    }
    return 2;
}