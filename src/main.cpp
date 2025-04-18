#include <Arduino.h>
#include <Servo.h>
#include <LiquidCrystal.h>
#include <vector>

#define JOINT_1_PIN 12
#define JOINT_2_PIN 11
#define JOINT_3_PIN 10
#define JOINT_4_PIN 9
#define JOINT_5_PIN 8
#define GRIPPER_OPEN 7
#define GRIPPER_CLOSE 6
#define GRIPPER_LIMIT 5

#define JOINT_1_START 90
#define JOINT_2_START 90
#define JOINT_3_START 0
#define JOINT_4_START 175
#define JOINT_5_START 90

// const int rs = 0, en = 1, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
// LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

Servo joint_1;
Servo joint_2; 
Servo joint_3; 
Servo joint_4; 
Servo joint_5; 

int gripper_state = -1;

void motors_init(){
  joint_1.write(JOINT_1_START);
  joint_2.write(JOINT_2_START);
  joint_3.write(JOINT_3_START);
  joint_4.write(JOINT_4_START);
  joint_5.write(JOINT_5_START);
}


void gripper_init(){
  digitalWrite(GRIPPER_OPEN, LOW); 
  digitalWrite(GRIPPER_CLOSE, LOW);

  while (digitalRead(GRIPPER_LIMIT) == LOW) {
    digitalWrite(GRIPPER_OPEN, HIGH);
    digitalWrite(GRIPPER_CLOSE, LOW);
    digitalWrite(LED_BUILTIN, HIGH);
    if (digitalRead(GRIPPER_LIMIT) == HIGH) {
      break;
    }
    delay(1);
  }
  digitalWrite(GRIPPER_OPEN, LOW);
  digitalWrite(GRIPPER_CLOSE, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  gripper_state = 0;
}

void reach_goal(Servo motors[], int goals[], int delay_){
  int read_1 = motors[0].read();
  int read_2 = motors[1].read();
  int read_3 = motors[2].read();
  int read_4 = motors[3].read();
  int read_5 = motors[4].read();

  for (int clk=0; clk<180; clk++) { 
    if (read_1 < goals[0]) {
      read_1++;
      motors[0].write(read_1);
    }
    if (read_2 < goals[1]) {
      read_2++;
      motors[1].write(read_2);
    }
    if (read_3 < goals[2]) {
      read_3++;
      motors[2].write(read_3);
    }
    if (read_4 < goals[3]) {
      read_4++;
      motors[3].write(read_4);
    }
    if (read_5 < goals[4]) {
      read_5++;
      motors[4].write(read_5);
    }

    if (read_1 > goals[0]) {
      read_1--;
      motors[0].write(read_1);
    }
    if (read_2 > goals[1]) {
      read_2--;
      motors[1].write(read_2);
    }
    if (read_3 > goals[2]) {
      read_3--;
      motors[2].write(read_3);
    }
    if (read_4 > goals[3]) {
      read_4--;
      motors[3].write(read_4);
    }
    if (read_5 > goals[4]) {
      read_5--;
      motors[4].write(read_5);
    }

    delay(delay_);                       
  }
}

std::vector<String> split_string(String data, char separator){
  std::vector<String> output;
  int startIndex = 0;
  int endIndex = 0;

  while ((endIndex = data.indexOf(separator, startIndex)) != -1) {
    output.push_back(data.substring(startIndex, endIndex));
    startIndex = endIndex + 1;
  }
  output.push_back(data.substring(startIndex));
  return output;
}

int corrector(int x, float a, float b){
  return int((x * a) + b);
}

void setup() {
  pinMode(GRIPPER_LIMIT, INPUT);
  pinMode(GRIPPER_OPEN, OUTPUT);
  pinMode(GRIPPER_CLOSE, OUTPUT);

  joint_1.attach(JOINT_1_PIN);
  joint_2.attach(JOINT_2_PIN);
  joint_3.attach(JOINT_3_PIN);
  joint_4.attach(JOINT_4_PIN);
  joint_5.attach(JOINT_5_PIN);

  delay(10); 
  motors_init();
  gripper_init();
  Serial.begin(115200);
  Serial.setTimeout(1);
  // lcd.begin(16, 2);
  delay(500); 
}

void loop() {
  if (Serial.available())
  {
    String data = Serial.readString();
    std::vector<String> joint_commands = split_string(data, ',');
    if(joint_commands.size() == 5){
      int goals[5] = {
        corrector(joint_commands[0].toInt(), 1.0, 0.0) + 90,  // mapping -90 - 90 to 0 - 180
        corrector(joint_commands[1].toInt(), 0.7222, -5.0) + 90, 
        corrector(joint_commands[2].toInt(), -1.0, 0.0) + 90,
        corrector(joint_commands[3].toInt(), 1.0, 0.0) + 90,
        corrector(joint_commands[4].toInt(), 1.0, 0.0) + 90
        };
      Servo motors[] = {joint_1, joint_2, joint_3, joint_4, joint_5};

      digitalWrite(LED_BUILTIN, HIGH);
      reach_goal(motors, goals, 0);
      digitalWrite(LED_BUILTIN, LOW);

      String response = 
          "F" +
          joint_commands[0] + "," + 
          joint_commands[1] + "," + 
          joint_commands[2] + "," + 
          joint_commands[3] + "," + 
          joint_commands[4];

      Serial.println(response);

      // lcd.clear();
      // lcd.setCursor(0, 0);
      // lcd.print(response);
    }
  }
}
