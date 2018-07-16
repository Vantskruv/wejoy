-- This example file creates virtual button presses when the two-way switches goes to the off state, and when the 3-way switches
-- goes to the middle state.

--Physical devices to use
devices =
{
    d0 = --Thrustmaster Warthog Throttle
    {
        vendorid = 0x044f,
        productid = 0x0404,
    }
}

--Virtual devices to create, current limit is maximum 54 (0 to 53) buttons and 19 (0 to 18) axes. Note that not every button or axis is fully tested to work.
v_devices =
{
    v0 =
    {
        buttons = 13,
        axes = 0
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

function d0_b15_event(value) -- ENG_L
    if value == 1 then send_button_event(0, 0, 0)
    else send_button_event(0, 0, 1)
    end
end

function d0_b16_event(value) -- FLOW_R
    if value == 1 then send_button_event(0, 1, 0)
    else send_button_event(0, 1, 1)
    end
end

function d0_b17_event(value) -- MOTOR_L
    if value == 1 then send_button_event(0, 2, 0)
    else send_button_event(0, 2, 1)
    end
end

function d0_b30_event(value) -- IGN_L
    if value == 1 then send_button_event(0, 2, 0)
    else send_button_event(0, 2, 1)
    end
end

function d0_b18_event(value) -- MOTOR_R
    if value == 1 then send_button_event(0, 3, 0)
    else send_button_event(0, 3, 1)
    end
end

function d0_b31_event(value) -- IGN_R
    if value == 1 then send_button_event(0, 3, 0)
    else send_button_event(0, 3, 1)
    end
end

function d0_b19_event(value) -- APU
    if value == 1 then send_button_event(0, 4, 0)
    else send_button_event(0, 4, 1)
    end
end

function d0_b23_event(value) -- EAC
    if value == 1 then send_button_event(0, 5, 0)
    else send_button_event(0, 5, 1)
    end
end

function d0_b24_event(value) -- RDR_ALTM
    if value == 1 then send_button_event(0, 6, 0)
    else send_button_event(0, 6, 1)
    end
end

function d0_b26_event(value) -- PATH
    if value == 1 then send_button_event(0, 7, 0)
    else send_button_event(0, 7, 1)
    end
end

function d0_b27_event(value) -- ALT
    if value == 1 then send_button_event(0, 7, 0)
    else send_button_event(0, 7, 1)
    end
end

function d0_b21_event(value) --Flaps UP
    if value == 1 then send_button_event(0, 8, 0);
    else send_button_event(0, 8, 1);
    end
end

function d0_b22_event(value) --Flaps down
    if value == 1 then send_button_event(0, 8, 0);
    else send_button_event(0, 8, 1);
    end
end

function d0_b12_event(value) -- LIGHTS_FORWARD
    if value == 1 then send_button_event(0, 9, 0)
    else send_button_event(0, 9, 1)
    end
end

function d0_b13_event(value) -- LIGHTS_BACKWARD
    if value == 1 then send_button_event(0, 9, 0)
    else send_button_event(0, 9, 1)
    end
end

function d0_b6_event(value) -- AIR_BRAKE_FORWARD
    if value == 1 then send_button_event(0, 10, 0)
    else send_button_event(0, 10, 1)
    end
end

function d0_b7_event(value) -- AIR_BRAKE_BACKWARDS
    if value == 1 then send_button_event(0, 10, 0)
    else send_button_event(0, 10, 1)
    end
end

function d0_b8_event(value) -- ROCKER_FORWARD
    if value == 1 then send_button_event(0, 11, 0)
    else send_button_event(0, 11, 1)
    end
end

function d0_b9_event(value) -- ROCKER_BACKWARD
    if value == 1 then send_button_event(0, 11, 0)
    else send_button_event(0, 11, 1)
    end
end

function d0_b10_event(value) -- BOAT_FORWARD
    if value == 1 then send_button_event(0, 12, 0)
    else send_button_event(0, 12, 1)
    end
end

function d0_b11_event(value) -- BOAT_BACKWARD
    if value == 1 then send_button_event(0, 12, 0)
    else send_button_event(0, 12, 1)
    end
end

