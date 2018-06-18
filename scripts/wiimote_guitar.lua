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
    --     d2 = {
    --         name = "Nintendo Wii Remote Guitar",
    --         wiimote = 1
    --     },
    --     d3 = {
    --         name = "Nintendo Wii Remote Accelerometer",
    --         wiimote = 1
    --     },
    --     d4 = {
    --         name = "Nintendo Wii Remote Guitar",
    --         wiimote = 2
    --     },
    --     d5 = {
    --         name = "Nintendo Wii Remote Accelerometer",
    --         wiimote = 2
    --     }
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
    -- v1 =
    -- {
    --     buttons = 9,
    --     axes = 6
    -- },
    -- v2 =
    -- {
    --     buttons = 9,
    --     axes = 6
    -- }
}

function scale(x, in_min, in_max, out_min, out_max)
    return math.floor((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
end

function main()
    for name, dev in pairs(devices) do
        dev.min = {};
        dev.max = {};
        for i = 0, get_axis_count(name) - 1 do
            dev.min[i + 1] = get_axis_min(name, i);
            dev.max[i + 1] = get_axis_max(name, i);
        end
    end
end
function startsWith(String,Start)
    return string.sub(String,1,string.len(Start))==Start
end
function axis_event(device, axis, value)
    if startsWith(device,"guitar") then
        local vDev = "v"..device;
        send_axis_event(vDev, axis, scale(value, devices[device].min[axis + 1], devices[device].max[axis + 1], MIN_ABS_VAL, MAX_ABS_VAL))
    end
    if startsWith(device,"accel") then
        local vDev = "vguitar"..string.sub(device,6);
        send_axis_event(vDev, axis + 3, scale(value, devices[device].min[axis + 1], devices[device].max[axis + 1], MIN_ABS_VAL, MAX_ABS_VAL))
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
end