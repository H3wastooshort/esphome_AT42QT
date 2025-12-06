UNFINISHED!!
currently works for AT42QT2120. others need work

### Example
´´´yaml
i2c:
  scl: GPIO5
  sda: GPIO6

at42qt:
  id: touch_ic
  pulse_length: 128

binary_sensor:
  - platform: at42qt
    at42qt_hub_id: touch_ic
    name: "Touch 1"
    id: touch1
    channel: 8
    threshold: 5
    oversampling: 0x20
  - platform: at42qt
    at42qt_hub_id: touch_ic
    name: "Touch 2"
    id: touch2
    channel: 0
    threshold: 5
    oversampling: 0x20

sensor:
  - platform: at42qt
    at42qt_hub_id: touch_ic
    channel: 8
    signal:
      name: "Touch 1 Signal"
    reference:
      name: "Touch 1 Reference"
    update_interval: 10s
  - platform: at42qt
    at42qt_hub_id: touch_ic
    channel: 0
    signal:
      name: "Touch 2 Signal"
    reference:
      name: "Touch 2 Reference"
    update_interval: 10s
´´´

### Parts of Code taken from:
https://github.com/janelia-arduino/AT42QT/ by Peter Polidoro (peter@polidoro.io)
https://github.com/esphome/esphome/blob/dev/esphome/components/cap1188/