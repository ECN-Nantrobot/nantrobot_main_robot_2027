# nantrobot_control

`nantrobot_control` includes the MCU bridge for drive commands, wheel feedback, and IMU measurements.

## Build and launch

Run these commands from the workspace directory.

```sh
source /opt/ros/jazzy/setup.bash
colcon build --packages-select nantrobot_control
source install/setup.bash
ros2 launch nantrobot_control control.launch.xml
```

## Recording a run

Recording is off by default. Enable it at launch:

```sh
ros2 launch nantrobot_control control.launch.xml record:=true
ros2 launch nantrobot_control teleop.launch.xml record:=true
```

## Tests

Run these commands from the workspace directory:

```sh
source /opt/ros/jazzy/setup.bash
colcon build --packages-select nantrobot_control --cmake-args -DBUILD_TESTING=ON
source install/setup.bash
colcon test --packages-select nantrobot_control --return-code-on-test-failure
colcon test-result --test-result-base build/nantrobot_control --verbose
```

## Package layout

| Files | Responsibility |
| --- | --- |
| [`src/mcu/bridge_node.cpp`](src/mcu/bridge_node.cpp)<br>[`include/mcu/bridge_node.hpp`](include/mcu/bridge_node.hpp) | MCU bridge node. Subscribes to velocity commands, enforces command freshness, and publishes timestamped wheel and IMU feedback. |
| [`src/mcu/bridge_node/serial.cpp`](src/mcu/bridge_node/serial.cpp) | Serial setup, reads and writes, disconnect cleanup, and reconnect handling for the bridge. |
| [`src/mcu/bridge_node/settings.cpp`](src/mcu/bridge_node/settings.cpp) | Loads and validates bridge parameters at startup. The bridge header declares the settings struct. |
| [`src/mcu/clock_sync.cpp`](src/mcu/clock_sync.cpp)<br>[`include/mcu/clock_sync.hpp`](include/mcu/clock_sync.hpp) | Matches sync replies to requests and converts MCU acquisition times to ROS timestamps. |
| [`src/mcu/protocol.cpp`](src/mcu/protocol.cpp)<br>[`include/mcu/protocol.hpp`](include/mcu/protocol.hpp) | Defines message types and wire sizes, encodes outgoing frames, and decodes incoming serial data with checksum validation. |
| [`launch/control.launch.xml`](launch/control.launch.xml)<br>[`config/control.yaml`](config/control.yaml) | Control bring-up and MCU bridge configuration. |
| [`launch/teleop.launch.xml`](launch/teleop.launch.xml)<br>[`config/teleop.yaml`](config/teleop.yaml) | Gamepad bring-up, joystick mappings, and velocity scaling. The launch file also loads the bridge settings from control.yaml. |
| [`test/mcu/test_protocol.cpp`](test/mcu/test_protocol.cpp)<br>[`test/mcu/test_clock_sync.cpp`](test/mcu/test_clock_sync.cpp)<br>[`test/mcu/test_bridge.py`](test/mcu/test_bridge.py) | Protocol and clock-sync unit tests plus a bridge integration test using simulated serial ports. See [Tests](#tests) for the run commands. |
| [`CMakeLists.txt`](CMakeLists.txt)<br>[`package.xml`](package.xml) | Build and test targets, installation rules, and package dependencies. |

## Contracts

These are the contracts with the other packages.

### `nantrobot_firmware`

The firmware and MCU bridge exchange the following wire-protocol messages.

| Direction | Contract |
| --- | --- |
| Control → firmware | Send `DriveCommand` with target forward speed, target yaw rate, and drive permission. |
| Control → firmware | Send `TimeSyncRequest` at startup, after reconnect or firmware reboot, and periodically. |
| Firmware → control | Stream `DriveFeedback` with measured wheel positions and speeds, sample timing, and drive status. |
| Firmware → control | Respond to each `TimeSyncRequest` with `TimeSyncReply`. |
| Firmware → control | Stream `ImuFeedback` with angular velocity in rad/s, acceleration in m/s², and the acquisition time in ESP32 uptime. |

#### Settings

* Baud `115200`
* Data bits `8`
* Parity `None`
* Stop bits `1`

#### Message structure

| Position | Field | Type | Meaning |
| --- | --- | --- | --- |
| 0 | `version` | `u8` | Protocol version, `0x01`. |
| 1 | `type` | `u8` | Selects a payload layout below. |
| 2 | `payload_length` | `u8` | Number of decoded payload bytes. |
| 3 | `payload` | byte array | Message fields in the order listed below. |
| 3 + `payload_length` | `crc` | `u16` | CRC-16/CCITT-FALSE over the header and payload, stored low byte first. |

#### `DriveCommand`

* Type `0x01`
* Payload `13 bytes`

| Payload offset | Field | Type | Meaning |
| --- | --- | --- | --- |
| 0 | `sequence` | `u32` | Increments for each new command. |
| 4 | `forward_speed_mps` | `f32` | Signed target linear speed in m/s, positive forward. |
| 8 | `yaw_rate_radps` | `f32` | Signed target yaw rate in rad/s, positive counterclockwise viewed from above. |
| 12 | `drive_flags` | `u8` | Bit 0 permits drive. Clear it to request a stop. Bits 1–7 are reserved and must be zero. |

#### `DriveFeedback`

* Type `0x02`
* Payload `33 bytes`

| Payload offset | Field | Type | Meaning |
| --- | --- | --- | --- |
| 0 | `boot_id` | `u32` | Identifier that changes on each firmware boot. |
| 4 | `sample_time_us` | `u64` | Encoder measurement time in µs since boot. |
| 12 | `left_wheel_position_rad` | `f32` | Accumulated left wheel angle in radians since boot. |
| 16 | `right_wheel_position_rad` | `f32` | Accumulated right wheel angle in radians since boot. |
| 20 | `left_wheel_speed_radps` | `f32` | Measured left wheel speed in rad/s. |
| 24 | `right_wheel_speed_radps` | `f32` | Measured right wheel speed in rad/s. |
| 28 | `last_applied_sequence` | `u32` | Sequence of the most recent applied drive command. Valid when status bit 4 is set. |
| 32 | `status` | `u8` | TBD |

#### `TimeSyncRequest`

* Type `0x03`
* Payload `4 bytes`

| Payload offset | Field | Type | Meaning |
| --- | --- | --- | --- |
| 0 | `request_id` | `u32` | Identifies the request. |

#### `TimeSyncReply`

* Type `0x04`
* Payload `16 bytes`

| Payload offset | Field | Type | Meaning |
| --- | --- | --- | --- |
| 0 | `request_id` | `u32` | Echoes the request ID so the Pi can match the reply. |
| 4 | `boot_id` | `u32` | Identifier that changes on each firmware boot. |
| 8 | `eepoch_time_us` | `u64` | ESP32 uptime in µs, captured while handling the request. |

#### `ImuFeedback`

* Firmware → control
* Type `0x05`
* Payload `36 bytes`

| Payload offset | Field | Type | Meaning |
| --- | --- | --- | --- |
| 0 | `boot_id` | `u32` | Current ESP32 boot ID, shared with drive feedback and time sync. |
| 4 | `sample_time_us` | `u64` | IMU acquisition time in µs since boot. |
| 12 | `angular_velocity_x_radps` | `f32` | Angular velocity about the IMU X axis in rad/s. |
| 16 | `angular_velocity_y_radps` | `f32` | Angular velocity about the IMU Y axis in rad/s. |
| 20 | `angular_velocity_z_radps` | `f32` | Angular velocity about the IMU Z axis in rad/s. |
| 24 | `linear_acceleration_x_mps2` | `f32` | Accelerometer reading along the IMU X axis in m/s². |
| 28 | `linear_acceleration_y_mps2` | `f32` | Accelerometer reading along the IMU Y axis in m/s². |
| 32 | `linear_acceleration_z_mps2` | `f32` | Accelerometer reading along the IMU Z axis in m/s². |

### `nantrobot_path_planning`

| Direction | Contract |
| --- | --- |
| Planning → control | Provide an ordered path in the `map` frame using `nav_msgs/msg/Path` in a goal sent to the `/control/follow_path` action. |
| Control → planning | Provide drive constraints with `nav2_msgs/msg/SpeedLimit` on the `/control/speed_limit` topic and report execution progress and outcome through the `/control/follow_path` action (`nav2_msgs/action/FollowPath`). |
| Planning → control | Request a straight reverse maneuver through the `/control/backup` action (`nav2_msgs/action/BackUp`). Send the distance, speed, and time allowance. Control returns distance traveled and the final result through the same action. |
| Planning → control | Request an in-place turn through the `/control/spin` action (`nav2_msgs/action/Spin`). Send the relative yaw change in radians and a time allowance. Control returns angular distance traveled and the final result through the same action. |
| Planning → control | Request a straight move through the `/control/drive_on_heading` action (`nav2_msgs/action/DriveOnHeading`). Send the distance along the robot’s X axis in metres, speed in m/s, and a time allowance. Distance and speed must have the same sign. Control returns distance traveled and the final result through the same action. |

### `nantrobot_localization`

| Direction | Contract |
| --- | --- |
| Localization → control | Provide timestamped odometry `nav_msgs/msg/Odometry` on the `/odometry/filtered` topic with the pose in the odometry frame `odom` and the twist in the robot frame `base_link` (child frame). |
| Control → localization | Expose measured wheel feedback as `sensor_msgs/msg/JointState` on the `/joint_states` topic. |
| Control → localization | Expose timestamped IMU samples as `sensor_msgs/msg/Imu` on the `/imu/data_raw` topic in the IMU frame `imu_frame`. |
| Control → localization | Provide `sensor_msgs/msg/LaserScan` on the `/scan/data_raw` topic in the LiDAR frame `laser_frame`. |

