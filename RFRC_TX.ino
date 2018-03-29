/* Transmitter Code
*  Code to read Joystick position and transmit it with a RF24L01+ to a receiver 
*/

#include <SPI.h>
#include <RF24.h>

// Radio Configuration

RF24 radio(9,10);
byte addresses[][6] = {"1Node","2Node"};
bool radioNumber=1;
bool role = 1;  //Control transmit 1/receive 0

// Decide where you are going to plug the joystick into the circuit board.

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

int driveMode = 0;          // define different drive modes
int driveSpeed = 0;         // define drive speed
int aux1 = 0;               // Auxilary variables for later use
int aux2 = 0;
int aux3 = 0;
int aux4 = 0;
int aux5 = 0;

String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete


// End mods

void setup() {
    Serial.begin(9600);                   // Get ready to send data back for debugging purposes
    radio.begin();                        // Get the transmitter ready
    radio.setPALevel(RF24_PA_LOW);        // Set the power to low
    radio.openWritingPipe(addresses[1]);  // Where we send data out
    radio.openReadingPipe(1,addresses[0]);// Where we receive data back

    inputString.reserve(200);
}
void loop() {
    ForeAft_Input = analogRead(ForeAft_Pin) ;             // Read the Fore/Aft joystick value
    LeftRight_Input = analogRead(LeftRight_Pin) ;         // Read the Left/Right joystick value
    ForeAft_Output = convertForeAftToServo(ForeAft_Input) ;        // Convert the Fore/Aft joystick value to a Servo value (0-180)
    LeftRight_Output = convertLeftRightToServo(LeftRight_Input) ;  // Convert the Left/Right joystick value to a Servo value (0-180)


    //  Serial.print(ForeAft_Output);
    radio.stopListening();                                 // Stop listening and begin transmitting
    delay(500);                                            // quite a long delay -- causes jittering of servo
    if(radio.write(&ForeAft_Output, sizeof(ForeAft_Output)),Serial.println("sent ForeAft"));              //Send ForeAft data
    if(radio.write(&LeftRight_Output, sizeof(LeftRight_Output)),Serial.println("sent LeftRight"));        //Send LeftRight data

// Begin Josh's Mods

    if(radio.write(&aux5, sizeof(aux5)),Serial.println("sent aux 5"));
    if(radio.write(&aux4, sizeof(aux4)),Serial.println("sent aux 4"));
    if(radio.write(&aux3, sizeof(aux3)),Serial.println("sent aux 3"));
    if(radio.write(&aux2, sizeof(aux2)),Serial.println("sent aux 2"));
    if(radio.write(&aux1, sizeof(aux1)),Serial.println("sent aux 1"));
    if(radio.write(&inputString, sizeof(inputString)),Serial.println(inputString));
    if(digitalRead(2 == HIGH)) {
      aux3 = 55;
    }
    if (stringComplete) {
        Serial.println(inputString);
        // clear the string:
        inputString = "";
        stringComplete = false;
      }
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

