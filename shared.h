#include "autopilot.h"

typedef enum STATE {turning, going_straight, stopped} State;
enum SENSOR_POS {left_b, left_f, front_l, front_r, right_f, right_b};
extern State state;