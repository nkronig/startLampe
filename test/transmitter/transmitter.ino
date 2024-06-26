#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(10,9);

const byte address[6] = "00001";

void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.stopListening();
}

void loop() {
  const char text[] = "nrftest";
  radio.write(&text, sizeof(text));
  delay(500);
}
