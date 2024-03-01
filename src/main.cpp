#include "main.h"

// Motor Ports
const int LEFT_CAT_MOTOR_PORT = 11;
const int RIGHT_CAT_MOTOR_PORT = 2;
const int Puncher_Motor_Port = 10;

int count = 0;

pros::ADIDigitalIn limit('A');

// Declarations
pros::Motor left_cat_motor(LEFT_CAT_MOTOR_PORT, pros::E_MOTOR_GEAR_GREEN, false,
                           pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor right_cat_motor(RIGHT_CAT_MOTOR_PORT, pros::E_MOTOR_GEAR_GREEN,
                              true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor puncher_motor(Puncher_Motor_Port, pros::E_MOTOR_GEAR_RED, true,
                          pros::E_MOTOR_ENCODER_DEGREES);

pros::Motor_Group catapult({left_cat_motor, right_cat_motor});

pros::Controller master(pros::E_CONTROLLER_MASTER);

/**
 * This function moves the puncher_motor 165 degrees,
 * then stops for half a second and then moves back to its original position
 */
void punch() {
  puncher_motor.move_relative(140, 70);
  pros::delay(500);
  puncher_motor.move_relative(-140, 70);
  pros::delay(1000);
  puncher_motor.brake();
}

void initialize() { puncher_motor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST); }

void disabled() {}

void competition_initialize() {}

// We have one preload, one alliance triball, and 10 match loads that can be introduced in autonomous
// Small Robot also has one preload
void autonomous() {
  pros::delay(5000);
  // Run the catapult once which is 723 degrees to lauch preload
  catapult.move_relative(-723, 100);
  puncher_motor.move_relative(-100, 70);
  pros::delay(500); // 250ms not enough time.
  // Then loop to load and launch catapult 11 times.
  for (int i = 0; i < 23; i++) {  // 11 for normal matches
    punch(); // 2 seconds
    catapult.move_relative(-723, 100);
    pros::delay(250); // reduced from 500ms to 250ms to load while catpult is resetting
  }
}

double dabs(double v) { return v < 0 ? -v : v; }

void opcontrol() {
  while (true) {
    // Launch and reload the catapult
    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_A)) {
      catapult.move_relative(-723, 100);
      pros::delay(250);
      punch();
    // if the joystick is not at 0, move the catapult manuelly to fix cam issues
    } else if (master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y) != 0){
      const double MAX_RPM = 30.;
      int t = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
      if (t < -10 || t > 10)
        catapult.move_velocity((int)((double)(t) / 127. * MAX_RPM));
      else
        catapult.move_velocity(0);
    }
    /**
     * IMPORTANT: Drop the intake to get match load introduced
     * between autonomous and driver control
    */
    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)) {
      punch();
    }
    pros::delay(20);
  }
}