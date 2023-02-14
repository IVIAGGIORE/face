#include <Servo.h>
#include <Wire.h>

typedef struct {
  int x_prc;
  int y_prc;
} data_servo_t;

static data_servo_t data_servo;

#define servomaxx 180.0
#define servomaxy 180.0

#define servopinx 9
#define servopiny 10
#define baudrate 115200
#define half(x) ((x)/2)

unsigned long time_last_get_data = 0;

Servo servox;
Servo servoy;

unsigned short MSB = 0;
unsigned short LSB = 0;
unsigned short MSBLSB = 0;


void ServoControl();
void ScanProcess();

void setup() {
  Serial.begin(baudrate) ;
  servoy.attach(servopiny);
  servox.attach(servopinx);
  delay(200);
  servox.write(half(servomaxx)); 
  delay(200);
  servoy.write(half(servomaxy));
  delay(200);

  Wire.begin(1);                                
  Wire.onReceive(ReceivI2C); 
}

void loop() {
  float scan = 0;
  unsigned short tick = 0;
  unsigned short tick2 = 30;
  
  if (millis() - time_last_get_data > 5000) ScanProcess();
  else ServoControl();
  
}

void ScanProcess()
{
  static float scan = 0;  
  static unsigned long  tick = 0;
  if (millis() - tick < 200) return;
  tick = millis();

  scan+=3.14/servomaxx;
  if (scan > 3.14*2) scan = 0.0;
  servox.write(sin(scan)*60+90);
  servoy.write(sin(scan*5)*15+100);
}

void ServoControl()
{
  static unsigned long  tick = 0;
  if (millis() - tick < 250) return;
  tick = millis();
  
  short posx = 0;
  int val_correct = 0;
  if (abs(data_servo.x_prc)>10)
  { 
    posx = servox.read();
    val_correct = data_servo.x_prc * 0.1;
    posx -= val_correct;
    data_servo.x_prc -= val_correct*2;
    constrain(posx, 0, servomaxx);
    servox.write(posx);
  }
  short posy = 0;  
  if (abs(data_servo.y_prc)>10)
  {
    posy = servoy.read();
    val_correct = data_servo.y_prc * 0.1;
    posy -= val_correct;
    data_servo.y_prc -= val_correct*2;
    constrain(posy, 0, servomaxy);
    servoy.write(posy);  
  }
}

void ReceivI2C()
{
  byte * ss = (byte*)&data_servo;
  for (int s=0; s<sizeof(data_servo); s++) {ss[s] = Wire.read(); }
  Serial.print("Resive X = ");
  Serial.print(data_servo.x_prc);
  Serial.println("%");
  Serial.print("Resive Y = ");
  Serial.print(data_servo.y_prc);
  Serial.println("%");
  time_last_get_data = millis();  
}
