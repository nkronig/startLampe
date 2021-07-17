#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define led 3

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
    pinMode(led, OUTPUT);
    Serial.begin(115200);
    radio.begin();
    radio.openReadingPipe(0, address);
    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_250KBPS);
    radio.startListening();
}
void loop()
{
    char text[32] = "";

    if (radio.available())
    {
        radio.read(&text, sizeof(text));
        String transData = String(text);
        Serial.println(transData);
        if (text[0] == '!')
        {
            Serial.println("!");
            if (strcmp(text, "!On") == 0)
            {
                Serial.println("Lamp status: On");
                digitalWrite(led, HIGH);
            }
            else if (strcmp(text, "!Of") == 0)
            {
                digitalWrite(led, LOW);
                Serial.println("Lamp status: Off");
            }
        }
    }
    delay(10);
}