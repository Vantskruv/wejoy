-- ##########
-- IL2 Sturmovik Battle of Stalingrad does not recognize the sliders axis of the throtle and pedals.
-- This config creates a virtual joystick with those 2 axis.
-- It also makes the button 2(1) of the throtle a modifier, to use it as the LEFT ALT key.
-- ##########

--Physical devices to use (type lsusb in terminal to list your connected devices)
devices = 
   {
      d0 = --Thrustmaster Throttle
	 {
	    vendorid = 0x044f,
	    productid = 0xb687,
	 }
   }

--Virtual devices to create, current limit is maximum 53 (0 to 52) buttons and 19 (0 to 18) axes. Note that not every button or axis is fully tested to work.
--Creating more than one virtual devices is possible, making room for more buttons and axes.
v_devices = 
   {
      v0 = 
	 {
	    buttons = 1,
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

-- When axis 1 on device 0 is changed, set the axis 1 on virtual device 0, inverted or not depending on button 1 on device 0.
function d0_a6_event(value)
	send_axis_event(0, 0, value)
end
function d0_a7_event(value)
	send_axis_event(0, 1, value)
end
-- Send a button 0 event to virtual device 0 when button 0 on physical device 0 is pressed and released.
function d0_b1_event(value)
	if value == 1 then
	   send_keyboard_event(KEY_LEFTALT, 1)
	else
	   send_keyboard_event(KEY_LEFTALT, 0)
	end
end