// nothing in this file, check AAA_main.cpp for the primary source


/* 
 * A PingPong Motor bounces from one limit (left or right) switch to the other (left or right).  
 * Left/Right can also be considered Top/Bottom in some scultptures.   
 * It is up the the software config to match the internal names (left/right/top/bottom) to the real world scuplture and wiring. 
 * 
 * The action consists of a number of DanceMoves that have a Delay, Run Time, and Direction.  If a limit switch is reached or the Run Time expires 
 * The software switches to the next DanceMove in the list, and wrapps around to the first DanceMove if necessary
 * 
 *    Each move can have a delay before it starts; That dealy can be from the TIME the DANCE STARTED DELAY_TYPE_DANCE ('d')
 *    OR a delay that is based on the time the CURRENT DanceMove started DELAY_TYPE_MOVE ('m')
 *
 *    A DanceMove can stop short of a limit switch if a given amount of time has been reached during the run.   
 * 
 * There is support for a third limit switch called the 'center'  This is where the sculpture returns to when it stops dancing. 
 * The center switch can be the same as the left or right switch or it can be an actual 3rd sensor.  The Life scuplture uses a true center switch (analog)
 * 
 * The software also supports an initial 1 time delay for the motor, before it starts executing moves.
 * 
 * NOTE1: The IBT2Motor has not be re-implemed using this new structure.
 * NOTE2: Support for other Analog limit switches needs to be added back
 * 
 */
