////////////////////////////////////////////////////////////////////////////
//
//  This file is part of RTIMULib
//
//  Copyright (c) 2014, richards-tech
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of
//  this software and associated documentation files (the "Software"), to deal in
//  the Software without restriction, including without limitation the rights to use,
//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
//  Software, and to permit persons to whom the Software is furnished to do so,
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

//  The MPU-9250 and SPI driver code is based on code generously supplied by
//  staslock@gmail.com (www.clickdrive.io)


#include "RTIMUSettings.h"
#include "RTIMUMPU9150.h"
#include "RTIMUMPU9250.h"
#include "RTIMUGD20HM303D.h"
#include "RTIMUGD20M303DLHC.h"
#include "RTIMULSM9DS0.h"

#define RATE_TIMER_INTERVAL 2

RTIMUSettings::RTIMUSettings(const char *productType)
{
    if ((strlen(productType) > 200) || (strlen(productType) == 0)) {
        HAL_ERROR("Product name too long or null - using default\n");
        strcpy(m_filename, "RTIMULib.ini");
    } else {
        sprintf(m_filename, "%s.ini", productType);
    }
    loadSettings();
}

bool RTIMUSettings::discoverIMU(int& imuType, bool& busIsI2C, unsigned char& slaveAddress)
{
    unsigned char result;
    unsigned char altResult;

    //  auto detect on I2C bus

    m_busMode  = true;
    m_busIsI2C = true;

    if (HALOpen()) {

        if (HALRead(MPU9150_ADDRESS0, MPU9150_WHO_AM_I, 1, &result, "")) {
            if (result == MPU9250_ID) {
                imuType = RTIMU_TYPE_MPU9250;
                slaveAddress = MPU9250_ADDRESS0;
                busIsI2C = true;
                HAL_INFO("Detected MPU9250 at standard address\n");
                return true;
            } else if (result == MPU9150_ID) {
                imuType = RTIMU_TYPE_MPU9150;
                slaveAddress = MPU9150_ADDRESS0;
                busIsI2C = true;
                HAL_INFO("Detected MPU9150 at standard address\n");
                return true;
            }
        }

        if (HALRead(MPU9150_ADDRESS1, MPU9150_WHO_AM_I, 1, &result, "")) {
            if (result == MPU9250_ID) {
                imuType = RTIMU_TYPE_MPU9250;
                slaveAddress = MPU9250_ADDRESS1;
                busIsI2C = true;
                HAL_INFO("Detected MPU9250 at option address\n");
                return true;
            } else if (result == MPU9150_ID) {
                imuType = RTIMU_TYPE_MPU9150;
                slaveAddress = MPU9150_ADDRESS1;
                busIsI2C = true;
                HAL_INFO("Detected MPU9150 at option address\n");
                return true;
            }
        }

        if (HALRead(L3GD20H_ADDRESS0, L3GD20H_WHO_AM_I, 1, &result, "")) {
            if (result == L3GD20H_ID) {
                imuType = RTIMU_TYPE_GD20HM303D;
                slaveAddress = L3GD20H_ADDRESS0;
                busIsI2C = true;
                HAL_INFO("Detected L3GD20H at standard address\n");
                return true;
            } else if (result == LSM9DS0_GYRO_ID) {
                if (HALRead(LSM9DS0_ACCELMAG_ADDRESS0, LSM9DS0_WHO_AM_I, 1, &altResult, "")) {
                    if (altResult == LSM9DS0_ACCELMAG_ID) {
                        imuType = RTIMU_TYPE_LSM9DS0;
                        slaveAddress = LSM9DS0_GYRO_ADDRESS0;
                        busIsI2C = true;
                        HAL_INFO("Detected LSM9DS0 at standard address\n");
                        return true;
                    }
                }
            }
        }

        if (HALRead(L3GD20H_ADDRESS1, L3GD20H_WHO_AM_I, 1, &result, "")) {
            if (result == L3GD20H_ID) {
                imuType = RTIMU_TYPE_GD20HM303D;
                slaveAddress = L3GD20H_ADDRESS1;
                busIsI2C = true;
                HAL_INFO("Detected L3GD20H at option address\n");
                return true;
            } else if (result == LSM9DS0_GYRO_ID) {
                if (HALRead(LSM9DS0_ACCELMAG_ADDRESS1, LSM9DS0_WHO_AM_I, 1, &altResult, "")) {
                    if (altResult == LSM9DS0_ACCELMAG_ID) {
                        imuType = RTIMU_TYPE_LSM9DS0;
                        slaveAddress = LSM9DS0_GYRO_ADDRESS1;
                        busIsI2C = true;
                        HAL_INFO("Detected LSM9DS0 at option address\n");
                        return true;
                    }
                }
            }
        }

        if (HALRead(L3GD20_ADDRESS0, L3GD20_WHO_AM_I, 1, &result, "")) {
            if (result == L3GD20_ID) {
                imuType = RTIMU_TYPE_GD20M303DLHC;
                slaveAddress = L3GD20_ADDRESS0;
                busIsI2C = true;
                HAL_INFO("Detected L3GD20 at standard address\n");
                return true;
            }
        }

        if (HALRead(L3GD20_ADDRESS1, L3GD20_WHO_AM_I, 1, &result, "")) {
            if (result == L3GD20_ID) {
                imuType = RTIMU_TYPE_GD20M303DLHC;
                slaveAddress = L3GD20_ADDRESS1;
                busIsI2C = true;
                HAL_INFO("Detected L3GD20 at option address\n");
                return true;
            }
        }
    }

    //  nothing found on I2C bus - try SPI instead

    m_busIsI2C = false;

    if (HALOpen()) {
        if (HALRead(MPU9150_ADDRESS0, MPU9150_WHO_AM_I, 1, &result, "")) {
            if (result == MPU9250_ID) {
                imuType = RTIMU_TYPE_MPU9250;
                slaveAddress = MPU9250_ADDRESS0;
                busIsI2C = false;
                HAL_INFO("Detected MPU9250 on SPI bus\n");
                return true;
            }
        }
    }

    HAL_ERROR("No IMU detected\n");
    return false;
}

bool RTIMUSettings::loadSettings()
{
    char buf[200];
    char key[200];
    char val[200];
    RTFLOAT ftemp;

    //  preset general defaults

    m_imuType = RTIMU_TYPE_AUTODISCOVER;
    m_I2CSlaveAddress = 0;
    m_busIsI2C = true;
    m_I2CBus = 1;
    m_SPIBus = 0;
    m_SPISpeed = 500000;
    m_fusionType = RTFUSION_TYPE_RTQF;
    m_compassCalValid = false;
    m_compassCalEllipsoidValid = false;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            m_compassCalEllipsoidCorr[i][j] = 0;
        }
    }
    m_compassCalEllipsoidCorr[0][0] = 1;
    m_compassCalEllipsoidCorr[1][1] = 1;
    m_compassCalEllipsoidCorr[2][2] = 1;
    m_accelCalValid = false;
    m_gyroBiasValid = false;

    //  MPU9150 defaults

    m_MPU9150GyroAccelSampleRate = 50;
    m_MPU9150CompassSampleRate = 25;
    m_MPU9150GyroAccelLpf = MPU9150_LPF_20;
    m_MPU9150GyroFsr = MPU9150_GYROFSR_1000;
    m_MPU9150AccelFsr = MPU9150_ACCELFSR_8;

    //  MPU9250 defaults

    m_MPU9250GyroAccelSampleRate = 80;
    m_MPU9250CompassSampleRate = 40;
    m_MPU9250GyroLpf = MPU9250_GYRO_LPF_41;
    m_MPU9250AccelLpf = MPU9250_ACCEL_LPF_41;
    m_MPU9250GyroFsr = MPU9250_GYROFSR_1000;
    m_MPU9250AccelFsr = MPU9250_ACCELFSR_8;

    //  GD20HM303D defaults

    m_GD20HM303DGyroSampleRate = L3GD20H_SAMPLERATE_50;
    m_GD20HM303DGyroBW = L3GD20H_BANDWIDTH_1;
    m_GD20HM303DGyroHpf = L3GD20H_HPF_4;
    m_GD20HM303DGyroFsr = L3GD20H_FSR_500;

    m_GD20HM303DAccelSampleRate = LSM303D_ACCEL_SAMPLERATE_50;
    m_GD20HM303DAccelFsr = LSM303D_ACCEL_FSR_8;
    m_GD20HM303DAccelLpf = LSM303D_ACCEL_LPF_50;

    m_GD20HM303DCompassSampleRate = LSM303D_COMPASS_SAMPLERATE_50;
    m_GD20HM303DCompassFsr = LSM303D_COMPASS_FSR_2;

    //  GD20M303DLHC defaults

    m_GD20M303DLHCGyroSampleRate = L3GD20_SAMPLERATE_95;
    m_GD20M303DLHCGyroBW = L3GD20_BANDWIDTH_1;
    m_GD20M303DLHCGyroHpf = L3GD20_HPF_4;
    m_GD20M303DLHCGyroFsr = L3GD20H_FSR_500;

    m_GD20M303DLHCAccelSampleRate = LSM303DLHC_ACCEL_SAMPLERATE_50;
    m_GD20M303DLHCAccelFsr = LSM303DLHC_ACCEL_FSR_8;

    m_GD20M303DLHCCompassSampleRate = LSM303DLHC_COMPASS_SAMPLERATE_30;
    m_GD20M303DLHCCompassFsr = LSM303DLHC_COMPASS_FSR_1_3;

    //  LSM9DS0 defaults

    m_LSM9DS0GyroSampleRate = LSM9DS0_GYRO_SAMPLERATE_95;
    m_LSM9DS0GyroBW = LSM9DS0_GYRO_BANDWIDTH_1;
    m_LSM9DS0GyroHpf = LSM9DS0_GYRO_HPF_4;
    m_LSM9DS0GyroFsr = LSM9DS0_GYRO_FSR_500;

    m_LSM9DS0AccelSampleRate = LSM9DS0_ACCEL_SAMPLERATE_50;
    m_LSM9DS0AccelFsr = LSM9DS0_ACCEL_FSR_8;
    m_LSM9DS0AccelLpf = LSM9DS0_ACCEL_LPF_50;

    m_LSM9DS0CompassSampleRate = LSM9DS0_COMPASS_SAMPLERATE_50;
    m_LSM9DS0CompassFsr = LSM9DS0_COMPASS_FSR_2;

    //  check to see if settings file exists

    if (!(m_fd = fopen(m_filename, "r"))) {
        HAL_INFO("Settings file not found. Using defaults and creating settings file\n");
        return saveSettings();
    }

    while (fgets(buf, 200, m_fd)) {
        if ((buf[0] == '#') || (buf[0] == ' ') || (buf[0] == '\n'))
            // just a comment
            continue;

        if (sscanf(buf, "%[^=]=%s", key, val) != 2) {
            HAL_ERROR1("Bad line in settings file: %s\n", buf);
            fclose(m_fd);
            return false;
        }

        //  now decode keys

        //  general config

        if (strcmp(key, RTIMULIB_IMU_TYPE) == 0) {
            m_imuType = atoi(val);
        } else if (strcmp(key, RTIMULIB_FUSION_TYPE) == 0) {
            m_fusionType = atoi(val);
        } else if (strcmp(key, RTIMULIB_BUS_MODE) == 0) {
            m_busMode = strcmp(val, "false") == 0;
        } else if (strcmp(key, RTIMULIB_BUS_IS_I2C) == 0) {
            m_busIsI2C = strcmp(val, "true") == 0;
        } else if (strcmp(key, RTIMULIB_I2C_BUS) == 0) {
            m_I2CBus = atoi(val);
        } else if (strcmp(key, RTIMULIB_SPI_BUS) == 0) {
            m_SPIBus = atoi(val);
        } else if (strcmp(key, RTIMULIB_SPI_SPEED) == 0) {
            m_SPISpeed = atoi(val);
        } else if (strcmp(key, RTIMULIB_I2C_SLAVEADDRESS) == 0) {
            m_I2CSlaveAddress = atoi(val);

        // compass calibration

        } else if (strcmp(key, RTIMULIB_COMPASSCAL_VALID) == 0) {
            m_compassCalValid = strcmp(val, "true") == 0;
        } else if (strcmp(key, RTIMULIB_COMPASSCAL_MINX) == 0) {
            sscanf(val, "%f", &ftemp);
            m_compassCalMin.setX(ftemp);
        } else if (strcmp(key, RTIMULIB_COMPASSCAL_MINY) == 0) {
            sscanf(val, "%f", &ftemp);
            m_compassCalMin.setY(ftemp);
        } else if (strcmp(key, RTIMULIB_COMPASSCAL_MINZ) == 0) {
            sscanf(val, "%f", &ftemp);
            m_compassCalMin.setZ(ftemp);
        } else if (strcmp(key, RTIMULIB_COMPASSCAL_MAXX) == 0) {
            sscanf(val, "%f", &ftemp);
            m_compassCalMax.setX(ftemp);
        } else if (strcmp(key, RTIMULIB_COMPASSCAL_MAXY) == 0) {
            sscanf(val, "%f", &ftemp);
            m_compassCalMax.setY(ftemp);
        } else if (strcmp(key, RTIMULIB_COMPASSCAL_MAXZ) == 0) {
            sscanf(val, "%f", &ftemp);
            m_compassCalMax.setZ(ftemp);

            // compass ellipsoid calibration

        } else if (strcmp(key, RTIMULIB_COMPASSCAL_ELLIPSOID_VALID) == 0) {
            m_compassCalEllipsoidValid = strcmp(val, "true") == 0;
        } else if (strcmp(key, RTIMULIB_COMPASSCAL_OFFSET_X) == 0) {
            sscanf(val, "%f", &ftemp);
            m_compassCalEllipsoidOffset.setX(ftemp);
        } else if (strcmp(key, RTIMULIB_COMPASSCAL_OFFSET_Y) == 0) {
            sscanf(val, "%f", &ftemp);
            m_compassCalEllipsoidOffset.setY(ftemp);
        } else if (strcmp(key, RTIMULIB_COMPASSCAL_OFFSET_Z) == 0) {
            sscanf(val, "%f", &ftemp);
            m_compassCalEllipsoidOffset.setZ(ftemp);
        } else if (strcmp(key, RTIMULIB_COMPASSCAL_CORR11) == 0) {
            sscanf(val, "%f", &ftemp);
            m_compassCalEllipsoidCorr[0][0] = ftemp;
        } else if (strcmp(key, RTIMULIB_COMPASSCAL_CORR12) == 0) {
            sscanf(val, "%f", &ftemp);
            m_compassCalEllipsoidCorr[0][1] = ftemp;
        } else if (strcmp(key, RTIMULIB_COMPASSCAL_CORR13) == 0) {
            sscanf(val, "%f", &ftemp);
            m_compassCalEllipsoidCorr[0][2] = ftemp;
        } else if (strcmp(key, RTIMULIB_COMPASSCAL_CORR21) == 0) {
            sscanf(val, "%f", &ftemp);
            m_compassCalEllipsoidCorr[1][0] = ftemp;
        } else if (strcmp(key, RTIMULIB_COMPASSCAL_CORR22) == 0) {
            sscanf(val, "%f", &ftemp);
            m_compassCalEllipsoidCorr[1][1] = ftemp;
        } else if (strcmp(key, RTIMULIB_COMPASSCAL_CORR23) == 0) {
            sscanf(val, "%f", &ftemp);
            m_compassCalEllipsoidCorr[1][2] = ftemp;
        } else if (strcmp(key, RTIMULIB_COMPASSCAL_CORR31) == 0) {
            sscanf(val, "%f", &ftemp);
            m_compassCalEllipsoidCorr[2][0] = ftemp;
        } else if (strcmp(key, RTIMULIB_COMPASSCAL_CORR32) == 0) {
            sscanf(val, "%f", &ftemp);
            m_compassCalEllipsoidCorr[2][1] = ftemp;
        } else if (strcmp(key, RTIMULIB_COMPASSCAL_CORR33) == 0) {
            sscanf(val, "%f", &ftemp);
            m_compassCalEllipsoidCorr[2][2] = ftemp;

            // accel calibration

        } else if (strcmp(key, RTIMULIB_ACCELCAL_VALID) == 0) {
            m_accelCalValid = strcmp(val, "true") == 0;
        } else if (strcmp(key, RTIMULIB_ACCELCAL_MINX) == 0) {
            sscanf(val, "%f", &ftemp);
            m_accelCalMin.setX(ftemp);
        } else if (strcmp(key, RTIMULIB_ACCELCAL_MINY) == 0) {
            sscanf(val, "%f", &ftemp);
            m_accelCalMin.setY(ftemp);
        } else if (strcmp(key, RTIMULIB_ACCELCAL_MINZ) == 0) {
            sscanf(val, "%f", &ftemp);
            m_accelCalMin.setZ(ftemp);
        } else if (strcmp(key, RTIMULIB_ACCELCAL_MAXX) == 0) {
            sscanf(val, "%f", &ftemp);
            m_accelCalMax.setX(ftemp);
        } else if (strcmp(key, RTIMULIB_ACCELCAL_MAXY) == 0) {
            sscanf(val, "%f", &ftemp);
            m_accelCalMax.setY(ftemp);
        } else if (strcmp(key, RTIMULIB_ACCELCAL_MAXZ) == 0) {
            sscanf(val, "%f", &ftemp);
            m_accelCalMax.setZ(ftemp);

            // gyro bias

        } else if (strcmp(key, RTIMULIB_GYRO_BIAS_VALID) == 0) {
            m_gyroBiasValid = strcmp(val, "true") == 0;
        } else if (strcmp(key, RTIMULIB_GYRO_BIAS_X) == 0) {
            sscanf(val, "%f", &ftemp);
            m_gyroBias.setX(ftemp);
        } else if (strcmp(key, RTIMULIB_GYRO_BIAS_Y) == 0) {
            sscanf(val, "%f", &ftemp);
            m_gyroBias.setY(ftemp);
        } else if (strcmp(key, RTIMULIB_GYRO_BIAS_Z) == 0) {
            sscanf(val, "%f", &ftemp);
            m_gyroBias.setZ(ftemp);

        //  MPU9150 settings

        } else if (strcmp(key, RTIMULIB_MPU9150_GYROACCEL_SAMPLERATE) == 0) {
            m_MPU9150GyroAccelSampleRate = atoi(val);
        } else if (strcmp(key, RTIMULIB_MPU9150_COMPASS_SAMPLERATE) == 0) {
            m_MPU9150CompassSampleRate = atoi(val);
        } else if (strcmp(key, RTIMULIB_MPU9150_GYROACCEL_LPF) == 0) {
            m_MPU9150GyroAccelLpf = atoi(val);
        } else if (strcmp(key, RTIMULIB_MPU9150_GYRO_FSR) == 0) {
            m_MPU9150GyroFsr = atoi(val);
        } else if (strcmp(key, RTIMULIB_MPU9150_ACCEL_FSR) == 0) {
            m_MPU9150AccelFsr = atoi(val);

        //  MPU9250 settings

        } else if (strcmp(key, RTIMULIB_MPU9250_GYROACCEL_SAMPLERATE) == 0) {
            m_MPU9250GyroAccelSampleRate = atoi(val);
        } else if (strcmp(key, RTIMULIB_MPU9250_COMPASS_SAMPLERATE) == 0) {
            m_MPU9250CompassSampleRate = atoi(val);
        } else if (strcmp(key, RTIMULIB_MPU9250_GYRO_LPF) == 0) {
            m_MPU9250GyroLpf = atoi(val);
        } else if (strcmp(key, RTIMULIB_MPU9250_ACCEL_LPF) == 0) {
            m_MPU9250AccelLpf = atoi(val);
        } else if (strcmp(key, RTIMULIB_MPU9250_GYRO_FSR) == 0) {
            m_MPU9250GyroFsr = atoi(val);
        } else if (strcmp(key, RTIMULIB_MPU9250_ACCEL_FSR) == 0) {
            m_MPU9250AccelFsr = atoi(val);

        //  GD20HM303D settings

        } else if (strcmp(key, RTIMULIB_GD20HM303D_GYRO_SAMPLERATE) == 0) {
            m_GD20HM303DGyroSampleRate = atoi(val);
        } else if (strcmp(key, RTIMULIB_GD20HM303D_GYRO_FSR) == 0) {
            m_GD20HM303DGyroFsr = atoi(val);
        } else if (strcmp(key, RTIMULIB_GD20HM303D_GYRO_HPF) == 0) {
            m_GD20HM303DGyroHpf = atoi(val);
        } else if (strcmp(key, RTIMULIB_GD20HM303D_GYRO_BW) == 0) {
            m_GD20HM303DGyroBW = atoi(val);
        } else if (strcmp(key, RTIMULIB_GD20HM303D_ACCEL_SAMPLERATE) == 0) {
            m_GD20HM303DAccelSampleRate = atoi(val);
        } else if (strcmp(key, RTIMULIB_GD20HM303D_ACCEL_FSR) == 0) {
            m_GD20HM303DAccelFsr = atoi(val);
        } else if (strcmp(key, RTIMULIB_GD20HM303D_ACCEL_LPF) == 0) {
            m_GD20HM303DAccelLpf = atoi(val);
        } else if (strcmp(key, RTIMULIB_GD20HM303D_COMPASS_SAMPLERATE) == 0) {
            m_GD20HM303DCompassSampleRate = atoi(val);
        } else if (strcmp(key, RTIMULIB_GD20HM303D_COMPASS_FSR) == 0) {
            m_GD20HM303DCompassFsr = atoi(val);

        //  GD20M303DLHC settings

        } else if (strcmp(key, RTIMULIB_GD20M303DLHC_GYRO_SAMPLERATE) == 0) {
            m_GD20M303DLHCGyroSampleRate = atoi(val);
        } else if (strcmp(key, RTIMULIB_GD20M303DLHC_GYRO_FSR) == 0) {
            m_GD20M303DLHCGyroFsr = atoi(val);
        } else if (strcmp(key, RTIMULIB_GD20M303DLHC_GYRO_HPF) == 0) {
            m_GD20M303DLHCGyroHpf = atoi(val);
        } else if (strcmp(key, RTIMULIB_GD20M303DLHC_GYRO_BW) == 0) {
            m_GD20M303DLHCGyroBW = atoi(val);
        } else if (strcmp(key, RTIMULIB_GD20M303DLHC_ACCEL_SAMPLERATE) == 0) {
            m_GD20M303DLHCAccelSampleRate = atoi(val);
        } else if (strcmp(key, RTIMULIB_GD20M303DLHC_ACCEL_FSR) == 0) {
            m_GD20M303DLHCAccelFsr = atoi(val);
        } else if (strcmp(key, RTIMULIB_GD20M303DLHC_COMPASS_SAMPLERATE) == 0) {
            m_GD20M303DLHCCompassSampleRate = atoi(val);
        } else if (strcmp(key, RTIMULIB_GD20M303DLHC_COMPASS_FSR) == 0) {
            m_GD20M303DLHCCompassFsr = atoi(val);

        //  LSM9DS0 settings

        } else if (strcmp(key, RTIMULIB_LSM9DS0_GYRO_SAMPLERATE) == 0) {
            m_LSM9DS0GyroSampleRate = atoi(val);
        } else if (strcmp(key, RTIMULIB_LSM9DS0_GYRO_FSR) == 0) {
            m_LSM9DS0GyroFsr = atoi(val);
        } else if (strcmp(key, RTIMULIB_LSM9DS0_GYRO_HPF) == 0) {
            m_LSM9DS0GyroHpf = atoi(val);
        } else if (strcmp(key, RTIMULIB_LSM9DS0_GYRO_BW) == 0) {
            m_LSM9DS0GyroBW = atoi(val);
        } else if (strcmp(key, RTIMULIB_LSM9DS0_ACCEL_SAMPLERATE) == 0) {
            m_LSM9DS0AccelSampleRate = atoi(val);
        } else if (strcmp(key, RTIMULIB_LSM9DS0_ACCEL_FSR) == 0) {
            m_LSM9DS0AccelFsr = atoi(val);
        } else if (strcmp(key, RTIMULIB_LSM9DS0_ACCEL_LPF) == 0) {
            m_LSM9DS0AccelLpf = atoi(val);
        } else if (strcmp(key, RTIMULIB_LSM9DS0_COMPASS_SAMPLERATE) == 0) {
            m_LSM9DS0CompassSampleRate = atoi(val);
        } else if (strcmp(key, RTIMULIB_LSM9DS0_COMPASS_FSR) == 0) {
            m_LSM9DS0CompassFsr = atoi(val);

        //  Handle unrecognized key

        } else {
            HAL_ERROR1("Unrecognized key in settings file: %s\n", buf);
        }
    }
    HAL_INFO1("Settings file %s loaded\n", m_filename);
    fclose(m_fd);
    return saveSettings();                                  // make sure settings file is correct and complete
}

bool RTIMUSettings::saveSettings()
{
    if (!(m_fd = fopen(m_filename, "w"))) {
        HAL_ERROR("Failed to open settings file for save");
        return false;
    }

    //  General settings

    setComment("#####################################################################");
    setComment("");
    setComment("RTIMULib settings file");
    setBlank();
    setComment("General settings");
    setComment("");

    setBlank();
    setComment("IMU type - ");
    setComment("  0 = Auto discover");
    setComment("  1 = Null (used when data is provided from a remote IMU");
    setComment("  2 = InvenSense MPU-9150");
    setComment("  3 = STM L3GD20H + LSM303D");
    setComment("  4 = STM L3GD20 + LSM303DLHC");
    setComment("  5 = STM LSM9DS0");
    setComment("  6 = InvenSense MPU-9250");
    setValue(RTIMULIB_IMU_TYPE, m_imuType);

    setBlank();
    setComment("");
    setComment("Fusion type type - ");
    setComment("  0 - Null. Use if only sensor data required without fusion");
    setComment("  1 - Kalman STATE4");
    setComment("  2 - RTQF");
    setComment("  3 - Kalman STATE7");
    setValue(RTIMULIB_FUSION_TYPE, m_fusionType);

    setBlank();
    setComment("");
    setComment("Is bus I2C: 'true' for I2C, 'false' for SPI");
    setValue(RTIMULIB_BUS_IS_I2C, m_busIsI2C);

    setBlank();
    setComment("");
    setComment("I2C Bus (between 0 and 7) ");
    setValue(RTIMULIB_I2C_BUS, m_I2CBus);

    setBlank();
    setComment("");
    setComment("SPI Bus (between 0 and 7) ");
    setValue(RTIMULIB_SPI_BUS, m_SPIBus);

    setBlank();
    setComment("");
    setComment("SPI Speed in Hz");
    setValue(RTIMULIB_SPI_SPEED, (int)m_SPISpeed);

    setBlank();
    setComment("");
    setComment("I2C slave address (filled in automatically by auto discover) ");
    setValue(RTIMULIB_I2C_SLAVEADDRESS, m_I2CSlaveAddress);

    //  Compass calibration settings

    setBlank();
    setComment("#####################################################################");
    setComment("");

    setBlank();
    setComment("Compass calibration");
    setValue(RTIMULIB_COMPASSCAL_VALID, m_compassCalValid);
    setValue(RTIMULIB_COMPASSCAL_MINX, m_compassCalMin.x());
    setValue(RTIMULIB_COMPASSCAL_MINY, m_compassCalMin.y());
    setValue(RTIMULIB_COMPASSCAL_MINZ, m_compassCalMin.z());
    setValue(RTIMULIB_COMPASSCAL_MAXX, m_compassCalMax.x());
    setValue(RTIMULIB_COMPASSCAL_MAXY, m_compassCalMax.y());
    setValue(RTIMULIB_COMPASSCAL_MAXZ, m_compassCalMax.z());

    //  Compass ellipsoid calibration settings

    setBlank();
    setComment("#####################################################################");
    setComment("");

    setBlank();
    setComment("Compass ellipsoid calibration");
    setValue(RTIMULIB_COMPASSCAL_ELLIPSOID_VALID, m_compassCalEllipsoidValid);
    setValue(RTIMULIB_COMPASSCAL_OFFSET_X, m_compassCalEllipsoidOffset.x());
    setValue(RTIMULIB_COMPASSCAL_OFFSET_Y, m_compassCalEllipsoidOffset.y());
    setValue(RTIMULIB_COMPASSCAL_OFFSET_Z, m_compassCalEllipsoidOffset.z());
    setValue(RTIMULIB_COMPASSCAL_CORR11, m_compassCalEllipsoidCorr[0][0]);
    setValue(RTIMULIB_COMPASSCAL_CORR12, m_compassCalEllipsoidCorr[0][1]);
    setValue(RTIMULIB_COMPASSCAL_CORR13, m_compassCalEllipsoidCorr[0][2]);
    setValue(RTIMULIB_COMPASSCAL_CORR21, m_compassCalEllipsoidCorr[1][0]);
    setValue(RTIMULIB_COMPASSCAL_CORR22, m_compassCalEllipsoidCorr[1][1]);
    setValue(RTIMULIB_COMPASSCAL_CORR23, m_compassCalEllipsoidCorr[1][2]);
    setValue(RTIMULIB_COMPASSCAL_CORR31, m_compassCalEllipsoidCorr[2][0]);
    setValue(RTIMULIB_COMPASSCAL_CORR32, m_compassCalEllipsoidCorr[2][1]);
    setValue(RTIMULIB_COMPASSCAL_CORR33, m_compassCalEllipsoidCorr[2][2]);

    //  Accel calibration settings

    setBlank();
    setComment("#####################################################################");
    setComment("");

    setBlank();
    setComment("Accel calibration");
    setValue(RTIMULIB_ACCELCAL_VALID, m_accelCalValid);
    setValue(RTIMULIB_ACCELCAL_MINX, m_accelCalMin.x());
    setValue(RTIMULIB_ACCELCAL_MINY, m_accelCalMin.y());
    setValue(RTIMULIB_ACCELCAL_MINZ, m_accelCalMin.z());
    setValue(RTIMULIB_ACCELCAL_MAXX, m_accelCalMax.x());
    setValue(RTIMULIB_ACCELCAL_MAXY, m_accelCalMax.y());
    setValue(RTIMULIB_ACCELCAL_MAXZ, m_accelCalMax.z());

    //  Gyro bias settings

    setBlank();
    setComment("#####################################################################");
    setComment("");

    setBlank();
    setComment("Saved gyro bias data");
    setValue(RTIMULIB_GYRO_BIAS_VALID, m_gyroBiasValid);
    setValue(RTIMULIB_GYRO_BIAS_X, m_gyroBias.x());
    setValue(RTIMULIB_GYRO_BIAS_Y, m_gyroBias.y());
    setValue(RTIMULIB_GYRO_BIAS_Z, m_gyroBias.z());

    //  MPU-9150 settings

    setBlank();
    setComment("#####################################################################");
    setComment("");
    setComment("MPU-9150 settings");
    setComment("");

    setBlank();
    setComment("Gyro sample rate (between 5Hz and 1000Hz) ");
    setValue(RTIMULIB_MPU9150_GYROACCEL_SAMPLERATE, m_MPU9150GyroAccelSampleRate);

    setBlank();
    setComment("");
    setComment("Compass sample rate (between 1Hz and 100Hz) ");
    setValue(RTIMULIB_MPU9150_COMPASS_SAMPLERATE, m_MPU9150CompassSampleRate);

    setBlank();
    setComment("");
    setComment("Gyro/accel low pass filter - ");
    setComment("  0 - gyro: 256Hz, accel: 260Hz");
    setComment("  1 - gyro: 188Hz, accel: 184Hz");
    setComment("  2 - gyro: 98Hz, accel: 98Hz");
    setComment("  3 - gyro: 42Hz, accel: 44Hz");
    setComment("  4 - gyro: 20Hz, accel: 21Hz");
    setComment("  5 - gyro: 10Hz, accel: 10Hz");
    setComment("  6 - gyro: 5Hz, accel: 5Hz");
    setValue(RTIMULIB_MPU9150_GYROACCEL_LPF, m_MPU9150GyroAccelLpf);

    setBlank();
    setComment("");
    setComment("Gyro full scale range - ");
    setComment("  0  - +/- 250 degress per second");
    setComment("  8  - +/- 500 degress per second");
    setComment("  16 - +/- 1000 degress per second");
    setComment("  24 - +/- 2000 degress per second");
    setValue(RTIMULIB_MPU9150_GYRO_FSR, m_MPU9150GyroFsr);

    setBlank();
    setComment("");
    setComment("Accel full scale range - ");
    setComment("  0  - +/- 2g");
    setComment("  8  - +/- 4g");
    setComment("  16 - +/- 8g");
    setComment("  24 - +/- 16g");
    setValue(RTIMULIB_MPU9150_ACCEL_FSR, m_MPU9150AccelFsr);

    //  MPU-9250 settings

    setBlank();
    setComment("#####################################################################");
    setComment("");
    setComment("MPU-9250 settings");
    setComment("");

    setBlank();
    setComment("Gyro sample rate (between 5Hz and 1000Hz plus 8000Hz and 32000Hz) ");
    setValue(RTIMULIB_MPU9250_GYROACCEL_SAMPLERATE, m_MPU9250GyroAccelSampleRate);

    setBlank();
    setComment("");
    setComment("Compass sample rate (between 1Hz and 100Hz) ");
    setValue(RTIMULIB_MPU9250_COMPASS_SAMPLERATE, m_MPU9250CompassSampleRate);

    setBlank();
    setComment("");
    setComment("Gyro low pass filter - ");
    setComment("  0x11 - 8800Hz, 0.64mS delay");
    setComment("  0x10 - 3600Hz, 0.11mS delay");
    setComment("  0x00 - 250Hz, 0.97mS delay");
    setComment("  0x01 - 184Hz, 2.9mS delay");
    setComment("  0x02 - 92Hz, 3.9mS delay");
    setComment("  0x03 - 41Hz, 5.9mS delay");
    setComment("  0x04 - 20Hz, 9.9mS delay");
    setComment("  0x05 - 10Hz, 17.85mS delay");
    setComment("  0x06 - 5Hz, 33.48mS delay");
    setValue(RTIMULIB_MPU9250_GYRO_LPF, m_MPU9250GyroLpf);

    setBlank();
    setComment("");
    setComment("Accel low pass filter - ");
    setComment("  0x08 - 1130Hz, 0.75mS delay");
    setComment("  0x00 - 460Hz, 1.94mS delay");
    setComment("  0x01 - 184Hz, 5.80mS delay");
    setComment("  0x02 - 92Hz, 7.80mS delay");
    setComment("  0x03 - 41Hz, 11.80mS delay");
    setComment("  0x04 - 20Hz, 19.80mS delay");
    setComment("  0x05 - 10Hz, 35.70mS delay");
    setComment("  0x06 - 5Hz, 66.96mS delay");
    setValue(RTIMULIB_MPU9250_ACCEL_LPF, m_MPU9250AccelLpf);

    setBlank();
    setComment("");
    setComment("Gyro full scale range - ");
    setComment("  0  - +/- 250 degress per second");
    setComment("  8  - +/- 500 degress per second");
    setComment("  16 - +/- 1000 degress per second");
    setComment("  24 - +/- 2000 degress per second");
    setValue(RTIMULIB_MPU9250_GYRO_FSR, m_MPU9250GyroFsr);

    setBlank();
    setComment("");
    setComment("Accel full scale range - ");
    setComment("  0  - +/- 2g");
    setComment("  8  - +/- 4g");
    setComment("  16 - +/- 8g");
    setComment("  24 - +/- 16g");
    setValue(RTIMULIB_MPU9250_ACCEL_FSR, m_MPU9250AccelFsr);

    //  GD20HM303D settings

    setBlank();
    setComment("#####################################################################");
    setComment("");
    setComment("L3GD20H + LSM303D settings");

    setBlank();
    setComment("");
    setComment("Gyro sample rate - ");
    setComment("  0 = 12.5Hz ");
    setComment("  1 = 25Hz ");
    setComment("  2 = 50Hz ");
    setComment("  3 = 100Hz ");
    setComment("  4 = 200Hz ");
    setComment("  5 = 400Hz ");
    setComment("  6 = 800Hz ");
    setValue(RTIMULIB_GD20HM303D_GYRO_SAMPLERATE, m_GD20HM303DGyroSampleRate);

    setBlank();
    setComment("");
    setComment("Gyro full scale range - ");
    setComment("  0 = 245 degrees per second ");
    setComment("  1 = 500 degrees per second ");
    setComment("  2 = 2000 degrees per second ");
    setValue(RTIMULIB_GD20HM303D_GYRO_FSR, m_GD20HM303DGyroFsr);

    setBlank();
    setComment("");
    setComment("Gyro high pass filter - ");
    setComment("  0 - 9 but see the L3GD20H manual for details");
    setValue(RTIMULIB_GD20HM303D_GYRO_HPF, m_GD20HM303DGyroHpf);

    setBlank();
    setComment("");
    setComment("Gyro bandwidth - ");
    setComment("  0 - 3 but see the L3GD20H manual for details");
    setValue(RTIMULIB_GD20HM303D_GYRO_BW, m_GD20HM303DGyroBW);

    setBlank();
    setComment("Accel sample rate - ");
    setComment("  1 = 3.125Hz ");
    setComment("  2 = 6.25Hz ");
    setComment("  3 = 12.5Hz ");
    setComment("  4 = 25Hz ");
    setComment("  5 = 50Hz ");
    setComment("  6 = 100Hz ");
    setComment("  7 = 200Hz ");
    setComment("  8 = 400Hz ");
    setComment("  9 = 800Hz ");
    setComment("  10 = 1600Hz ");
    setValue(RTIMULIB_GD20HM303D_ACCEL_SAMPLERATE, m_GD20HM303DAccelSampleRate);

    setBlank();
    setComment("");
    setComment("Accel full scale range - ");
    setComment("  0 = +/- 2g ");
    setComment("  1 = +/- 4g ");
    setComment("  2 = +/- 6g ");
    setComment("  3 = +/- 8g ");
    setComment("  4 = +/- 16g ");
    setValue(RTIMULIB_GD20HM303D_ACCEL_FSR, m_GD20HM303DAccelFsr);

    setBlank();
    setComment("");
    setComment("Accel low pass filter - ");
    setComment("  0 = 773Hz");
    setComment("  1 = 194Hz");
    setComment("  2 = 362Hz");
    setComment("  3 = 50Hz");
    setValue(RTIMULIB_GD20HM303D_ACCEL_LPF, m_GD20HM303DAccelLpf);

    setBlank();
    setComment("");
    setComment("Compass sample rate - ");
    setComment("  0 = 3.125Hz ");
    setComment("  1 = 6.25Hz ");
    setComment("  2 = 12.5Hz ");
    setComment("  3 = 25Hz ");
    setComment("  4 = 50Hz ");
    setComment("  5 = 100Hz ");
    setValue(RTIMULIB_GD20HM303D_COMPASS_SAMPLERATE, m_GD20HM303DCompassSampleRate);


    setBlank();
    setComment("");
    setComment("Compass full scale range - ");
    setComment("  0 = +/- 200 uT ");
    setComment("  1 = +/- 400 uT ");
    setComment("  2 = +/- 800 uT ");
    setComment("  3 = +/- 1200 uT ");
    setValue(RTIMULIB_GD20HM303D_COMPASS_FSR, m_GD20HM303DCompassFsr);

    //  GD20M303DLHC settings

    setBlank();
    setComment("#####################################################################");
    setComment("");
    setComment("L3GD20 + LSM303DLHC settings");
    setComment("");

    setBlank();
    setComment("Gyro sample rate - ");
    setComment("  0 = 95z ");
    setComment("  1 = 190Hz ");
    setComment("  2 = 380Hz ");
    setComment("  3 = 760Hz ");
    setValue(RTIMULIB_GD20M303DLHC_GYRO_SAMPLERATE, m_GD20M303DLHCGyroSampleRate);

    setBlank();
    setComment("");
    setComment("Gyro full scale range - ");
    setComment("  0 = 250 degrees per second ");
    setComment("  1 = 500 degrees per second ");
    setComment("  2 = 2000 degrees per second ");
    setValue(RTIMULIB_GD20M303DLHC_GYRO_FSR, m_GD20M303DLHCGyroFsr);

    setBlank();
    setComment("");
    setComment("Gyro high pass filter - ");
    setComment("  0 - 9 but see the L3GD20 manual for details");
    setValue(RTIMULIB_GD20M303DLHC_GYRO_HPF, m_GD20M303DLHCGyroHpf);

    setBlank();
    setComment("");
    setComment("Gyro bandwidth - ");
    setComment("  0 - 3 but see the L3GD20 manual for details");
    setValue(RTIMULIB_GD20M303DLHC_GYRO_BW, m_GD20M303DLHCGyroBW);

    setBlank();
    setComment("Accel sample rate - ");
    setComment("  1 = 1Hz ");
    setComment("  2 = 10Hz ");
    setComment("  3 = 25Hz ");
    setComment("  4 = 50Hz ");
    setComment("  5 = 100Hz ");
    setComment("  6 = 200Hz ");
    setComment("  7 = 400Hz ");
    setValue(RTIMULIB_GD20M303DLHC_ACCEL_SAMPLERATE, m_GD20M303DLHCAccelSampleRate);

    setBlank();
    setComment("");
    setComment("Accel full scale range - ");
    setComment("  0 = +/- 2g ");
    setComment("  1 = +/- 4g ");
    setComment("  2 = +/- 8g ");
    setComment("  3 = +/- 16g ");
    setValue(RTIMULIB_GD20M303DLHC_ACCEL_FSR, m_GD20M303DLHCAccelFsr);

    setBlank();
    setComment("");
    setComment("Compass sample rate - ");
    setComment("  0 = 0.75Hz ");
    setComment("  1 = 1.5Hz ");
    setComment("  2 = 3Hz ");
    setComment("  3 = 7.5Hz ");
    setComment("  4 = 15Hz ");
    setComment("  5 = 30Hz ");
    setComment("  6 = 75Hz ");
    setComment("  7 = 220Hz ");
    setValue(RTIMULIB_GD20M303DLHC_COMPASS_SAMPLERATE, m_GD20M303DLHCCompassSampleRate);


    setBlank();
    setComment("");
    setComment("Compass full scale range - ");
    setComment("  1 = +/- 130 uT ");
    setComment("  2 = +/- 190 uT ");
    setComment("  3 = +/- 250 uT ");
    setComment("  4 = +/- 400 uT ");
    setComment("  5 = +/- 470 uT ");
    setComment("  6 = +/- 560 uT ");
    setComment("  7 = +/- 810 uT ");
    setValue(RTIMULIB_GD20M303DLHC_COMPASS_FSR, m_GD20M303DLHCCompassFsr);

    //  LSM9DS0 settings

    setBlank();
    setComment("#####################################################################");
    setComment("");
    setComment("LSM9DS0 settings");
    setComment("");

    setBlank();
    setComment("Gyro sample rate - ");
    setComment("  0 = 95z ");
    setComment("  1 = 190Hz ");
    setComment("  2 = 380Hz ");
    setComment("  3 = 760Hz ");
    setValue(RTIMULIB_LSM9DS0_GYRO_SAMPLERATE, m_LSM9DS0GyroSampleRate);

    setBlank();
    setComment("");
    setComment("Gyro full scale range - ");
    setComment("  0 = 250 degrees per second ");
    setComment("  1 = 500 degrees per second ");
    setComment("  2 = 2000 degrees per second ");
    setValue(RTIMULIB_LSM9DS0_GYRO_FSR, m_LSM9DS0GyroFsr);

    setBlank();
    setComment("");
    setComment("Gyro high pass filter - ");
    setComment("  0 - 9 but see the LSM9DS0 manual for details");
    setValue(RTIMULIB_LSM9DS0_GYRO_HPF, m_LSM9DS0GyroHpf);

    setBlank();
    setComment("");
    setComment("Gyro bandwidth - ");
    setComment("  0 - 3 but see the LSM9DS0 manual for details");
    setValue(RTIMULIB_LSM9DS0_GYRO_BW, m_LSM9DS0GyroBW);

    setBlank();
    setComment("Accel sample rate - ");
    setComment("  1 = 3.125Hz ");
    setComment("  2 = 6.25Hz ");
    setComment("  3 = 12.5Hz ");
    setComment("  4 = 25Hz ");
    setComment("  5 = 50Hz ");
    setComment("  6 = 100Hz ");
    setComment("  7 = 200Hz ");
    setComment("  8 = 400Hz ");
    setComment("  9 = 800Hz ");
    setComment("  10 = 1600Hz ");
    setValue(RTIMULIB_LSM9DS0_ACCEL_SAMPLERATE, m_LSM9DS0AccelSampleRate);

    setBlank();
    setComment("");
    setComment("Accel full scale range - ");
    setComment("  0 = +/- 2g ");
    setComment("  1 = +/- 4g ");
    setComment("  2 = +/- 6g ");
    setComment("  3 = +/- 8g ");
    setComment("  4 = +/- 16g ");
    setValue(RTIMULIB_LSM9DS0_ACCEL_FSR, m_LSM9DS0AccelFsr);

    setBlank();
    setComment("");
    setComment("Accel low pass filter - ");
    setComment("  0 = 773Hz");
    setComment("  1 = 194Hz");
    setComment("  2 = 362Hz");
    setComment("  3 = 50Hz");
    setValue(RTIMULIB_LSM9DS0_ACCEL_LPF, m_LSM9DS0AccelLpf);

    setBlank();
    setComment("");
    setComment("Compass sample rate - ");
    setComment("  0 = 3.125Hz ");
    setComment("  1 = 6.25Hz ");
    setComment("  2 = 12.5Hz ");
    setComment("  3 = 25Hz ");
    setComment("  4 = 50Hz ");
    setComment("  5 = 100Hz ");
    setValue(RTIMULIB_LSM9DS0_COMPASS_SAMPLERATE, m_LSM9DS0CompassSampleRate);


    setBlank();
    setComment("");
    setComment("Compass full scale range - ");
    setComment("  0 = +/- 200 uT ");
    setComment("  1 = +/- 400 uT ");
    setComment("  2 = +/- 800 uT ");
    setComment("  3 = +/- 1200 uT ");
    setValue(RTIMULIB_LSM9DS0_COMPASS_FSR, m_LSM9DS0CompassFsr);
    fclose(m_fd);
    return true;
}

void RTIMUSettings::setBlank()
{
    fprintf(m_fd, "\n");
}

void RTIMUSettings::setComment(const char *comment)
{
    fprintf(m_fd, "# %s\n", comment);
}

void RTIMUSettings::setValue(const char *key, const bool val)
{
    fprintf(m_fd, "%s=%s\n", key, val ? "true" : "false");
}

void RTIMUSettings::setValue(const char *key, const int val)
{
    fprintf(m_fd, "%s=%d\n", key, val);
}

void RTIMUSettings::setValue(const char *key, const RTFLOAT val)
{
    fprintf(m_fd, "%s=%f\n", key, val);
}


