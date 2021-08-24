#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define localLamp 4
#define lampButton 3
#define lampButtonLed 6
#define buzzer 5

#define buffSize 256

char receiveBuffer[buffSize];
uint8_t receiveBufferSize = 0;
bool offCommnadSent = false;
long lastTime;
boolean lampState = false;
RF24 radio(10, 9); // CE, CSN
const byte address[6] = "00001";
boolean buttonState = 0;
boolean lampSwitch = false;
long onTime = 0;
boolean warningBuzzer = false;
long lastBuzzerTime = 0;
boolean buzzerState = false;
int buzzerDelayTime[] = {500, 1600};
const char cmdBuf[][4] = {"!Of", "!On"};

void setup()
{
    pinMode(localLamp, OUTPUT);
    pinMode(lampButtonLed, OUTPUT);
    pinMode(buzzer, OUTPUT);
    pinMode(lampButton, INPUT);
    Serial.begin(115200);
    radio.begin();
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_250KBPS);
    radio.stopListening();
    Serial.println("Setup of Switch finished!");
    tone(buzzer, 2000); // Send 1KHz sound signal...
    delay(100);
    noTone(buzzer); // Stop sound...
    sendCommand(0);
    sendCommand(0);
}
void loop()
{
    if (warningBuzzer)
    {
        if (millis() >= lastBuzzerTime + buzzerDelayTime[buzzerState])
        {
            lastBuzzerTime = millis();
            if (buzzerState)
            {
                tone(buzzer, 1950); // Send 1KHz sound signal...
            }
            else{
                noTone(buzzer);
            }
            buzzerState = !buzzerState;
        }
    }
    else{
        noTone(buzzer);
    }
    if (digitalRead(lampButton))
    {
        while (digitalRead(lampButton))
            ;
        if (lampSwitch)
        {
            tone(buzzer, 2800); // Send 1KHz sound signal...
            delay(100);
            noTone(buzzer); // Stop sound...
            delay(100);
            tone(buzzer, 2000); // Send 1KHz sound signal...
            delay(100);
            noTone(buzzer);
            warningBuzzer = false;
        }
        else
        {
            tone(buzzer, 1000); // Send 1KHz sound signal...
            delay(100);
            noTone(buzzer); // Stop sound...
            delay(100);
            tone(buzzer, 2800); // Send 1KHz sound signal...
            delay(100);
            noTone(buzzer);
            onTime = millis();
        }
        lampSwitch = !lampSwitch;
    }
    if (lampSwitch)
    {
        offCommnadSent = false;
        if (millis() >= lastTime + 1000)
        {
            setState(true);
            lastTime = millis();
        }
        if (millis() >= onTime + 40000)
        {
            warningBuzzer = true;
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
    /*if(lampState){
        digitalWrite(led,HIGH);
    }
    else{
        digitalWrite(led,LOW);
    }*/
    checkState(false);
    delay(50);
}
void setState(boolean state)
{
    if (!state)
    {
        sendCommand(0);
        sendCommand(0);
        digitalWrite(localLamp, LOW);
    }
    if (state)
    {
        sendCommand(1);
        digitalWrite(localLamp, HIGH);
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
                digitalWrite(localLamp, HIGH);
                return 1;
            }
            else if (strcmp(receiveBuffer, "!Of") == 0)
            {
                digitalWrite(localLamp, LOW);
                Serial.println("Lamp status: Off");
                lampState = false;
                return 0;
            }
        }
    }
    return 2;
}