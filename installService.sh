if [[ $UID != 0 ]]; then
    echo "Please run this script with sudo:"
    echo "sudo $0 $*"
    exit 1
fi
if [ "$#" -ne 1 ]
then
  echo "Please provide the lua script that will be installed as a service in /etc/wejoy/config.lua"
  echo "Usage: ./installService.sh CONFIG_SCRIPT.lua"
  exit 1
fi

mkdir -p /etc/wejoy
cp $1 /etc/wejoy/config.lua
cp wejoy.service /etc/systemd/system/
systemctl daemon-reload
systemctl start wejoy
systemctl enable wejoy
echo "Wejoy was installed and started. You can test it on your gamepad settings or with evtest"