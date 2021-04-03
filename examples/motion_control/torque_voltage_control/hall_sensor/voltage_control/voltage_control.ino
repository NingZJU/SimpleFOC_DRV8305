/**
 * 
 * Torque control example using voltage control loop.
 * 
 * Most of the low-end BLDC driver boards doesn't have current measurement therefore SimpleFOC offers 
 * you a way to control motor torque by setting the voltage to the motor instead of the current. 
 * 
 * This makes the BLDC motor effectively a DC motor, and you can use it in a same way.
 */
#include <SimpleFOC.h>

// DRV8305 pins connections
// don't forget to connect the common ground pin
#define   INH_A   9
#define   INH_B   10
#define   INH_C   11
#define   EN_GATE 8
#define   HALL_A  5
#define   HALL_B  6
#define   HALL_C  7

#define   POLE_PAIRS  8

// BLDC motor instance
BLDCMotor motor = BLDCMotor(POLE_PAIRS);
// BLDC driver instance
BLDCDriver3PWM driver = BLDCDriver3PWM(INH_A, INH_B, INH_C, EN_GATE);
// Stepper motor & driver instance
//StepperMotor motor = StepperMotor(50);
//StepperDriver4PWM driver = StepperDriver4PWM(9, 5, 10, 6,  8);

// hall sensor instance
HallSensor sensor = HallSensor(HALL_A, HALL_B, HALL_C, POLE_PAIRS);

// Interrupt routine intialisation
// channel A and B callbacks
void doA(){sensor.handleA();}
void doB(){sensor.handleB();}
void doC(){sensor.handleC();}

// commander interface // TODO: may not work
Commander command = Commander(Serial);
void onMotor(char* cmd){ command.motor(&motor, cmd); }

void setup() { 
  
  // initialize encoder sensor hardware
  sensor.init();
  sensor.enableInterrupts(doA, doB, doC); 
  // link the motor to the sensor
  motor.linkSensor(&sensor);

  // driver config
  // TODO: may add some config to initialize DRV8305

  // power supply voltage [V]
  driver.voltage_power_supply = 14.8;
  driver.init();
  // link driver
  motor.linkDriver(&driver);

  // aligning voltage
  motor.voltage_sensor_align = 3;
  
  // choose FOC modulation (optional)
  motor.foc_modulation = FOCModulationType::SpaceVectorPWM;

  // set motion control loop to be used
  motor.controller = ControlType::voltage;

  // controller configuration based on the control type 
  motor.PID_velocity.P = 0.2;
  motor.PID_velocity.I = 20;

  // velocity low pass filtering time constant
  motor.LPF_velocity.Tf = 0.01;

  // angle loop controller
  motor.P_angle.P = 20;
  
  // angle loop velocity limit
  motor.velocity_limit = 50;
  // default voltage_power_supply
  motor.voltage_limit = 24;

  // use monitoring with serial 
  Serial.begin(115200);
  // comment out if not needed
  motor.useMonitoring(Serial);

  // initialize motor
  motor.init();
  // align sensor and start FOC
  motor.initFOC();

  // define the motor id
  command.add('M', onMotor, "motor");

  Serial.println("Motor ready.");
  Serial.println("Set the target voltage using serial terminal:");
  _delay(1000);
}

// target voltage to be set to the motor
float target_voltage = 2;

void loop() {

  // main FOC algorithm function
  // the faster you run this function the better
  // Arduino UNO loop  ~1kHz
  // Bluepill loop ~10kHz 
  motor.loopFOC();

  // Motion control function
  // velocity, position or voltage (defined in motor.controller)
  // this function can be run at much lower frequency than loopFOC() function
  // You can also use motor.move() and set the motor.target in the code
  motor.move(target_voltage);
  
  // communicate with the user
  serialReceiveUserCommand();

  command.run();
}


// utility function enabling serial communication with the user to set the target values
// this function can be implemented in serialEvent function as well
void serialReceiveUserCommand() {
  
  // a string to hold incoming data
  static String received_chars;
  
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the string buffer:
    received_chars += inChar;
    // end of user input
    if (inChar == '\n') {
      
      // change the motor target
      target_voltage = received_chars.toFloat();
      Serial.print("Target voltage: ");
      Serial.println(target_voltage);
      
      // reset the command buffer 
      received_chars = "";
    }
  }
}
