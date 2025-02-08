#include <Arduino.h>
#include <Servo.h>
#include <LiquidCrystal.h>
#include <vector>

#define JOINT_1_PIN 8
#define JOINT_2_PIN 9
#define JOINT_3_PIN 10
#define JOINT_4_PIN 11
#define JOINT_5_PIN 12

#define JOINT_1_START 0
#define JOINT_2_START 0
#define JOINT_3_START 0
#define JOINT_4_START 0
#define JOINT_5_START 0

const int rs = 0, en = 1, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Register the servo motors of each joint
Servo joint_1;
Servo joint_2; 
Servo joint_3; 
Servo joint_4; 
Servo joint_5; 


// Slow movement
// void reach_goal(Servo& motor, int goal){
//   if(goal>=motor.read()){
//     // goes from the start point degrees to the end point degrees
//     for (int pos = motor.read(); pos <= goal; pos += 1) { 
//       motor.write(pos);     
//       delay(10);                       
//     }
//   } else{
//     // goes from the end point degrees to the start point degrees
//     for (int pos = motor.read(); pos >= goal; pos -= 1) { 
//       motor.write(pos);     
//       delay(10);                       
//     }
//   }
// }

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

// Split String
std::vector<String> split_string(String data, char separator){
  std::vector<String> output;
  int startIndex = 0;
  int endIndex = 0;

  while ((endIndex = data.indexOf(separator, startIndex)) != -1) {
    output.push_back(data.substring(startIndex, endIndex));
    startIndex = endIndex + 1;
  }
  output.push_back(data.substring(startIndex));
  delay(1000);
  return output;
}

void setup() {
  digitalWrite(2, LOW); 
  joint_1.attach(JOINT_1_PIN);
  joint_2.attach(JOINT_2_PIN);
  joint_3.attach(JOINT_3_PIN);
  joint_4.attach(JOINT_4_PIN);
  joint_5.attach(JOINT_5_PIN);

  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(2, HIGH);
  joint_1.write(JOINT_1_START);
  joint_2.write(JOINT_2_START);
  joint_3.write(JOINT_3_START);
  joint_4.write(JOINT_4_START);
  joint_5.write(JOINT_5_START);
  digitalWrite(2, LOW);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(115200);
  Serial.setTimeout(1);
  lcd.begin(16, 2);

  delay(2000);  
}

void loop() {
  if (Serial.available())
  {
    String data = Serial.readString();
    std::vector<String> joint_commands = split_string(data, '-');
    if(joint_commands.size() == 5){
      int goals[5] = {
        joint_commands[0].toInt(), 
        joint_commands[1].toInt(), 
        joint_commands[2].toInt(), 
        joint_commands[3].toInt(), 
        joint_commands[4].toInt()
        };
      Servo motors[] = {joint_1, joint_2, joint_3, joint_4, joint_5};

      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(2, HIGH);
      reach_goal(motors, goals, 1);
      digitalWrite(2, LOW);
      digitalWrite(LED_BUILTIN, LOW);

      lcd.setCursor(0, 0);
      for (int i = 0; i < 5; i++){
        lcd.print(goals[i]);
        lcd.print("-");
      }
    }
  }
}
