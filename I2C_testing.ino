#include "SparkFun_LSM6DSV16X.h"
#include <Wire.h>

SparkFun_LSM6DSV16X myLSM;

// FIFO Constraints
#define FIFO_STATUS1        0x1B
#define FIFO_STATUS2        0x1C
#define FIFO_DATA_OUT       0x78

// TAG Decodes
#define TAG_GYRO            0x01
#define TAG_ACCEL           0x02
#define TAG_TIMESTAMP       0x04

const int D0 = 1;
const int D1 = 2;

struct fifoWord
{
    uint8_t rawTag;
    uint8_t sensorTag;
    int16_t x;
    int16_t y;
    int16_t z;
    uint32_t timestamp;
};

uint16_t getFifoWordCount()
{
    uint8_t status[2];

    myLSM.readRegisterRegion(FIFO_STATUS1, status, 2);

    return ((uint16_t)(status[1] & 0x01) << 8) | status[0];
}

void readFifoWord(fifoWord *word)
{
    uint8_t data[7];

    myLSM.readRegisterRegion(FIFO_DATA_OUT, data, 7);

    word->rawTag = data[0];
    word->sensorTag = data[0] >> 3;
    // Check if the packet is a timestamp tag (typically 0x04 for ST IMUs)
    if (word->sensorTag == TAG_TIMESTAMP) 
    {
        word->timestamp = ((uint32_t)data[4] << 24) | 
                          ((uint32_t)data[3] << 16) | 
                          ((uint32_t)data[2] << 8)  | 
                           (uint32_t)data[1];
        word->x = 0;
        word->y = 0;
        word->z = 0;
    }
    else
    {
        // Parse standard sensor data (Accelerometer / Gyroscope)
        word->x = (int16_t)(((uint16_t)data[2] << 8) | data[1]);
        word->y = (int16_t)(((uint16_t)data[4] << 8) | data[3]);
        word->z = (int16_t)(((uint16_t)data[6] << 8) | data[5]);
    }
}

// Structs for X,Y,Z data
sfe_lsm_data_t accelData;
sfe_lsm_data_t gyroData;

void setup()
{
    pinMode(D0, OUTPUT);
    pinMode(D1, OUTPUT);

    Wire.begin();
    // Set I2C clock to 1MHz
    Wire.setClock(1000000);

    Serial.begin(921600);
    while (!Serial)
    {
    }

    if (!myLSM.begin())
    {
        Serial.println("Did not begin, check your wiring and/or I2C address!");
        while (1)
            ;
    }

    // Reset the device to default settings. This if helpful is you're doing multiple
    // uploads testing different settings.
    myLSM.deviceReset();

    // Wait for it to finish reseting
    while (!myLSM.getDeviceReset())
    {
        delay(1);
    }

    Serial.println("Board has been Reset.");
    Serial.println("Applying settings.");

    // BDU: Accelerometer and Gyroscope registers will not be updated until read.
    myLSM.enableBlockDataUpdate();

    // Accelerometer
    myLSM.setAccelDataRate(LSM6DSV16X_ODR_AT_960Hz);
    myLSM.setAccelFullScale(LSM6DSV16X_8g);
    myLSM.setAccelMode(LSM6DSV16X_XL_HIGH_PERFORMANCE_MD);
    
    // Gyroscope
    myLSM.setGyroDataRate(LSM6DSV16X_ODR_AT_960Hz);
    myLSM.setGyroFullScale(LSM6DSV16X_500dps);
    myLSM.setGyroMode(LSM6DSV16X_GY_HIGH_PERFORMANCE_MD);

    // FIFO
    myLSM.setFifoWatermark(64);
    myLSM.setFifoMode(LSM6DSV16X_STREAM_MODE);
    myLSM.setAccelFifoBatchSet(LSM6DSV16X_XL_BATCHED_AT_960Hz);
    myLSM.setGyroFifoBatchSet(LSM6DSV16X_GY_BATCHED_AT_960Hz);
    myLSM.enableTimestamp();
    myLSM.setFifoTimestampDec(LSM6DSV16X_TMSTMP_DEC_8);

    // SFLP: to be added soon

    // Filtering: to be added soon
    //myLSM.enableFilterSettling();
    // Accelerometer Filter
    //myLSM.enableAccelLP2Filter();
    //myLSM.setAccelLP2Bandwidth(LSM6DSV16X_XL_STRONG);
    // Gyroscope Filter    
    //myLSM.enableGyroLP1Filter();
    //myLSM.setGyroLP1Bandwidth(LSM6DSV16X_GY_ULTRA_LIGHT);

    Serial.println("Ready.");
}

void loop()
{
    uint16_t words = getFifoWordCount();

    while (words--) 
    {
        fifoWord word;
        readFifoWord(&word);
        
        switch(word.sensorTag)
        {
            case TAG_ACCEL:
                Serial.print("ACC: ");
                Serial.print(word.x);
                Serial.print(", ");
                Serial.print(word.y);
                Serial.print(", ");
                Serial.println(word.z);
                break;

            case TAG_GYRO:
                Serial.print("GRYO: ");
                Serial.print(word.x);
                Serial.print(", ");
                Serial.print(word.y);
                Serial.print(", ");
                Serial.println(word.z);
                
                break;

            case TAG_TIMESTAMP:
                Serial.print("TIMESTAMP: ");
                Serial.println(word.timestamp);
                break;

            default:
                Serial.print("OTHER: 0x");
                Serial.println(word.sensorTag, HEX);
            break;
        }
    
    }
}