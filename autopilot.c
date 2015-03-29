#include <math.h>
#define MARGIN_OF_ERROR 1
#define MAX_FRONT_DIST 10
#define MAX_SIDE_DIST 10
#define MIN_FRONT_DIST 2
#define MIN_SIDE_DIST 2
#define ROBOT_WIDTH 12
#define DEFAULT_SPEED 180

State state = stopped;
const float WHEEL_RADIUS = 0.695; //1.75 cm

//array of infrared sensor pins
	
const int infraredSensorPins[6] = {A0, A1, A2, ?, ?, ?};

//array of init and echo pins for ultrasonic sensors

const int echoPinUltraSensors[6] = {2, 6, 8, 12, ?, ?};
const int initPinUltraSensors[6] = {3, 7, 9, 13, ?, ?};

//Arrays of distances for sensors.

int UltraSensorDists[] = new int[6];
int InfraSensorDists[] = new int[6];


int main()
{
	//Initialize the sensors.
	
	initSensors();
	
	//Execute forever until robot stops (indicating error).
	
	do
	{
		pingAll();
	
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
		
		if((UltraSensors[front_l] < MAX_FRONT_DIST && UltraSensors[front_r] < MAX_FRONT_DIST) &&
		(UltraSensors[front_l] > MIN_FRONT_DIST || UltraSensors[front_l] > MIN_FRONT_DIST))
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
					//turnRight(UltraSensors[front_r]-UltraSensors[front_l])
					state = turning;
				}
				else
				{
					printf("Robot trapped. Stopping...\n");
					//stop();
					state = stopped;
				}
			}
			else if(UltraSensors[front_l] > UltraSensors[front_r])
			{
				//Only turn left if there is a forward gradient in the left wall or the wall does not exist going forward.
				
				if(UltraSensors[left_f] > MAX_SIDE_DIST || UltraSensors[left_f] > UltraSensors[left_b])
				{
					printf("Turning left...\n");
					//turnLeft(UltraSensors[front_l]-UltraSensors[front_r])
					state = turning;
				}
				else
				{
					printf("Robot trapped. Stopping...\n");
					//stop();
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
					//turnLeft(0);
					state = turning;
				 }
				 else if(UltraSensors[right_f] > MAX_SIDE_DIST)
				 {
					printf("Turning right...\n");
					//turnRight(0);
					state = turning;
				 }
				 else
				 {
					printf("Robot trapped. Stopping...\n");
					//stop();
					state = stopped;
				 }
			}
		}
		else if(state == turning)
		{
			printf("Going straight...\n");
			//goStraight();
			state = going_straight;
		}
		else if(UltraSensors[left_f] < MIN_SIDE_DIST)
		{
			printf("Too close! Moving to the right...\n");
			//scootRight();
		}
		else if(UltraSensors[right_f] < MIN_SIDE_DIST)
		{
			printf("Too close! Moving to the left...\n");
			//scootLeft();
		}
		
		//Wait 50 ms before continuing.
		
		delay(50);
	}
	while(state != stopped)
	return 0;
}

//Initialize sensors.

void initSensors()
{
	//Set up the ultrasonic sensors.
	
	int i = 0;
	for(; i < initPinUltraSensors.length; i++) pinMode(initPinUltraSensors[i], INPUT);
	int j = 0;
	for(; j < echoPinUltraSensors.length; j++) pinMode(echoPinUltraSensors[j], OUTPUT);
}

//Ping all sensors.

void pingAll()
{
	int i = 0;
	int j = 0;
	for(; i < infras.length; i++)	InfraSensorDists[i] = analogRead(InfraredSensorPins[i]);
	for(; j < ultras.length; j++)	UltraSensorDists[j] = getDistance(initPinUltraSensors[j], echoPinUltraSensors[j]);
 
}

//Helper function to get distance from ultrasonic sensors.

int getDistance (int initPin, int echoPin)
{
	digitalWrite(initPin, HIGH);
	delayMicroseconds(10); 
	digitalWrite(initPin, LOW); 
	unsigned long pulseTime = pulseIn(echoPin, HIGH); 
	int distance = pulseTime/58;
	return distance;
}
//Send sensor signal to turn right based on a certain gradient.

void turnRight(float gradient)
{
	motors[motor_front_r].write(DEFAULT_SPEED + atanf(gradient/ROBOT_WIDTH) * ROBOT_WIDTH) / WHEEL_RADIUS);
	motors[motor_back_r].write(DEFAULT_SPEED + atanf(gradient/ROBOT_WIDTH) * ROBOT_WIDTH) / WHEEL_RADIUS);
	motors[motor_front_l].write(DEFAULT_SPEED / WHEEL_RADIUS);
	motors[motor_back_l].write(DEFAULT_SPEED / WHEEL_RADIUS);
}

//Send sensor signal to turn left based on a certain gradient.

void turnLeft(float gradient)
{
	motors[motor_front_l].write(DEFAULT_SPEED + atanf(gradient/ROBOT_WIDTH) * ROBOT_WIDTH) / WHEEL_RADIUS);
	motors[motor_back_l].write(DEFAULT_SPEED + atanf(gradient/ROBOT_WIDTH) * ROBOT_WIDTH) / WHEEL_RADIUS);
	motors[motor_front_r].write(DEFAULT_SPEED / WHEEL_RADIUS);
	motors[motor_back_r].write(DEFAULT_SPEED / WHEEL_RADIUS);
}

//Send sensor signal to stop.

void stop()
{
	motors[motor_front_l].write(0);
	motors[motor_back_l].write(0);
	motors[motor_front_r].write(0);
	motors[motor_back_r].write(0);
}

//Straighten out the robot.

void goStraight()
{
	motors[motor_front_r].write(DEFAULT_SPEED / WHEEL_RADIUS);
	motors[motor_back_r].write(DEFAULT_SPEED / WHEEL_RADIUS);
	motors[motor_front_l].write(DEFAULT_SPEED / WHEEL_RADIUS);
	motors[motor_back_l].write(DEFAULT_SPEED / WHEEL_RADIUS);
}

//Move to the right for a short distance, then straighten out.

void scootRight()
{
	turnRight(ROBOT_WIDTH / 2);
	wait(5);
	turnLeft(ROBOT_WIDTH / 2);
	wait(5);
	goStraight();
}

//Move to the left for a short distance, then straighten out.

void scootLeft()
{
	turnLeft(ROBOT_WIDTH / 2);
	wait(5);
	turnRight(ROBOT_WIDTH / 2);
	wait(5);
	goStraight();
}