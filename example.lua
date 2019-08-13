--Physical devices to use (type lsusb in terminal to list your connected devices)
devices = 
{
	d0 = --Thrustmaster Warthog Joystick
	{
		vendorid = 0x044f,
		productid = 0x0402,
	},

	d1 = --Thrustmaster Warthog Throttle
	{
		vendorid = 0x044f,
		productid = 0x0404,
	}
	
	kbd0 = "/dev/input/by-id/usb-04d9_USB_Keyboard-event-kbd"  -- keyboard device (try to find a suitable device by listing input devices by typing 'ls /dev/input/by-id/' )
	kbd1 = "/dev/input/by-id/usb-Aqua_Computer_GmbH___Co._KG_aquaero_07538-20376-event-kbd" -- another keyboard device example
}

--Virtual devices to create, current limit is maximum 53 (0 to 52) buttons and 19 (0 to 18) axes. Note that not every button or axis is fully tested to work.
--Creating more than one virtual devices is possible, making room for more buttons and axes.
v_devices = 
{
	v0 = 
	{
		buttons = 3,
		axes = 2
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

function kbd0_pressed(value)
	if value == KEY_W then
		send_button_event(0, 1, 1)
	end
end

function kbd0_released(value)
	if value == KEY_W then
		send_button_event(0, 1, 0)
	end
end


-- Send keyboard key 'a' when button 0 on device 1 is pressed, and release the key when button is released.
function d1_b0_event(value)
	if value == 1 then
		send_keyboard_event(KEY_A, 1)
	else
		send_keyboard_event(KEY_A, 0)
	end
end


-- Send keyboard key 'LEFTSHIFT + a' (A) when button 1 on device 1 is pressed, and release the keys when button is released.
function d1_b1_event(value)
	if value == 1 then
		send_keyboard_event(KEY_LEFTSHIFT, 1)
		send_keyboard_event(KEY_A, 1)
	else
		send_keyboard_event(KEY_LEFTSHIFT, 0)
		send_keyboard_event(KEY_A, 0)
	end
end


-- Send a button 0 event to virtual device 0 when button 0 on physical device 0 is pressed and released.
function d0_b0_event(value)
	if value == 1 then
		send_button_event(0, 0, 1)
	else
		send_button_event(0, 0, 0)
	end
end

-- When button 1 on device 0 is pressed, invert virtual axes 0 and 1 on virtual device 0, otherwise these axes is as the first two axes on physical device 0.
-- Send a button 1 event to virtual device 0 when button 1 on physical device 0 is pressed and released.
function d0_b1_event(value)
	if value == 1 then
		send_button_event(0, 1, 1)
		x = get_axis_status(0, 0)
		y = get_axis_status(0, 1)
		send_axis_event(0, 0, -x)
		send_axis_event(0, 1, -y)
	else
		send_button_event(0, 1, 0)
		x = get_axis_status(0, 0)
		y = get_axis_status(0, 1)
		send_axis_event(0, 0, x)
		send_axis_event(0, 1, y)
	end
end

-- Send a button 2 event to virtual device 0 when button 2 on physical device 0 is pressed and released.
function d0_b2_event(value)
	if value == 1 then
		send_button_event(0, 2, 1)
	else
		send_button_event(0, 2, 0)
	end
end

-- When axis 0 on device 0 is changed, set the axis 0 on virtual device 0, inverted or not depending on button 1 on device 0.
function d0_a0_event(value)
	d0_b1 = get_button_status(0, 1)
	if d0_b1 == 1 then	send_axis_event(0, 0, -value)
	else send_axis_event(0, 0, value)
	end
end

-- When axis 1 on device 0 is changed, set the axis 1 on virtual device 0, inverted or not depending on button 1 on device 0.
function d0_a1_event(value)
	d0_b1 = get_button_status(0, 1)
	if d0_b1 == 1 then	send_axis_event(0, 1, -value)
	else send_axis_event(0, 1, value)
	end
end


