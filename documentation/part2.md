# Architecture Improvement

## Proposal
### Migration from an old joystick api
The current architecture of WeJoy uses the "joystick" API. As a result, it has classes for manually wrapping the joystick API, and requires the use of the "udev" api for querying information about the connected joystick, such as a vendor or product id.

A better solution would be to use the "event" api with a wrapper like "libevdev". This allows us to leave the implementation of the event API up to libevdev, and we only need to ask for events using data structures it has already defined for us. This also allows us to remove the use of udev, as the event API exposes this informaton for us.
Another benefit is this removes the requirement for controllers to be connected via usb, and opens up both bluetooth and even virtual devices.


### Testing framework
The current architecture of WeJoy has 0 tests. I have written a testing library that creates a virtual joystick, and then runs WeJoy with an example script configured for the joysticks. I then feed data into the virtual joystick and use libevdev to check that the virtual joysticks and keyboard created by WeJoy are outputting the correct data.

### New build system
WeJoy was using a shell script to build. This is not very flexible, so I have replaced it with a CMake script instead.

# Alterations
### Removal of udev querying
WeJoy previously used udev to query for information, and just assumed that the device was USB. As libevdev provided this information, I have removed this code and replaced it with calls to libevdev.

### Removal of the event structure
WeJoy was previously using its own event structure in order to manually read data from the old joystick api. This is no longer needed, as the event api provides this type of data for us. As a result, the code for handling events is much simpler as it has access to the required data when called.

### Added functions for recieving many events at once
While testing, I needed a way to identify what joystick button was pressed. To do this, I added some methods to the lua script that would let you write general handlers for all axis or all button events.

## Added an importer for the events from the Linux kernel instead of hardcoding basic events
WeJoy had a hardcoded list of events that it could listen to. I thought this was a bad idea as it means that it will need to be changed if a device is ever added to the linux kernel.
Instead, I found and modified a script to allow me to generate a list of events, and have changed the code to reference this list.

## Added functions to get the max and min values of a specific axis
While testing, I found it difficult to map from a real device to a virtual one as they both used different max and min values. This meant that one joystick may have been at the max value, and the other joystick may not even have registered a change. I added the ability to retrive this information so a user could write a simple script in lua to map from one coordinate sytem to another. I also added constants for the coordinate system used the the virtual joystick.


# Evaluation

## Stakeholder Concerns