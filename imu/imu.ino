/*
   IMU section of code adapted and modified. Original source below.
   Arduino and MPU6050 Accelerometer and Gyroscope Sensor Tutorial
   by Dejan, https://howtomechatronics.com
*/


#include <Wire.h>

#define ACC_LSB_SENSITIVITY 16384
#define GYRO_LSB_SENSITIVITY 131
#define ITERATIONS 1000
#define THROWAWAY 100

#define BUZZER 4

//40Hz
#define THRESHOLD 25
// 50Hz
//#define THRESHOLD 20
// 60Hz
//#define THRESHOLD 17

bool has_thrown = false;
bool has_calibrate = false;

const int MPU = 0x68; // MPU6050 I2C address
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float AccX_avg, AccY_avg, AccZ_avg;
float GyroX_avg, GyroY_avg, GyroZ_avg;
float AccErrorX, AccErrorY, AccErrorZ, GyroErrorX, GyroErrorY, GyroErrorZ;
int counter = 0;
int reading_number = 0;
unsigned long current_time = 0;

void setup() {
  pinMode(BUZZER, OUTPUT);
  Serial.begin(19200);
  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission
  
  delay(20);
}

void loop() {
  if(has_calibrate == false) {
    calibrate_imu();
    has_calibrate = true;
  }

  // === Read acceleromter data === //
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  
  //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  AccX = (Wire.read() << 8 | Wire.read()); // X-axis value
  AccY = (Wire.read() << 8 | Wire.read()); // Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read()); // Z-axis value
  
  // === Read gyroscope data === //
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers

  // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  GyroX = (Wire.read() << 8 | Wire.read());
  GyroY = (Wire.read() << 8 | Wire.read());
  GyroZ = (Wire.read() << 8 | Wire.read());

  reading_number++;
  
  // Correct the outputs with the calculated error values as obtained from calibrate_imu
  AccX -= AccErrorX;
  AccY -= AccErrorY;
  AccZ -= AccErrorZ;
  GyroX -= GyroErrorX;
  GyroY -= GyroErrorY;
  GyroZ -= GyroErrorZ;

  // Divide by respective values
  AccX /= ACC_LSB_SENSITIVITY;
  AccY /= ACC_LSB_SENSITIVITY;
  AccZ /= ACC_LSB_SENSITIVITY;
  GyroX /= GYRO_LSB_SENSITIVITY;
  GyroY /= GYRO_LSB_SENSITIVITY;
  GyroZ /= GYRO_LSB_SENSITIVITY;

  AccX_avg += AccX;
  AccY_avg += AccY;
  AccZ_avg += AccZ;
  GyroX_avg += GyroX;
  GyroY_avg += GyroY;
  GyroZ_avg += GyroZ;

  // THRESHOLD is 25ms = 40Hz. Sampling at 40Hz
  if(millis() - current_time > THRESHOLD) {
    current_time = millis();
    AccX_avg /= reading_number;
    AccY_avg /= reading_number;
    AccZ_avg /= reading_number;
    GyroX_avg /= reading_number;
    GyroY_avg /= reading_number;
    GyroZ_avg /= reading_number;
    Serial.print(AccX_avg);
    Serial.print(" / ");
    Serial.print(AccY_avg);
    Serial.print(" / ");
    Serial.println(AccZ_avg);
    Serial.print(GyroX_avg);
    Serial.print(" / ");
    Serial.print(GyroY_avg);
    Serial.print(" / ");
    Serial.println(GyroZ_avg);
    Serial.println();
    reading_number = 0;
  }
}

void calibrate_imu() {
  // We can call this funtion in the setup section to calculate the accelerometer and gyro data error. From here we will get the error values used in the above equations printed on the Serial Monitor.
  // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
  // Read values 1000 times
  // Throw away the first 100 values as they tend to be the most inaccurate

  while(counter < THROWAWAY && has_thrown == false) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    AccErrorX += (Wire.read() << 8 | Wire.read());
    AccErrorY += (Wire.read() << 8 | Wire.read());
    AccErrorZ += (Wire.read() << 8 | Wire.read());

    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    GyroErrorX += (Wire.read() << 8 | Wire.read());
    GyroErrorY += (Wire.read() << 8 | Wire.read());
    GyroErrorZ += (Wire.read() << 8 | Wire.read());

    if(counter == THROWAWAY - 1) {
      counter = 0;
      has_thrown = !has_thrown;
      delay(1000);
    } else
      counter++;
  }
  
  while (counter < ITERATIONS) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    AccErrorX += (Wire.read() << 8 | Wire.read());
    AccErrorY += (Wire.read() << 8 | Wire.read());
    AccErrorZ += (Wire.read() << 8 | Wire.read());

    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    GyroErrorX += (Wire.read() << 8 | Wire.read());
    GyroErrorY += (Wire.read() << 8 | Wire.read());
    GyroErrorZ += (Wire.read() << 8 | Wire.read());
    
    counter++;
  }
  
  //Divide the sum by iteration to get the average error value
  AccErrorX /= ITERATIONS;
  AccErrorY /= ITERATIONS;
  AccErrorZ /= ITERATIONS;
  GyroErrorX /= ITERATIONS;
  GyroErrorY /= ITERATIONS;
  GyroErrorZ /= ITERATIONS; 

  // Print the error values on the Serial Monitor
  Serial.print("AccErrorX: ");
  Serial.println(AccErrorX);
  Serial.print("AccErrorY: ");
  Serial.println(AccErrorY);
  Serial.print("AccErrorZ: ");
  Serial.println(AccErrorZ);
  Serial.print("GyroErrorX: ");
  Serial.println(GyroErrorX);
  Serial.print("GyroErrorY: ");
  Serial.println(GyroErrorY);
  Serial.print("GyroErrorZ: ");
  Serial.println(GyroErrorZ);

  // PLAY VIBRATION FEEDBACK ON BUZZER WHEN CALIBRATION IS COMPLETE
  play_buzzer();
}


void play_buzzer() {
 digitalWrite(BUZZER, HIGH);
 delay(100);
 digitalWrite(BUZZER, LOW);
 delay(150);
 digitalWrite(BUZZER, HIGH);
 delay(100);
 digitalWrite(BUZZER, LOW);
 delay(200);
 digitalWrite(BUZZER, HIGH);
 delay(400);
 digitalWrite(BUZZER, LOW);
 delay(100);
}
