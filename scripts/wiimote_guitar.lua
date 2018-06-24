--Physical devices to use (type lsusb in terminal to list your connected devices)
devices =
{
    guitar0 = {
        name = "Nintendo Wii Remote Guitar",
        wiimote = 0
    },
    accel0 = {
        name = "Nintendo Wii Remote Accelerometer",
        wiimote = 0
    },
    guitar1 = {
        name = "Nintendo Wii Remote Guitar",
        wiimote = 1
    },
    accel1 = {
        name = "Nintendo Wii Remote Accelerometer",
        wiimote = 1
    },
    guitar2 = {
        name = "Nintendo Wii Remote Guitar",
        wiimote = 2
    },
    accel2 = {
        name = "Nintendo Wii Remote Accelerometer",
        wiimote = 2
    },
    drums0 = {
        name = "Nintendo Wii Remote Drums",
        wiimote = 3
    },
}

--Virtual devices to create, current limit is maximum 54 (0 to 53) buttons and 19 (0 to 18) axes. Note that not every button or axis is fully tested to work.
--Creating more than one virtual devices is possible, making room for more buttons and axes.
v_devices =
{
    vguitar0 =
    {
        buttons = 9,
        axes = 6
    },
    vguitar1 =
    {
        buttons = 9,
        axes = 6
    },
    vguitar2 =
    {
        buttons = 9,
        axes = 6
    },
    vguitar3 =
    {
        buttons = 9,
        axes = 6
    },
    vdrums0 =
    {
        buttons = 9,
        axes = 6
    }
}
function startsWith(String,Start)
    return string.sub(String,1,string.len(Start))==Start
end
function axis_event_scaled(device, axis, value)
    if startsWith(device,"guitar") then
        local vDev = "v"..device;
        send_axis_event(vDev, axis, value)
    end
    if startsWith(device,"accel") then
        local vDev = "vguitar"..string.sub(device,6);
        send_axis_event(vDev, axis + 3, value)
    end
    if startsWith(device,"drums") then
        local vDev = "v"..device;
        if axis < 2 then
            send_axis_event(vDev, axis, value)
        else
            if value > -32767 then
                send_button_event(vDev, axis, 1)
            else
                send_button_event(vDev, axis, 0)
            end
        end
    end
end

function button_event(device, button, value)
    if startsWith(device,"guitar") then
        local vDev = "v"..device;
        if button >= 2 then
            send_button_event(vDev, button - 2, value)
        else
            send_button_event(vDev, button + 7, value)
        end
    end
    if startsWith(device,"drums") then
        local vDev = "v"..device;
        send_button_event(vDev, button, value)
    end
end