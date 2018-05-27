#!/usr/bin/env python
# Parses linux/input.h scanning for #define KEY_FOO 134
# Prints C header files or Python files that can be used as
# mapping and lookup tables.
#

import re
import sys


class Bits(object):
    pass


prefixes = [
    "EV_",
    "REL_",
    "ABS_",
    "KEY_",
    "BTN_",
    "LED_",
    "SND_",
    "MSC_",
    "SW_",
    "FF_",
    "SYN_",
    "REP_",
    "INPUT_PROP_",
]

blacklist = [
    "EV_VERSION",
    "BTN_MISC",
    "BTN_MOUSE",
    "BTN_JOYSTICK",
    "BTN_GAMEPAD",
    "BTN_DIGI",
    "BTN_WHEEL",
    "BTN_TRIGGER_HAPPY",
    "SW_MAX",
    "REP_MAX",
]

btn_additional = [
    [0, "BTN_A"],
    [0, "BTN_B"],
    [0, "BTN_X"],
    [0, "BTN_Y"],
]

names = [
    "REL_",
    "ABS_",
    "KEY_",
    "BTN_",
    "LED_",
    "SND_",
    "MSC_",
    "SW_",
    "FF_",
    "SYN_",
    "REP_",
]


def print_bits(bits, prefix):
    if not hasattr(bits, prefix):
        return
    print("static std::map<int, const char*> %s_map= {" % prefix)
    for val, name in list(getattr(bits, prefix).items()):
        print("	{%s,\"%s\"}," % (name, name))
    if prefix == "key":
        for val, name in list(getattr(bits, "btn").items()):
            print("\t{%s,\"%s\"}," % (name, name))
    print("};")
    print("")


def print_map(bits):
    print("static std::map<int, std::map<int, const char*>> event_type_map = {")

    for prefix in prefixes:
        if prefix == "BTN_" or prefix == "EV_" or prefix == "INPUT_PROP_":
            continue
        print("\t{EV_%s,%s_map}," % (prefix[:-1], prefix[:-1].lower()))

    print("};")
    print("")
    print("static std::map<int, const int> ev_max = {")
    for prefix in prefixes:
        if prefix == "BTN_" or prefix == "EV_" or prefix == "INPUT_PROP_":
            continue
        print("\t{EV_%s,%s_MAX}," % (prefix[:-1], prefix[:-1]))
    print("};")


def print_lookup(bits, prefix):
    if not hasattr(bits, prefix):
        return

    names = list(getattr(bits, prefix).items())
    if prefix == "btn":
        names = names + btn_additional;

    for val, name in sorted(names, key=lambda e: e[1]):
        print("\t{ .name = \"%s\", .value = %s }," % (name, name))


def print_lookup_table(bits):
    print("struct name_entry {")
    print("	const char *name;")
    print("	unsigned int value;")
    print("};")
    print("")
    print("static const struct name_entry ev_names[] = {")
    print_lookup(bits, "ev")
    print("};")
    print("")

    print("static const struct name_entry code_names[] = {")
    for prefix in sorted(names, key=lambda e: e):
        print_lookup(bits, prefix[:-1].lower())
    print("};")
    print("")
    print("static const struct name_entry prop_names[] = {")
    print_lookup(bits, "input_prop")
    print("};")
    print("")


def print_mapping_table(bits):
    print("/* THIS FILE IS GENERATED, DO NOT EDIT */")
    print("")
    print("#ifndef EVENT_NAMES_H")
    print("#define EVENT_NAMES_H")
    print("")
    print("#include <linux/uinput.h>")
    print("#include <map>")

    for prefix in prefixes:
        if prefix == "BTN_":
            continue
        print_bits(bits, prefix[:-1].lower())

    print_map(bits)
    print_lookup_table(bits)

    print("#endif /* EVENT_NAMES_H */")


def parse_define(bits, line):
    m = re.match(r"^#define\s+(\w+)\s+(\w+)", line)
    if m is None:
        return

    name = m.group(1)

    if name in blacklist:
        return

    try:
        value = int(m.group(2), 0)
    except ValueError:
        return

    for prefix in prefixes:
        if not name.startswith(prefix):
            continue

        attrname = prefix[:-1].lower()

        if not hasattr(bits, attrname):
            setattr(bits, attrname, {})
        b = getattr(bits, attrname)
        b[value] = name


def parse(fp):
    bits = Bits()

    lines = fp.readlines()
    for line in lines:
        if not line.startswith("#define"):
            continue
        parse_define(bits, line)

    return bits


def usage(prog):
    print("Usage: cat <files> | %s" % prog)


if __name__ == "__main__":
    if len(sys.argv) != 1:
        usage(sys.argv[0])
        sys.exit(2)

    bits = parse(sys.stdin)
    print_mapping_table(bits)
