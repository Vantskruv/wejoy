--[[
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS-IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
--]]

--Physical devices to use (type lsusb in terminal to list your connected devices)
devices = 
{
	d0 = --Thrustmaster Joystick
	{
		vendorid = 0x044f,
		productid = 0xb10a,
	},

	d1 = --Thrustmaster Throttle
	{
		vendorid = 0x044f,
		productid = 0xb687,
	}
}

--Virtual devices to create, current limit is maximum 54 (0 to 53) buttons and 19 (0 to 18) axes. Note that not every button or axis is fully tested to work.
--Creating more than one virtual devices is possible, making room for more buttons and axes.
v_devices = 
{
	v0 = 
	{
		buttons = 28,
		axes = 17
	}
}

-- Send method for keyboard. Key is given, i.e. KEY_G (check reference document for more supported key-codes), and state is given, i.e. 0 for release or 1 for press.
-- send_keyboard_event(key, state)

-- Send methods for virtual devices. Index of virtual device is given, i.e. 0 for v0, 1 for v1 and so on. Button/axis is given, i.e. 0 for button 0, 1 for button 1 and so on.
-- send_button_event(vjoy, button, state)
-- send_axis_event(vjoy, axis, pos)			//pos is the axis position (-32767 >= pos <= 32767)

-- Get methods for physical devices. Arguments applies in the same way as for above methods.
-- get_button_status(joy, button) 	//Returns 0 or 1 if button is pressed or not.
-- get_axis_status(joy, axis)		//Returns the axis position value

-- Get methods for virtual devices, applies in the same way as for physical devices.
-- get_vjoy_button_status(vjoy, button)
-- get_vjoy_axis_status(vjoy, axis)

-- MAP all buttons to new buttons and do the same with axes
function d0_b0_event(value)
	if value == 1 then
		send_button_event(0, 0, 1)
	else
		send_button_event(0, 0, 0)
	end
end
function d0_b1_event(value)
	if value == 1 then
		send_button_event(0, 1, 1)
	else
		send_button_event(0, 1, 0)
	end
end
function d0_b2_event(value)
	if value == 1 then
		send_button_event(0, 2, 1)
	else
		send_button_event(0, 2, 0)
	end
end
function d0_b3_event(value)
	if value == 1 then
		send_button_event(0, 3, 1)
	else
		send_button_event(0, 3, 0)
	end
end
function d0_b4_event(value)
	if value == 1 then
		send_button_event(0, 4, 1)
	else
		send_button_event(0, 4, 0)
	end
end
function d0_b5_event(value)
	if value == 1 then
		send_button_event(0, 5, 1)
	else
		send_button_event(0, 5, 0)
	end
end
function d0_b6_event(value)
	if value == 1 then
		send_button_event(0, 6, 1)
	else
		send_button_event(0, 6, 0)
	end
end
function d0_b7_event(value)
	if value == 1 then
		send_button_event(0, 7, 1)
	else
		send_button_event(0, 7, 0)
	end
end
function d0_b8_event(value)
	if value == 1 then
		send_button_event(0, 8, 1)
	else
		send_button_event(0, 8, 0)
	end
end
function d0_b9_event(value)
	if value == 1 then
		send_button_event(0, 9, 1)
	else
		send_button_event(0, 9, 0)
	end
end
function d0_b10_event(value)
	if value == 1 then
		send_button_event(0, 10, 1)
	else
		send_button_event(0, 10, 0)
	end
end
function d0_b11_event(value)
	if value == 1 then
		send_button_event(0, 11, 1)
	else
		send_button_event(0, 11, 0)
	end
end
function d0_b12_event(value)
	if value == 1 then
		send_button_event(0, 12, 1)
	else
		send_button_event(0, 12, 0)
	end
end
function d0_b13_event(value)
	if value == 1 then
		send_button_event(0, 13, 1)
	else
		send_button_event(0, 13, 0)
	end
end
function d0_b14_event(value)
	if value == 1 then
		send_button_event(0, 14, 1)
	else
		send_button_event(0, 14, 0)
	end
end
function d0_b15_event(value)
	if value == 1 then
		send_button_event(0, 15, 1)
	else
		send_button_event(0, 15, 0)
	end
end
function d1_b0_event(value)
	if value == 1 then
		send_button_event(0, 16, 1)
	else
		send_button_event(0, 16, 0)
	end
end
function d1_b1_event(value)
	if value == 1 then
		send_button_event(0, 17, 1)
	else
		send_button_event(0, 17, 0)
	end
end
function d1_b2_event(value)
	if value == 1 then
		send_button_event(0, 18, 1)
	else
		send_button_event(0, 18, 0)
	end
end
function d1_b3_event(value)
	if value == 1 then
		send_button_event(0, 19, 1)
	else
		send_button_event(0, 19, 0)
	end
end
function d1_b4_event(value)
	if value == 1 then
		send_button_event(0, 20, 1)
	else
		send_button_event(0, 20, 0)
	end
end
function d1_b5_event(value)
	if value == 1 then
		send_button_event(0, 21, 1)
	else
		send_button_event(0, 21, 0)
	end
end
function d1_b6_event(value)
	if value == 1 then
		send_button_event(0, 22, 1)
	else
		send_button_event(0, 22, 0)
	end
end
function d1_b7_event(value)
	if value == 1 then
		send_button_event(0, 23, 1)
	else
		send_button_event(0, 23, 0)
	end
end
function d1_b8_event(value)
	if value == 1 then
		send_button_event(0, 24, 1)
	else
		send_button_event(0, 24, 0)
	end
end
function d1_b9_event(value)
	if value == 1 then
		send_button_event(0, 25, 1)
	else
		send_button_event(0, 25, 0)
	end
end
function d1_b10_event(value)
	if value == 1 then
		send_button_event(0, 26, 1)
	else
		send_button_event(0, 26, 0)
	end
end
function d1_b11_event(value)
	if value == 1 then
		send_button_event(0, 27, 1)
	else
		send_button_event(0, 27, 0)
	end
end
function d1_b12_event(value)
	if value == 1 then
		send_button_event(0, 28, 1)
	else
		send_button_event(0, 28, 0)
	end
end
function d1_b13_event(value)
	if value == 1 then
		send_button_event(0, 29, 1)
	else
		send_button_event(0, 29, 0)
	end
end
-- X
function d0_a0_event(value)
	send_axis_event(0, 0, value)
end
-- Y
function d0_a1_event(value)
	send_axis_event(0, 1, value)
end
-- Z
function d1_a2_event(value)
	send_axis_event(0, 2, value)
end
-- rX
function d0_a2_event(value)
	send_axis_event(0, 3, value)
end
-- rY
function d0_a3_event(value)
	send_axis_event(0, 8, value)
end
-- hat 0x
function d0_a4_event(value)
	send_axis_event(0, 11, value)
end
-- hat 0y
function d0_a5_event(value)
	send_axis_event(0, 12, value)
end
-- index x 
function d1_a0_event(value)
	send_axis_event(0, 10, value)
end
-- index y
function d1_a1_event(value)
	send_axis_event(0, 7, value)
end
-- 
function d1_a3_event(value)
	send_axis_event(0, 9, value)
end
function d1_a4_event(value)
	send_axis_event(0, 10, value)
end

function d1_a5_event(value)
	send_axis_event(0, 11, value)
end
-- plug in stuff? throttle
function d1_a6_event(value)
	send_axis_event(0, 6, value)
end
-- trim
function d1_a7_event(value)
	send_axis_event(0, 5, value)
end
-- hat1x
function d1_a8_event(value)
	send_axis_event(0, 13, value)
end
-- hat1y
function d1_a9_event(value)
	send_axis_event(0, 14, value)
end
