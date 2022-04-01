//seguidor linha

int Speed = 120;        // MAX 255

const int mspeed     = 0;
const int motor1A    = 1;
const int motor2A    = 3;

//Sensor Connection
const int left_pin = 2;
const int right_pin = 4;

void setup() {
  pinMode(left_pin, INPUT);
  pinMode(right_pin, INPUT);
  pinMode(motor1A, OUTPUT);
  pinMode(motor2A, OUTPUT);
  analogWrite (mspeed, Speed);
}

void loop() {

  unsigned char left_value = digitalRead(left_pin);
  unsigned char right_value = digitalRead(right_pin);

  if (right_value && left_value)
  {
    //em frente
    digitalWrite(motor1A, HIGH);
    digitalWrite(motor2A, HIGH);
  }
  else if (right_value && !left_value)
  {
    //vira direita
    digitalWrite(motor1A, LOW);
    digitalWrite(motor2A, HIGH);
  }
  else //if (!right_value  && left_value)
  {
    //vira esquerda
    digitalWrite(motor1A, HIGH);
    digitalWrite(motor2A, LOW);
  }

  delay(10);
}
