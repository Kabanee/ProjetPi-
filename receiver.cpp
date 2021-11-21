/*

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

//#include <Windows.h>
#include "config.h"
#include "mathHelp.h"
#include "receiver.h"
#include <iostream>
#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>

receiver::receiver()
{
	this->roll_pwm = RECEIVER_ROLL_KEY_PWM;
	this->pitch_pwm	= RECEIVER_PITCH_KEY_PWM;
	this->yaw_pwm = RECEIVER_YAW_KEY_PWM;
	this->throttle_pwm = RECEIVER_THROTTLE_KEY_PWM;

	this->output_blocked = false;
}

receiver::~receiver()
{
}



void receiver::get_desired_theta(Vector3d &theta_d)
{
	// zero signals in case no input or blocked
	theta_d(0) = RECEIVER_PWM_ZERO_SIGNAL;
	theta_d(1) = RECEIVER_PWM_ZERO_SIGNAL;
	theta_d(2) = RECEIVER_PWM_ZERO_SIGNAL;

	// only roll and pitch
	if(this->output_blocked == false)
	{

		// roll (A and D)
		if(keypressed(0x51)) //0x41 pour A donc 0x51 pour Q
			theta_d(0) -= this->roll_pwm;
		else if(keypressed(0x44))
			theta_d(0) += this->roll_pwm;

		// pitch (W and S)
		if(keypressed(0x5A))//0x57 pour W donc 0x5A pour Z
			theta_d(1) += this->pitch_pwm;
		else if(keypressed(0x53))
			theta_d(1) -= this->pitch_pwm;

		// yaw (Q and E)
		if(keypressed(0x41))//0x51 pour Q donc 0x41 pour A
			theta_d(2) -= this->yaw_pwm;
		else if(keypressed(0x45))
			theta_d(2) += this->yaw_pwm;

		/*sf::Joystick::Identification id = sf::Joystick::getIdentification(0);
		sf::Joystick::update();
		if (sf::Joystick::getAxisPosition(0, sf::Joystick::U) > 0.1)//0.0015259
		{
			theta_d(0) += this->roll_pwm;
		}
		else if (sf::Joystick::getAxisPosition(0, sf::Joystick::U) < -0.1)//0.0015259
		{
			theta_d(0) -= this->roll_pwm;
		}

		if (sf::Joystick::getAxisPosition(0, sf::Joystick::Y) > 0.1)//0.0015259
		{
			theta_d(1) += this->pitch_pwm;
		}
		else if (sf::Joystick::getAxisPosition(0, sf::Joystick::Y) < -0.1)//0.0015259
		{
			theta_d(1) -= this->pitch_pwm;
		}

		if (sf::Joystick::getAxisPosition(0, sf::Joystick::X) > 0.1)//0.0015259
		{
			theta_d(2) += this->yaw_pwm;
		}
		else if (sf::Joystick::getAxisPosition(0, sf::Joystick::X) < -0.1)//0.0015259
		{
			theta_d(2) -= this->yaw_pwm;
		}
		*/
	}

	theta_d(0) = constrainn<double>(theta_d(0), RECEIVER_PWM_MIN, RECEIVER_PWM_MAX);
	theta_d(1) = constrainn<double>(theta_d(1), RECEIVER_PWM_MIN, RECEIVER_PWM_MAX);
	theta_d(2) = constrainn<double>(theta_d(2), RECEIVER_PWM_MIN, RECEIVER_PWM_MAX);
}

void receiver::get_desired_throttle(double &throttle)
{	
	// user has to put stick in hover position if he does not want to climb/sink
	throttle = getPWMinPointOfEquilibirum();

	// if block demand normal position
	if(this->output_blocked == false)
	{
		// climb (+)
		if(keypressed(0x57))//VK_OEM_PLUS pour la touche "+" mais changé pour W : 0x57
			throttle += this->throttle_pwm;
		// sink (-)
		else if(keypressed(0x58))//VK_OEM_MINUS pour la touche "-" mais changé pour X : 0x58
			throttle -= this->throttle_pwm;

		sf::Joystick::update();
		if (sf::Joystick::getAxisPosition(0, sf::Joystick::Z) > 0.1)//0.0015259
		{
			throttle += this->throttle_pwm;
		}
		else if (sf::Joystick::getAxisPosition(0, sf::Joystick::Z) < -0.1)//0.0015259
		{
			throttle -= this->throttle_pwm;
		}
	}

	throttle = constrainn<double>(throttle, RECEIVER_PWM_MIN, RECEIVER_PWM_MAX);
}

void receiver::block_receiver(bool blocked)
{
	this->output_blocked = blocked;
}

bool receiver::keypressed(int keyvalue)
{
	SHORT tabKeyState = GetAsyncKeyState(keyvalue);

	// test high bit - if set, key was down when GetAsyncKeyState was called
	if(( 1 << 16 ) & tabKeyState)
		return true;

	return false;
}