// Open loop motor control example
#include <SimpleFOC.h>

// DRV8305 pins connections
// don't forget to connect the common ground pin
#define   INH_A   5
#define   INH_B   6
#define   INH_C   7
#define   EN_GATE 8
// SPI
#define   SCK     13
#define   MISO    12
#define   MOSI    11
#define   CS      10

#define   WAKE    2

#define   POLE_PAIRS  8

// BLDC motor instance
BLDCMotor motor = BLDCMotor(POLE_PAIRS);
// BLDC driver instance
BLDCDriver3PWM driver = BLDCDriver3PWM(INH_A, INH_B, INH_C, EN_GATE);
// Stepper motor & driver instance
//StepperMotor motor = StepperMotor(50);
//StepperDriver4PWM driver = StepperDriver4PWM(9, 5, 10, 6,  8);

// SPI
char *mode_setting;

void setup() {
    // driver config
    // TODO: may add some config to initialize DRV8305
    pinMode(WAKE, OUTPUT);
    digitalWrite(WAKE, HIGH);

    // power supply voltage [V]
    driver.voltage_power_supply = 16.8;
    driver.init();
    // link the motor and the driver
    motor.linkDriver(&driver);

    // limiting motor movements
    motor.voltage_limit = 24;   // [V]
    motor.velocity_limit = 30; // [rad/s]

    // open loop control config
    motor.controller = ControlType::velocity_openloop;

    // init motor hardware
    motor.init();

    Serial.begin(115200);
    Serial.println("Motor ready!");
    _delay(1000);
}

float target_velocity = 0; // [rad/s]

void loop() {
    // open loop velocity movement
    // using motor.voltage_limit and motor.velocity_limit
    motor.move(target_velocity);

    // receive the used commands from serial
    serialReceiveUserCommand();
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
        target_velocity = received_chars.toFloat();
        Serial.print("Target velocity ");
        Serial.println(target_velocity);

        // reset the command buffer
        received_chars = "";
        }
    }
}


void SPI_Init()
{
    char c;
    char *data;
    // strcpy
    
    // SPI
    digitalWrite(SS, LOW); // disable Slave Select
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV16);//divide the clock by 8
    for (char c ; c = *mode_setting; modesett ++) {
        SPI.transfer (c);
        Serial.print(c);
    }
}
