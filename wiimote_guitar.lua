--Physical devices to use (type lsusb in terminal to list your connected devices)
devices =
{
    guitar = {
        name = "Nintendo Wii Remote Guitar",
        index = 0
    },
    accel = {
        name = "Nintendo Wii Remote Accelerometer",
        index = 0
    }
}

--Virtual devices to create, current limit is maximum 54 (0 to 53) buttons and 19 (0 to 18) axes. Note that not every button or axis is fully tested to work.
--Creating more than one virtual devices is possible, making room for more buttons and axes.
v_devices =
{
    v0 =
    {
        buttons = 9,
        axes = 6
    }
}

function axis_event(device, axis, value)
    if device == "guitar" then
        send_axis_event(0, axis, value * 100)
    end
    if device == "accel" then
        send_axis_event(0, axis+3, value * 100)
    end
end

function button_event(device, button, value)
    if device == "guitar" then
        if button >= 2 then
            send_button_event(0, button-2, value)
        else
            send_button_event(0, button+7, value)
        end
    end
end