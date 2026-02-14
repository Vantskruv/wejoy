-- Overview

-- System thread ----------------------------------------------------------------
-- function system_thread(milliseconds)
-- The function system_thread acts like a system loop in its own thread. It is
-- set to update every millisecond, but it can be longer than that.
-- An elapsed time value in milliseconds since the last external call is given.
-- Here can you make more advanced calculations.

-- Note that global variables declared outside any function will not be shared with
-- the system_thread function. The only way to share variables between the
-- system_thread function and the event functions (or custom functions) is to
-- declare the variables in the global_variables table, which is read and written
-- to with these functions:
-- set_global_value("my_global_variable")
-- get_global_value("my_global_variable")

-- The global_values table
-- These variables can be shared between the system_thread function and all the
-- other functions using the above methods.
-- The variables in the table can only be numbers, and has to be declared in
-- the global_values table.
-- Example:
-- global_variables = {
--   my_first_global_variable = 0,
--   my_second_global_variable = 0,
-- }
--
--
-- Device events ------------------------------------------------------------------
-- To recieve device events from devices, a table needs to be created in the script.
-- It should look something like this:
-- devices = {
-- "/dev/input/by-id/usb-wraper@inbox.lv_HE_Sequential_Shifter_060228-event-joystick",
-- "/dev/input/by-id/usb-Thustmaster_Joystick_-_HOTAS_Warthog-event-joystick",
-- "/dev/input/by-id/usb-DuckyChannel_International_Co.__Ltd._Ducky_Keyboard-event-kbd",
-- "/dev/input/by-id/usb-Logitech_G502_HERO_Gaming_Mouse_0F84396C3833-event-mouse",
-- }

-- Just add the physical devices you want to use. To list the available devices
-- on your system, either add the option --list-devices when running wejoy,
-- or view the /dev/input/by-id folder.
--
-- There are three type of events depending on what events a device has.
-- When an event occurs, a function for that device is externally called.
-- Do not do any time intensive computations in this function, as it would stall
-- other event calls. It may look like this:
--  function d2_event(type, code, value)
-- The the 'd2' in this example states which device is calling the function, which
-- is the device you added in order in the devices table, in this case, device 2,
-- which is the third device in the devices table.
--
-- 'type' is the event type:
-- EV_KEY for joystick buttons, keyboard keys, or mouse buttons.
-- EV_ABS for absoulate axes
-- EV_REL for relative axes (i.e. mices may normally use relative axes)
--
-- 'code' is what key, button, absolute axis or relative axis has triggered
-- the event. I.e. it may be KEY_A, BTN_TRIGGER, ABS_X, REL_Y and other codes.
-- To know what codes the device is supporting you can
-- use the --show-device option with wejoy from the terminal.
--
-- 'value' is the state of key or button press, or the axis position, or the
-- relative movement on a relative axis.
--
--
-- Function calls for virtual devices -------------------------------------------
-- send_vjoy_button_event(vjoy_index, button, button_state)
-- send_vjoy_abs_event(vjoy_index, axis, axis_position)
-- send_vjoy_rel_event(vjoy_index, rel, acceleration)
-- Send values to your created virtual devices
-- 'vjoy_index' is the (in order) created virtual joystick
-- 'axis', 'rel' and 'button' is which index to set the data on (i.e 0, 1, 2 and so on)
-- 'button_state' is 0 for release, 1 for press
-- 'axis_position' is the position of the axis (-32767 to 32767)
-- 'acceleration' is the position moved the last time
--
-- send_vkeyboard_key_event(key, state)
-- Send key pressed to the virtual keyboard
-- 'key' is the key to press or release, i.e. KEY_A
-- 'state' is 0 for release and 1 for press
--
-- get_vjoy_button_status(vjoy_index, button)
-- get_vjoy_abs_status(vjoy_index, axis)
-- get_vjoy_rel_status(vjoy_index, rel)
-- Get the current value of the virtual joystick
-- 'vjoy_index' is the (in order) created virtual joystick
-- 'button' which button to read
-- 'axis' and 'rel' which axis to read

-- Function calls for physical devices ------------------------------------------
-- get_joy_button_status(device_index, code)
-- get_joy_abs_status(device_index, code)
-- get_joy_rel_axis_status(device_index, code)
-- Get the current value from a physical device
-- 'device_index' is the index of the device you added in the order of creation.
-- 'code' what to return from the device, i.e. BTN_A, ABS_X, KEY_A, REL_Y etc.

--
-- Keyboard and mouse events ----------------------------------------------------
-- Reading keyboard and mouse events is exactly the same thing as reading events
-- from joysticks.
-- Them same applies for sending keyboard events. Just create a virtual device
-- and add the keys you want to use in your virtual device table.
-- However, I noticed it can be problems when sending keyboard key events if
-- the virtual device also has ABS axes (hence, key presses are not detected).
-- Test and try, if it works for thy, happy fly!

--Physical devices to use
devices = {
	"/dev/input/by-id/usb-wraper@inbox.lv_HE_Sequential_Shifter_060228-event-joystick",
	"/dev/input/by-id/usb-Thustmaster_Joystick_-_HOTAS_Warthog-event-joystick",
	"/dev/input/by-id/usb-DuckyChannel_International_Co.__Ltd._Ducky_Keyboard-event-kbd",
	"/dev/input/by-id/usb-Logitech_G502_HERO_Gaming_Mouse_0F84396C3833-event-mouse",
}

-- Global values to use between the system_thread and functions
global_values = {
	brake_apply_release = 0,
	virtual_brake_data = 0,
	virtual_steering_data = 0,
}

--Virtual devices to create
virtualjoys = {
	{ -- A small virtual keyboard with a name
		name = "Wejoy Virtual Keyboard",
		buttons = {
			KEY_A,
			KEY_S,
			KEY_D,
			KEY_W,
			KEY_Q,
			KEY_E,
		},
	},
	{ -- A virtual joystick with two buttons and two axis with some settings
		buttons = {
			BTN_TRIGGER,
			BTN_THUMB,
		},
		abs = {
			{ code = ABS_X, min = -32767, max = 32767, fuzz = 0, flat = 0, resolution = 0 },
			{ code = ABS_Y, min = -32767, max = 32767, fuzz = 0, flat = 0, resoultuon = 0 },
		},
	},
	{ -- I hope this is right for emulating a Xbox One controller
		name = "Xbox One Wireless Controller",
		vendorid = 0x045e,
		productid = 0x028e,
		buttons = {
			BTN_SOUTH,
			BTN_EAST,
			BTN_NORTH,
			BTN_WEST,
			BTN_TL,
			BTN_TR,
			BTN_TL2,
			BTN_TR2,
			BTN_START,
			BTN_SELECT,
			BTN_THUMBL,
			BTN_THUMBR,
		},
		abs = {
			{ code = ABS_X },
			{ code = ABS_Y },
			{ code = ABS_Z },
			{ code = ABS_RX },
			{ code = ABS_RY },
			{ code = ABS_RZ },
			{ code = ABS_HAT0X },
			{ code = ABS_HAT0Y },
		},
	},
}

function clamp(value, min, max)
	if value < min then
		value = min
	elseif value > max then
		value = max
	end
	return value
end

-- Example 1A
-- Control a virtual joystick axis with global variables from a device in the system_thread
function system_thread(milliseconds)
	brake_apply_release = get_global_value("brake_apply_release")
	if brake_apply_release == 0 then
		return
	end

	virtual_brake_data = get_global_value("virtual_brake_data")
	virtual_brake_data = virtual_brake_data + milliseconds * 10 * brake_apply_release
	virtual_brake_data = clamp(virtual_brake_data, -32767, 32767)
	send_vjoy_abs_event(1, ABS_Y, virtual_brake_data)
	set_global_value("virtual_brake_data", virtual_brake_data)
end

-- Example 1B
-- Control a virtual joystick with buttons on a physical device
-- using system_thread and global variables
function d0_event(type, code, value)
	if type == EV_KEY then
		if code == BTN_TRIGGER then
			if value > 0 then
				set_global_value("brake_apply_release", 1)
			else
				set_global_value("brake_apply_release", 0)
			end
		elseif code == BTN_THUMB then
			if value > 0 then
				set_global_value("brake_apply_release", -1)
			else
				set_global_value("brake_apply_release", 0)
			end
		end
	end
end

-- Example 2
-- Example of using a mouse with relative axes controlling a virtual joystick
function d3_event(type, code, value)
	if type == EV_REL then
		if code == REL_X then
			if value ~= 0 then
				mx_relative = get_joy_rel_status(3, REL_X)
				--print("MOUSE: " .. mx_relative)
				virtual_steering_data = get_global_value("virtual_steering_data")
				virtual_steering_data = clamp(virtual_steering_data + value * 100, -32767, 32767)
				set_global_value("virtual_steering_data", virtual_steering_data)
				send_vjoy_abs_event(1, ABS_X, virtual_steering_data)
			end
		end
	end
end

-- Example 3
-- Read keyboard events
function d2_event(type, code, value)
	if type == EV_KEY then
		if code == KEY_A then
			--print("KEY_A: " .. value)
		end
	end
end

-- Example 4
-- When the Y-axis is moved on a joystick, print out both the X and Y values
-- Also press and release key W when BTN_TRIGGER is pressed and released
function d1_event(type, code, value)
	if type == EV_KEY then
		if code == BTN_TRIGGER then
			if value > 0 then
				send_vjoy_button_event(0, KEY_W, 1)
			else
				send_vjoy_button_event(0, KEY_W, 0)
			end
		end
	elseif type == EV_ABS then
		if code == ABS_Y then
			x_value = get_joy_abs_status(1, ABS_X)
			--print("ABS_XY (" .. x_value .. ", " .. value .. ")")
		end
	end
end
