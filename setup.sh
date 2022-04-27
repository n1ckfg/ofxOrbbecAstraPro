# This enables accessing the camera at runtime without sudo
# https://3dclub.orbbec3d.com/t/orbbec-openi-problem-with-usb/279/5

sudo cp orbbec-usb.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules


