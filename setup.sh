# https://3dclub.orbbec3d.com/t/orbbec-openi-problem-with-usb/279/5
# https://github.com/pierrep/ofxOrbbecAstra/tree/master

sudo cp orbbec-usb.rules /etc/udev/rules.d/558-orbbec-usb.rules
sudo udevadm control --reload-rules

echo "Reboot to enable accessing the camera at runtime without sudo"
