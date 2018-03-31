/* Transmitter Code
*  Code to read Joystick position and transmit it with a RF24L01+ to a receiver 
*/

#include <SPI.h>
#include <RF24.h>
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ A5, /* data=*/ A4, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather M0 Basic Proto + FeatherWing OLED

// Radio Configuration

RF24 radio(9,10);
byte addresses[][6] = {"1Node","2Node"};
bool radioNumber=1;
bool role = 1;  //Control transmit 1/receive 0

// Decide where you are going to plug the joystick into the circuit board.
int axis1 = A3;
int ForeAft_Pin = 0;        // Plug Joystick Fore/Aft into Analog pin 0
int LeftRight_Pin = 1;      // Plug Joystick Left/Right into Analog pin 1

// Create variables to read joystick values

float ForeAft_Input ;       // Variable to store data for Fore/Aft input from joystick
float LeftRight_Input ;     // Variable to store data for for Left/Right input from joystick

// Create variables to transmit servo value

int ForeAft_Output;       // Expected range 0 - 180 degrees
int LeftRight_Output;     // Expected range 0 - 180 degrees

// These variables allow for math conversions and later error checking as the program evolves.

int Fore_Limit = 800;       // High ADC Range of Joystick ForeAft
int Aft_Limit = 220;        // Low ADC Range of Joystick ForeAft
int Right_Limit = 800;      // High ADC Range of Joystick LeftRight
int Left_Limit = 226;       // Low ADC Range of Joystick LeftRight

// Begin Josh's Mods to pre-setup

char driveMode = 0;          // define different drive modes
uint8_t driveSpeed = 0;         // define drive speed
byte aux1 = 0;               // Auxilary variables for later use
byte aux2 = 0;
byte aux3 = 0;
byte aux4 = 0;
byte aux5 = 0;

#define redButton 8
#define greenButton 7
#define blueButton 6
#define whiteButton 5



String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete

boolean whiteButtonStatus;
// End mods

void setup() {
    setupSerial();                   // Start serial setup procedure
    setupRadio();                    // Start radio setup procedure
    setupOLED();                     // Start OLED setup procedure
    inputString.reserve(200);
    pinMode(3, INPUT_PULLUP);
    pinMode(8, INPUT_PULLUP);
    pinMode(7, INPUT_PULLUP);
    pinMode(6, INPUT_PULLUP);
    pinMode(5, INPUT_PULLUP);
    u8g2.setFontMode(1);
}
void loop() {
    ForeAft_Input = analogRead(ForeAft_Pin) ;             // Read the Fore/Aft joystick value
    LeftRight_Input = analogRead(LeftRight_Pin) ;         // Read the Left/Right joystick value
    ForeAft_Output = convertForeAftToServo(ForeAft_Input) ;        // Convert the Fore/Aft joystick value to a Servo value (0-180)
    LeftRight_Output = convertLeftRightToServo(LeftRight_Input) ;  // Convert the Left/Right joystick value to a Servo value (0-180)
    axis1 = analogRead(A3);

    //  Serial.print(ForeAft_Output);
    radio.stopListening();                                 // Stop listening and begin transmitting
    if(radio.write(&ForeAft_Output, sizeof(ForeAft_Output)),Serial.print("sent ForeAft"),Serial.print(ForeAft_Output));              //Send ForeAft data
    if(radio.write(&LeftRight_Output, sizeof(LeftRight_Output)),Serial.print("sent LeftRight"),Serial.print(LeftRight_Output));        //Send LeftRight data

// Begin Josh's Mods

    if(radio.write(&aux5, sizeof(aux5)),Serial.print("sent aux 5, "),Serial.println(aux5));    
    if(radio.write(&aux4, sizeof(aux4)),Serial.print("sent aux 4, "),Serial.println(aux4));
    if(radio.write(&aux3, sizeof(aux3)),Serial.print("sent aux 3, "),Serial.println(aux3));
    if(radio.write(&aux2, sizeof(aux2)),Serial.print("sent aux 2, "),Serial.println(aux2));
    if(radio.write(&aux1, sizeof(aux1)),Serial.print("sent aux 1, "),Serial.println(aux1));
    if(radio.write(&axis1, sizeof(axis1)),Serial.print("sent axis 1, "),Serial.println(axis1));

    updateOLED();
    //delay(1000);
    if(radio.write(&inputString, sizeof(inputString)),Serial.println(inputString));
    if(digitalRead(redButton == HIGH)) {
      aux3 = 55;
    }
    if(whiteButtonStatus) {
      aux1 = (aux1 + 1);
    }
    else{
      aux1 = (aux1 - 1);
    }

    whiteButtonStatus = (digitalRead(whiteButton));
    


    
    if (stringComplete) {
        Serial.println(inputString);
        // clear the string:
        inputString = "";
        stringComplete = false;
      }

    
radio.startListening();                                // Get ready to receive confirmation from receiver
}

// Function to convert and scale the Fore/Aft data

float convertForeAftToServo(float y) {
    int result;
    result = ((y - Aft_Limit) / (Fore_Limit - Aft_Limit) * 180);
    return result;
}

// Function to convert and scale the Left / Right data
// Can be replaced with Map function
float convertLeftRightToServo(float x) {
    int result;
    result = ((x - Left_Limit) / (Right_Limit - Left_Limit) * 180);
    return result;
}

void updateOLED() {
    u8g2.setFont(u8g2_font_ncenB08_tr);  // choose a suitable font
    u8g2.clearBuffer();          // clear the internal memory
    u8g2.setFontDirection(0);
    u8g2.setCursor(0,8);
    u8g2.print("aux1=");  // write something to the internal memory
    u8g2.println(aux1);        // transfer internal memory to the display
    u8g2.print(" aux2=");
    u8g2.print(aux2);
    u8g2.print(" aux3=");
    u8g2.print(aux3);
    u8g2.setCursor(0,17);
    u8g2.print("aux4=");
    u8g2.print(aux4);
    u8g2.print(" aux5=");
    u8g2.print(aux5);
    u8g2.print("ax1=");
    u8g2.println(axis1);
    u8g2.setFont(u8g2_font_ncenB10_tr);    
    u8g2.setCursor(0,28);
    u8g2.print("Josh is awesome");  
    u8g2.sendBuffer();  
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

