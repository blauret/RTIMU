/*
 * imuread.cpp
 *
 *  Created on: Nov 19, 2014
 *      Author: blauret
 */


#include "RTIMU.h"

#include <iostream>
using namespace std;

int main() {
	RTIMUSettings *m_settings = new RTIMUSettings("IMURead.ini");

	 RTIMU::createIMU(m_settings);
	 cout << "IMU Created" << endl;

	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!



	return 0;
}

