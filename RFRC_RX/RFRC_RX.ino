/*  Receiver Code 
*   Code to receive data from RF24L01+ and use it to control a servo */

#include <Servo.h>
#include <SPI.h>
#include <RF24.h>

//Radio Configuration

bool radioNumber=0;
RF24 radio(9,10);
byte addresses[][6] = {"1Node","2Node"};
bool role = 0;  //Control transmit/receive

// Create variables to control servo value

Servo ForeAft;
Servo LeftRight;
unsigned int ForeAft_Output;       // Expected range 0 - 180 degrees
unsigned int LeftRight_Output;     // Expected range 0 - 180 degrees

// Begin Josh's Mods to pre-setup

int driveMode = 0;          // define different drive modes
int driveSpeed = 0;         // define drive speed
int aux1 = 0;               // Auxilary variables for later use
int aux2 = 0;
int aux3 = 0;
int aux4 = 0;
int aux5 = 0;




// End mods

void setup() { 
    Serial.begin(9600);        // Get ready to send data back for debugging purposes
    ForeAft.attach(6);         // Plug a servo signal line into digital output pin 6
    LeftRight.attach(7);       // Plug a servo signal line into digital output pin 7
    radio.begin();             // Initialize radio
    radio.setPALevel(RF24_PA_LOW);    // Set the power output to low
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
    radio.startListening();
}
void loop() {
    delay(500);               //increase for debuggy, decrease to decrease jitter
    if(radio.available()){
        radio.read(&ForeAft_Output,sizeof(ForeAft_Output));
        radio.read(&LeftRight_Output,sizeof(ForeAft_Output));
        radio.read(&aux5,sizeof(aux5));
        radio.read(&aux4,sizeof(aux4));
        radio.read(&aux3,sizeof(aux3));
        radio.read(&aux2,sizeof(aux2));
        radio.read(&aux1,sizeof(aux1));
        } else {Serial.print("No radio");
    }
    Serial.print(ForeAft_Output);
    Serial.print("\t");
    Serial.println(LeftRight_Output);

    Serial.println(aux5);
    Serial.println(aux4);
    Serial.println(aux3);
    Serial.println(aux2);
    Serial.println(aux1);
    
    ForeAft.write(ForeAft_Output);                          // Command the Fore/Aft servo to a position
    LeftRight.write(LeftRight_Output);                    // Command the Left/Right servo to a position
    }
