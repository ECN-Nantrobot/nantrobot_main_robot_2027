# Firmware

ESP-IDF firmware for the main robot's ESP32-S3: motors, encoders, IMU, battery and the serial link to the Raspberry Pi.

- Target: ESP32-S3
- Framework: ESP-IDF v6.1.1
- Language: C

## Layout

```text
Firmware/
├── CMakeLists.txt        project file
├── sdkconfig.defaults    team-shared configuration
├── main/                 app_main: init modules, start tasks
├── components/
│   ├── board/            pin map (all GPIO numbers live here)
│   ├── motor/            H-bridge driver (PWM, direction)
│   ├── encoder/          quadrature encoders (PCNT)
│   ├── drive_control/    speed PID, kinematics, command watchdog
│   ├── imu/
│   ├── battery/
│   ├── comms/            serial link, framing, message packing
│   └── params/           tunable parameters stored in NVS
└── tests/                unit tests
```

## Build and flash

With the ESP-IDF environment active, from `Firmware/`:

```bash
idf.py set-target esp32s3   # first time only
idf.py build
idf.py -p /dev/ttyACM0 flash monitor
```

## Conventions

- One component per module, with its public API in `include/<module>.h`.
- Public names are prefixed with the module name (`motor_init`, `encoder_get_ticks`); everything else is `static`.
- Each module exposes `<module>_init(...)` returning `esp_err_t`, called from `app_main`.
- No GPIO numbers outside `components/board/`.
- No global variables shared between modules: use functions, queues or mutex-protected data.
- Use `PRIV_REQUIRES` by default; `REQUIRES` only when a public header needs the dependency.
- One `static const char *TAG = "<module>";` per module for logging.

## Tasks

| Task | Period | Priority | Core | Stack |
|---|---|---|---|---|
| | | | | |
