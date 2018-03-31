void setupSerial() {
  Serial.begin(115200);                         // Start serial communications at 115200bps
}

void setupRadio() {
  radio.begin();                                // Get the transmitter ready
  radio.setPALevel(RF24_PA_LOW);                // Set the power to low
  radio.openWritingPipe(addresses[1]);          // Where we send data out
  radio.openReadingPipe(1,addresses[0]);        // Where we receive data back
}

void setupOLED() {
  u8g2.begin();
  u8g2.setFontMode(1);
}

