// This is the code for the adapted feeder to run with having the full set up of electronics 
// as shown in the github.

// Before using this example, be sure to change the setCurrentMilliamps36v4 line
// to have an appropriate current limit for your system. For the adapted if using the 
// stepper driver and motor suggested in the github the current is 2800.

#include <SPI.h>
#include <HighPowerStepperDriver.h> //You may need to download the library to use. 
//Library is called HighPowerStepperDriver by Pololu

const uint8_t CSPin = 4; //This is for the Stepper Driver
const int buttonPin = 8; //This is where your start button is connected and defined
const int switchBeginning = 9; //This is the limit switch at the beginning
const int switchAtMotor = 7; //This is the limit switch at the motor
const int stopButton = 5; //Stop button
const int resetButton = 6; //Reset button back to start position

//The intial states of all the buttons
int buttonState = 0;  //Should be Zero
int switchBeginningState = 0; //Should be Zero
int switchAtMotorState = 0; //Should be Zero
int stopButtonState = 0; //Should be Zero
int resetState = 0; //Should be Zero
int direction = 1; //Negative moves backwards while positive moves forward
bool startButton = false;

const uint16_t StepPeriodUs = 10;// This period is the length of the delay between steps, which controls the
// stepper motor's speed.  You can increase the delay to make the stepper motor
// go slower.  If you decrease the delay, the stepper motor will go faster, but
// there is a limit to how fast it can go before it starts missing steps.

HighPowerStepperDriver sd; //Here are important for the stepper driver

void setup()
{
  Serial.begin(9600); //This is needed for the serial Moniter which is good for debugging
  SPI.begin();
  sd.setChipSelectPin(CSPin);
  pinMode(buttonPin, INPUT);
  delay(500); // Give the driver some time to power up. 
  Serial.print("Finished Delay"); //For debugging
  
  sd.resetSettings(); // Reset the driver to its default settings and clear latched status conditions.
  sd.clearStatus();

    sd.setDecayMode(HPSDDecayMode::AutoMixed);// Select auto mixed decay.  TI's DRV8711 documentation recommends this mode
  // for most applications, and we find that it usually works well.

  
  sd.setCurrentMilliamps36v4(2800);// Set the current limit. You should change the number here to an appropriate
  // value for your particular system if not using the recommend motor and stepper driver

  // Set the number of microsteps that correspond to one full step.
  sd.setStepMode(HPSDStepMode::MicroStep32);

  // Enable the motor outputs.
  sd.enableDriver();
  //sd.setDirection(0);
}

void loop()
{
  //This allows for the buttons to be read without this nothing will happen
  buttonState = digitalRead(buttonPin);
  switchBeginningState = digitalRead(switchBeginning);
  switchAtMotorState = digitalRead(switchAtMotor);
  resetState = digitalRead(resetButton);
  stopButtonState = digitalRead(stopButton);
 
  //Start of the motor state
  //direction(0) goes away from motor
  //direction(1) goes towards motor
  if(stopButtonState == LOW) //If the Stop Button is not pressed so its normal state
  {
    if(startButton == false) //Before Start button is pressed or after whole cycle or after stop button it will return here
     {
      if(switchBeginningState == HIGH)// this is what sets up the motor moving reading where it is at 
      {
        if(buttonState == HIGH)//button hit
        {
          startButton = true;
          direction = -1;
          sd.setDirection(0);
          Serial.println("ButtonPressed"); //Here for debugging
        }
      }
      else if(resetState == HIGH) { //Returns to start position
        startButton = true;
        direction = 0;
        Serial.println("Reset Hit"); //Here for debugging
        sd.setDirection(1);
      }
     }
     else //When the Start Button is pressed
     {
      if(direction < 0)
      {
        if(switchAtMotorState == HIGH) //Move towards the motor point
        {
          direction = 1;
          Serial.println("SwitchAtMotor Switched directions"); //Here for debugging
          Serial.println("Going Back"); //Here for debugging
          sd.setDirection(1);
        }
        else 
        {
          sd.step(); //this moves the motor
          delayMicroseconds(StepPeriodUs); //How fast the motor is moving
        }
      }
      else if(direction == 0) //Return to Start from restart button
      {
        if(switchBeginningState == HIGH){
          direction = -1;
          Serial.println("Resetting"); //Here for debugging
          sd.setDirection(1);
          startButton = false;
        }
        else
        {
          sd.step(); //this moves the motor
          delayMicroseconds(StepPeriodUs); //How fast the motor is moving
        }
      }
      else
      {
        if(switchBeginningState == HIGH) //move towards motor
        {
          startButton= false;
          direction = -1;
          //Serial.print("Stop"); //Here for debugging
          sd.setDirection(0);
          Serial.println("Going Forward"); //Here for debugging
        }
        else 
        {
          sd.step(); //this moves the motor
          delayMicroseconds(StepPeriodUs); //How fast the motor is moving

        }
      }      
    }
  }
  else //If the stop button is pressed
  {
    startButton = false; //this will stop the deviece
    Serial.println("Stop"); //Here for debugging
  }
}
