--Physical devices to use pool
devices = {
	--HE Sequential Shifter
	{
		vendorid = 0xa020,
		productid = 0x3142,
	},
	{ --Thrustmaster Joystick
		vendorid = 0x044f,
		productid = 0x0402,
	},
	{ -- Keyboard
		vendorid = 0x04d9,
		productid = 0x0348,
	},
	{ --Mouse
		vendorid = 0x046d,
		productid = 0xc08b,
	},
}

--Virtual devices to create, current limit is maximum 53 (0 to 52) buttons and 19 (0 to 18) axes. Note that not every button or axis is fully tested to work.
v_devices = {
	v0 = {
		buttons = 2,
		axes = 2,
	},
}

global_values = {
	brake_apply_release = 0,
	virtual_brake_axis = 0,
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
-- Control a virtual joystick axis with global variables from a device
function system_thread(milliseconds)
	brake_apply_release = get_global_value("brake_apply_release")
	if brake_apply_release == 0 then
		return
	end

	virtual_brake_axis = get_global_value("virtual_brake_axis")
	virtual_brake_axis = virtual_brake_axis + milliseconds * 10 * brake_apply_release
	virtual_brake_axis = clamp(virtual_brake_axis, -32767, 32767)
	send_vjoy_axis_event(0, 0, virtual_brake_axis)
	set_global_value("virtual_brake_axis", virtual_brake_axis)
end

-- Example 1B
-- Control a virtual joystick with a device with buttons using
-- system_thread and global variables
function device_a020_3142_event(type, code, value)
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
function _device_046d_c08b_event(type, code, value)
	if type == EV_REL then
		if code == REL_X then
			if value ~= 0 then
				mx_relative = get_joy_relative_axis_status(REL_X, 0x046d, 0xc08b, 1)
				print("MOUSE: " .. mx_relative)
				virtual_brake_axis = get_global_value("virtual_brake_axis")
				virtual_brake_axis = clamp(virtual_brake_axis + value * 100, -32767, 32767)
				set_global_value("virtual_brake_axis", virtual_brake_axis)
				send_vjoy_axis_event(0, 0, virtual_brake_axis)
			end
		end
	end
end

-- Example 3
-- A new way to use a keyboard, the old way still works as a backup though.
function device_04d9_0348_event(type, code, value)
	if type == EV_KEY then
		if code == KEY_A then
			print("KEY_A: " .. value)
		end
	end
end

-- Example 4
-- When a the Y-axis is moved on a joystick, print out both the X and Y values
function device_044f_0402_event(type, code, value)
	if type == EV_ABS then
		if code == ABS_Y then
			x_value = get_joy_axis_status(ABS_X, 0x44f, 0x0402, 0)
			print("ABS_XY (" .. x_value .. ", " .. value .. ")")
		end
	end
end
