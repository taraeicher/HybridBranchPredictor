typedef enum STATE {turning, going_straight, stopped} State;
enum SENSOR_POS {left_b, left_f, front_l, front_r, right_f, right_b};
enum MOTOR_POS {motor_front_l, motor_front_r, motor_back_r, motor_back_l};

void pingAll();
void handleSensorError();
void turnRight(int oppositeSideLength);
void turnLeft(int oppositeSideLength);
void stop();
void goStraight();
void scootRight();
void scootLeft();
