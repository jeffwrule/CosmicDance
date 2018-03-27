// nothing in this file, check AAA_main.cpp for the primary source

/*
 * A timed motor can do one of two things;   
 * 
 * You can either run a motor at a given speed for a given time or 
 * 
 * You can run a motor for a number of rotations when paird with an interrupt routine
 * 
 *    NOTE: The interrupt routine was proving to be problematic when running multiple motors.  It works for a single
 *    motor but not for multiple.   No word yet on what is wrong with it yet.
 * 
 * Each move can have a delay before it starts; That dealy can be from the time the dance started DELAY_TYPE_DANCE ('d')
 * or a delay that is based on the time the current DanceMove started DELAY_TYPE_MOVE ('m')
 * 
 * There is support for a 1 time dealy at the start of a dance.
 *    NOTE: unline the PingPongCentered motor there is no extended run after the move complets to do a final position for the motor
 * 
 * A TimedMotor Motor just spins in a single direction and can vary speeds of that spinning via a new DanceMove as configured.  
 * 
 * The action consists of a number of DanceMoves that have a Delay, RunTime, and optionally a number of Rotations.  
 * The software switches to the next DanceMove when the delay has been satisified 
 *    and either the number of rotations is reached or the run time has been exhausted.
 * 
 *    Each move can have a delay before it starts; That dealy can be from the time the DANCE started, DELAY_TYPE_DANCE ('d')
 *    OR a delay that is based on the time the current DANCEMOVE started DELAY_TYPE_MOVE ('m')
 * 
 * All DanceMoves and delays will be excuted on a repeat except for the 1 time Dance delay, that is only executed at the start of the dance.
 *    NOTE: this 1 time dealy is often used to wiat to start the motor until the music reaches a level acceptable to cover the motor noise.
 * 
 */
