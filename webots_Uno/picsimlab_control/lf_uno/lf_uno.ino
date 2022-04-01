//line follower

int Speed = 120;        // MAX 255

//L293 Connection
const int motor1A      = 8;
const int motor1B      = 9;
const int m1speed      = 10;
const int motor2A      = 12;
const int motor2B      = 13;
const int m2speed      = 11;

//Sensor Connection
const int left_pin = A0;
const int right_pin = A1;


void setup() {
  pinMode(motor1A, OUTPUT);
  pinMode(motor1B, OUTPUT);
  pinMode(motor2A, OUTPUT);
  pinMode(motor2B, OUTPUT);

  pinMode(left_pin, INPUT);
  pinMode(right_pin, INPUT);
}

void loop() {

  unsigned int left_value = analogRead(left_pin);
  unsigned int right_value = analogRead(right_pin);

  if ((right_value > 512)  && (left_value > 512) )
  {
    //going forward
    digitalWrite(motor1A, HIGH);
    digitalWrite(motor1B, LOW);
    digitalWrite(motor2A, LOW);
    digitalWrite(motor2B, HIGH);
    analogWrite (m1speed, Speed);
    analogWrite (m2speed, Speed);
  }
  else if ((right_value > 512) && (left_value < 512))
  {
    //turning right
    digitalWrite(motor1A, LOW);
    digitalWrite(motor1B, HIGH);
    digitalWrite(motor2A, LOW);
    digitalWrite(motor2B, HIGH);
    analogWrite (m1speed, Speed);
    analogWrite (m2speed, Speed);
  }
  else //if ((right_value < 512) && (left_value > 512))
  {
    //turning left
    digitalWrite(motor1A, HIGH);
    digitalWrite(motor1B, LOW);
    digitalWrite(motor2A, HIGH);
    digitalWrite(motor2B, LOW);
    analogWrite (m1speed, Speed);
    analogWrite (m2speed, Speed);
  }

  delay(10);
}
