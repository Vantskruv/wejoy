# WeJoy

# How to clone
- `git clone git@gitlab.ecs.vuw.ac.nz:swen301-2018/govindsanj/SWEN301-Project-2.git`

# How to build and run tests
- Install a copy of vagrant (this is already provided on university machines)
- run `./build_and_test.sh`

# Original Code:
the original_code folder contains the original code, modified to disable the hard requirement on
USB devices. the tests have also been copied over, as the original code required much longer timings
so the tests were incompatible.

# How do scripts work
Check an example script such as `scripts/wiimote_guitar.lua` for new features, 
and `scripts/example.lua` for the original syntax.

# Testing the original code
A modified version of the original code is provided that has no architectural changes, but instead 
has modified udev code so it does not have a reliance on USB anymore. This means all the new tests can run - however,
 it appears the original code has issues with checking if a button is down after pushing it,
and the timings have all been changed as the original code base was slower to update.