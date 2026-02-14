// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>

#include "joystick.h"

#include <libevdev-1.0/libevdev/libevdev.h>
#include <fcntl.h>
#include <iostream>
#include "unistd.h"
#include <cstring>
#include <filesystem>


Joystick::Joystick(const std::string& _device_path)
{
  open_joy(_device_path);
}

bool Joystick::open_joy(const std::string& _device_path)
{
  close_joy();
  device_path = _device_path;
  vendorid = -1;
  productid = -1;

  int fd = open(device_path.c_str(), O_RDONLY | O_NONBLOCK);
  int rc = libevdev_new_from_fd(fd, &evdev);
  if(rc < 0)
  {
    close(fd);
    std::cerr << "Failed to open joystick: " << std::strerror(-rc) << std::endl;
    return false;
  }

  vendorid = libevdev_get_id_vendor(evdev);
  productid = libevdev_get_id_product(evdev);
  device_name = libevdev_get_name(evdev);

  for(size_t i=KEY_RESERVED; i<KEY_CNT; i++) if(libevdev_has_event_code(evdev, EV_KEY, i)) number_of_buttons++;
  for(size_t i=ABS_X; i<ABS_CNT; i++) if(libevdev_has_event_code(evdev, EV_ABS, i)) number_of_abs++;
  for(size_t i=REL_X; i<REL_CNT; i++) if(libevdev_has_event_code(evdev, EV_REL, i)) number_of_rels++;

  return true;
}

bool Joystick::is_open()
{
  return evdev!=nullptr;
}

void Joystick::close_joy()
{
  if(evdev)
  {
    libevdev_free(evdev);
    evdev = nullptr;
  }
}

int Joystick::get_button_status(int type)
{
  return button_data[type];
}

int Joystick::get_abs_status(int _i)
{
  return abs_data[_i];
}

int Joystick::get_rel_status(int _i)
{
  return rel_data[_i];
}

bool Joystick::read_joy(JoystickEvent* event)
{
  if(libevdev_next_event(evdev, LIBEVDEV_READ_FLAG_NORMAL, event) !=  LIBEVDEV_READ_STATUS_SUCCESS)
  {
    return false;
  }

  if(event->type == EV_KEY)
  {
    button_data[event->code] = event->value;
  }
  else if(event->type == EV_ABS)
  {
    abs_data[event->code] = event->value;
  }
  else if(event->type == EV_REL)
  {
    rel_data[event->code] = event->value;
  }

  return true;
}

void Joystick::print_verbose_info()
{
  if(evdev == nullptr) return;

  printf("Name: %s\n", libevdev_get_name(evdev));
  printf("Vendorid: %.4x\n", vendorid);
  printf("Productid: %.4x\n", productid);

  printf("\nBTN_count: %lu\n",get_num_buttons());
  for(int i=KEY_RESERVED; i<KEY_CNT; i++)
  {
    if(libevdev_has_event_code(evdev, EV_KEY, i))
    {
      printf("%4d : %24s\n", i, libevdev_event_code_get_name(EV_KEY, i));
    }
  }

  printf("\nREL_count: %lu\n", number_of_rels);
  for(int i=REL_X; i<REL_CNT; i++)
  {
    if(libevdev_has_event_code(evdev, EV_REL, i))
    {
      printf("%4d : %24s\n", i, libevdev_event_code_get_name(EV_REL, i));
    }
  }

  printf("\nABS_count: %lu\n", number_of_abs);
  for(int i=ABS_X; i<ABS_CNT; i++)
  {
    if(libevdev_has_event_code(evdev, EV_ABS, i))
    {
      const input_absinfo* abs_info = libevdev_get_abs_info(evdev, i);
      printf("%4d : %24s | Min: %6d | Max: %6d | Resolution: %6d | Fuzz: %6d | Flat: %6d\n",
      i, libevdev_event_code_get_name(EV_ABS, i), abs_info->minimum, abs_info->maximum, abs_info->resolution, abs_info->fuzz, abs_info->flat);
    }
  }
}

size_t Joystick::get_num_buttons()
{
  return number_of_buttons;
}

size_t Joystick::get_num_abs()
{
  return number_of_abs;
}

size_t Joystick::get_num_rel()
{
  return number_of_rels;
}


Joystick::~Joystick()
{
  close_joy();
};


std::vector<std::string> Joystick::SystemDeviceData::get_device_paths()
{
  const std::filesystem::path dir{"/dev/input/by-id"};
  std::vector<std::string> system_device_paths;
  
  // Use a non‑throwing iterator; any error will be reported via `ec`
  std::error_code ec;
  for (const auto& entry : std::filesystem::directory_iterator{dir, ec})
  {
    if (ec)
    {
      std::cerr << "Joystick::SystemDeviceData::refresh_device_paths: Error accessing " << entry.path() << ": " << ec.message() << '\n';
      continue;
    }
      system_device_paths.push_back(entry.path());
  }

  if (ec) std::cerr << "Finished directory iteration with error: " << ec.message() << '\n';

  return system_device_paths;
}

