#define CLK_PIN PD2 // INT0
#define DT_PIN PD3  // INT1
#define SW_PIN PD4

//Funtion pointer declaration
void (*cw_rot_ptr)(int);
void (*ccw_rot_ptr)(int);

void init_rotary_encoder(void (*cw_rot_fcn)(int), void (*ccw_rot_fcn)(int), int cw_event, int ccw_event);
