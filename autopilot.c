#include <math.h>
#include "shared.h"
#define MARGIN_OF_ERROR 1
#define MAX_FRONT_DIST 10
#define MAX_SIDE_DIST 10
#define MIN_FRONT_DIST 2
#define MIN_SIDE_DIST 2
#define ROBOT_WIDTH 12

float UltraSensors[] = new float[6];
float InfraSensors[] = new float[6];

void choose()
{
	//Execute forever until robot stops (indicating error).
	
	do
	{
		pingAll(UltraSensors, InfraSensors);
	
		//Check for sensor errors.
	
		for (unsigned short i = 0; i < right_b; i++)
		{
			if(abs(UltraSensors[i]-InfraSensors[i]) > MARGIN_OF_ERROR) 
			{
				printf("Error between infrared sensor %d and ultrasonic sensor %d.\n", i, i);
				stop();
				state = stopped;
			}
		}
		
		//If there is a wall in front of the robot and close enough to be pertinent, turn in the correct direction.
		//If there is no wall in front and the robot is turning, go straight.
		//If the robot is going straight and is too close to the side, turn slightly and straighten out again. 
		
		if((UltraSensors[front_l] < MAX_FRONT_DIST && UltraSensors[front_r] < MAX_FRONT_DIST) && (UltraSensors[front_l] > MIN_FRONT_DIST || UltraSensors[front_l] > MIN_FRONT_DIST))
		{
			/*Determine whether there is a gradient in the wall and the direction of the gradient.
			 *Turn in the corresponding direction.
			 */
		
			if(UltraSensors[front_l] < UltraSensors[front_r])
			{
				//Only turn right if there is a forward gradient in the right wall or the wall does not exist going forward.
				
				if(UltraSensors[right_f] > MAX_FRONT_DIST || UltraSensors[right_f] >= UltraSensors[right_b])
				{
					printf("Turning right...\n");
					turnRight(UltraSensors[front_r]-UltraSensors[front_l])
					state = turning;
				}
				else
				{
					printf("Robot trapped. Stopping...\n");
					stop();
					state = stopped;
				}
			}
			else if(UltraSensors[front_l] > UltraSensors[front_r])
			{
				//Only turn left if there is a forward gradient in the left wall or the wall does not exist going forward.
				
				if(UltraSensors[left_f] > MAX_SIDE_DIST || UltraSensors[left_f] > UltraSensors[left_b])
				{
					printf("Turning left...\n");
					turnLeft(UltraSensors[front_l]-UltraSensors[front_r])
					state = turning;
				}
				else
				{
					printf("Robot trapped. Stopping...\n");
					stop();
					state = stopped;
				}
			}
			else
			{
				/*If there is no wall to the left, turn left. If no wall to the right, turn right (unless there is also no wall to the left). 
				 *If walls to both the right and left, stop.
				 */
				 
				 if(UltraSensors[left_f] > MAX_SIDE_DIST)
				 {
					printf("Turning left...\n");
					turnLeft(0);
					state = turning;
				 }
				 else if(UltraSensors[right_f] > MAX_SIDE_DIST)
				 {
					printf("Turning right...\n");
					turnRight(0);
					state = turning;
				 }
				 else
				 {
					printf("Robot trapped. Stopping...\n");
					stop();
					state = stopped;
				 }
			}
		}
		else if(state == turning)
		{
			printf("Going straight...\n");
			goStraight();
			state = going_straight;
		}
		else if(UltraSensors[left_f] < MIN_SIDE_DIST)
		{
			printf("Too close! Moving to the right...\n");
			scootRight();
		}
		else if(UltraSensors[right_f] < MIN_SIDE_DIST)
		{
			printf("Too close! Moving to the left...\n");
			scootLeft();
		}
		
		//Wait 1 ms before continuing.
		
		delay(1);
	}
	while(state != stopped)
}

//

void pingAll(float[] sensors1, float[] sensors2)
{
	
}

//Send sensor signal to turn right based on a certain gradient.

void turnRight(float gradient)
{
	float angle = atanf(gradient/ROBOT_WIDTH);
}

void turnLeft(float gradient);
void stop();
void goStraight();
void scootRight();
void scootLeft();