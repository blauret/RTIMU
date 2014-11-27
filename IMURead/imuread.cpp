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
	RTIMUSettings *m_settings = new RTIMUSettings("IMURead");

	RTIMU *myIMU = RTIMU::createIMU(m_settings);
	cout << "IMU Created" << endl;

	if(myIMU->IMUInit()) {

        cout << "IMU initialized" << endl;


	} else {

        cout << "IMU intialisation failed" << endl;
	}


	cout << "Done" << endl; // prints !!!Hello World!!!



	return 0;
}

