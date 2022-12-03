/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleWrite.cpp
    Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Adafruit_LSM6DS3TRC.h>

Adafruit_LSM6DS3TRC lsm6ds3trc;
uint32_t ref = 0U;
uint32_t dur = 100U;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_ACCEL_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define SERVICE_GYRO_UUID        "4fafc202-1fb5-459e-8fcc-c5c9c331914b"
#define SERVICE_TEMP_UUID        "4fafc203-1fb5-459e-8fcc-c5c9c331914b"

#define CHAR_ACCEL_X_UUID "b7ad0680-4aca-4b27-8f4b-8592a6e53d3e"
#define CHAR_ACCEL_Y_UUID "b7ad0681-4aca-4b27-8f4b-8592a6e53d3e"
#define CHAR_ACCEL_Z_UUID "b7ad0682-4aca-4b27-8f4b-8592a6e53d3e"
#define CHAR_GYRO_X_UUID "b7ad0683-4aca-4b27-8f4b-8592a6e53d3e"
#define CHAR_GYRO_Y_UUID "b7ad0684-4aca-4b27-8f4b-8592a6e53d3e"
#define CHAR_GYRO_Z_UUID "b7ad0685-4aca-4b27-8f4b-8592a6e53d3e"
#define CHAR_TEMP_UUID "b7ad0686-4aca-4b27-8f4b-8592a6e53d3e"


class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    
  };
  void onDisconnect(BLEServer* pServer) {
    // Automatically restart advertising if connection is terminated
    pServer->getAdvertising()->start();
  }
};

BLEServer *pServer;
BLEService *service_accel;
BLEService *service_gyro;
BLEService *service_temp;
BLECharacteristic *char_accel_x;
BLECharacteristic *char_accel_y;
BLECharacteristic *char_accel_z;
BLECharacteristic *char_gyro_x;
BLECharacteristic *char_gyro_y;
BLECharacteristic *char_gyro_z;
BLECharacteristic *char_temp;

void setup() {
//  Serial.begin(115200);

  if (!lsm6ds3trc.begin_I2C()) {
    while (1) {
      delay(10);
    }
  }

  lsm6ds3trc.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
  lsm6ds3trc.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS);
  lsm6ds3trc.setAccelDataRate(LSM6DS_RATE_1_66K_HZ);
  lsm6ds3trc.setGyroDataRate(LSM6DS_RATE_1_66K_HZ);
  lsm6ds3trc.configInt1(false, false, true); // accelerometer DRDY on INT1
  lsm6ds3trc.configInt2(false, true, false); // gyro DRDY on INT2

  BLEDevice::init("NailBiter");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  service_accel = pServer->createService(SERVICE_ACCEL_UUID);
  char_accel_x = service_accel->createCharacteristic(CHAR_ACCEL_X_UUID, BLECharacteristic::PROPERTY_READ);
  char_accel_y = service_accel->createCharacteristic(CHAR_ACCEL_Y_UUID, BLECharacteristic::PROPERTY_READ);
  char_accel_z = service_accel->createCharacteristic(CHAR_ACCEL_Z_UUID, BLECharacteristic::PROPERTY_READ);

  service_gyro = pServer->createService(SERVICE_GYRO_UUID);
  char_gyro_x = service_gyro->createCharacteristic(CHAR_GYRO_X_UUID, BLECharacteristic::PROPERTY_READ);
  char_gyro_y = service_gyro->createCharacteristic(CHAR_GYRO_Y_UUID, BLECharacteristic::PROPERTY_READ);
  char_gyro_z = service_gyro->createCharacteristic(CHAR_GYRO_Z_UUID, BLECharacteristic::PROPERTY_READ);

  service_temp = pServer->createService(SERVICE_TEMP_UUID);
  char_temp = service_temp->createCharacteristic(CHAR_TEMP_UUID, BLECharacteristic::PROPERTY_READ);

  float temp = 0.0;
  char_accel_x->setValue(temp);
  char_accel_y->setValue(temp);
  char_accel_z->setValue(temp);
  char_gyro_x->setValue(temp);
  char_gyro_y->setValue(temp);
  char_gyro_z->setValue(temp);
  char_temp->setValue(temp);
  
  service_accel->start();
  service_gyro->start();
  service_temp->start();

  pServer->getAdvertising()->start();

  ref = millis();
}

void loop() {
  if (millis() >= ref + dur)
  {
    ref = millis();
    
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    lsm6ds3trc.getEvent(&accel, &gyro, &temp);
  
    char_accel_x->setValue(accel.acceleration.x);
    char_accel_y->setValue(accel.acceleration.y);
    char_accel_z->setValue(accel.acceleration.z);

    char_gyro_x->setValue(gyro.gyro.x);
    char_gyro_y->setValue(gyro.gyro.y);
    char_gyro_z->setValue(gyro.gyro.z);

    char_temp->setValue(temp.temperature);
  }
}
