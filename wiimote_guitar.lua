--Physical devices to use (type lsusb in terminal to list your connected devices)
devices =
{
    d0 = {
        name = "Nintendo Wii Remote Guitar",
        index = 0
    },
    d1 = {
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

function d0_b548_event(value)
    send_button_event(0, 0, value)
end

function d0_b549_event(value)
    send_button_event(0, 1, value)
end

function d0_b550_event(value)
    send_button_event(0, 2, value)
end

function d0_b551_event(value)
    send_button_event(0, 3, value)
end

function d0_b552_event(value)
    send_button_event(0, 4, value)
end

function d0_b553_event(value)
    send_button_event(0, 5, value)
end

function d0_b554_event(value)
    send_button_event(0, 6, value)
end

function d0_b315_event(value)
    send_button_event(0, 7, value)
end

function d0_b316_event(value)
    send_button_event(0, 8, value)
end

function d0_a0_event(value)
    send_axis_event(0, 0, value * 100)
end

function d0_a1_event(value)
    send_axis_event(0, 1, value * 100)
end

function d0_a62_event(value)
    send_axis_event(0, 2, value * 1000)
end

function d1_a3_event(value)
    send_axis_event(0, 3, value * 100)
end

function d1_a4_event(value)
    send_axis_event(0, 4, value * 100)
end

function d1_a5_event(value)
    send_axis_event(0, 5, value * 100)
end