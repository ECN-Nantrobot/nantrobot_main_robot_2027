# nantrobot_control

`nantrobot_control` is the package for executing the robot’s planned motion and interacting with the MCU. It uses the planned path and the estimated robot pose to compute forward speed and yaw rate, checks whether a motion is allowed, and sends approved commands to the MCU.

## Build and launch

Run these commands from the workspace:

```sh
source /opt/ros/jazzy/setup.bash
colcon build --packages-select nantrobot_control
source install/setup.bash
```

For normal launch:

```sh
ros2 launch nantrobot_control control.launch.xml
```

For teleop with an Xbox controller:

```sh
ros2 launch nantrobot_control teleop.launch.xml
```

## Recording a run

Recording is off by default. Enable it at launch:

```sh
ros2 launch nantrobot_control control.launch.xml record:=true
ros2 launch nantrobot_control teleop.launch.xml record:=true
```

## Tests

Run the Python tests from the workspace. The build compiles a test-only Python binding for the MCU codec and clock. The tests run when you invoke `colcon test`.

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
| [`src/mcu/`](src/mcu/) | The bridge node handles drive commands and sensor feedback. Clock sync converts MCU timestamps to ROS time, and the protocol encodes and decodes serial frames. |
| [`src/mcu/bridge_node/`](src/mcu/bridge_node/) | Serial I/O, USB reconnection, and bridge parameter loading and validation. |
| [`src/motion/`](src/motion/) | The motion supervisor checks odometry before forwarding autonomous velocity requests to the collision monitor. |
| [`launch/control.launch.xml`](launch/control.launch.xml)<br>[`config/control.yaml`](config/control.yaml)<br>[`config/nav2.yaml`](config/nav2.yaml) | Control bring-up and configuration for the bridge, motion controller, and Nav2 nodes. |
| [`launch/teleop.launch.xml`](launch/teleop.launch.xml)<br>[`config/teleop.yaml`](config/teleop.yaml) | Gamepad bring-up, joystick mappings, and velocity scaling. |
| [`test/mcu/`](test/mcu/) | Protocol and clock-sync unit tests plus a bridge integration test using simulated serial ports. |
| [`test/motion/`](test/motion/) | Tests odometry timeout and recovery, and runs a Nav2 path through the control launch with simulated sensors and a serial port. |
| [`CMakeLists.txt`](CMakeLists.txt)<br>[`package.xml`](package.xml) | Build and test targets, installation rules, and package dependencies. |

## Algorithms

### Path tracking ([Rotation Shim Controller](https://docs.nav2.org/jazzy/configuration_and_development/configuration_guide/controller_plugins/configuring_rotation_shim_controller/) · [Regulated Pure Pursuit](https://docs.nav2.org/jazzy/configuration_and_development/configuration_guide/controller_plugins/configuring_regulated_pp/))

When starting a path, Nav2’s Rotation Shim Controller checks the robot’s heading against the start of the path. If the robot is misaligned, it rotates in place until the heading is within tolerance. It then hands off to Regulated Pure Pursuit, which follows the path and computes forward speed and yaw rate. It can slow for tight turns, nearby obstacles, and goal approach. With collision checking enabled, it projects the commanded motion against the local costmap and stops if it predicts a collision. A detour requires a new path from planning.

### [Collision Monitor](https://docs.nav2.org/jazzy/configuration_and_development/configuration_guide/core_servers/collision_monitor/)

Nav2’s Collision Monitor subscribes to the LiDAR scans and checks them against zones around the robot. It filters commands from both the path controller and Behavior Server before they reach the MCU, reducing speed or commanding a stop when a zone triggers. 

### [BackUp](https://docs.nav2.org/jazzy/configuration_and_development/configuration_guide/core_servers/bt_plugins/actions/BackUp/)

Nav2’s Behavior Server runs `BackUp` as a straight reverse maneuver with zero commanded yaw rate. It can be used to change the reset the robot position from the target to retry a monuver. It is supplied the reverse distance, speed, and time allowance. The commands are routed through the collision monitor.

### [Spin](https://docs.nav2.org/jazzy/configuration_and_development/configuration_guide/core_servers/bt_plugins/actions/Spin/)

Nav2’s Behavior Server runs `Spin` as an in-place turn. The mission controller supplies a signed yaw change in radians relative to the robot’s current heading and a time allowance. It can be used to reorient the robot.

### [DriveOnHeading](https://docs.nav2.org/jazzy/configuration_and_development/configuration_guide/core_servers/bt_plugins/actions/DriveOnHeading/)

Nav2’s Behavior Server runs `DriveOnHeading` as a straight move along the robot’s current heading with zero commanded yaw rate. It can be used for a short forward move.

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
| Firmware → control | Stream `ImuFeedback` with angular velocity in rad/s, acceleration in m/s², and the acquisition time in MCU uptime. |

#### Settings

* Baud `115200`
* Data bits `8`
* Parity `None`
* Stop bits `1`

#### Message structure

Both endpoints COBS-encode the complete message below, including its CRC, and append one
`0x00` byte as the frame delimiter. The table gives offsets after COBS decoding. Receivers
discard malformed frames through the delimiter and resume at the next frame.

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
* Frequency `25 Hz`
* Timeout `5 samples (200 ms)`

| Payload offset | Field | Type | Meaning |
| --- | --- | --- | --- |
| 0 | `sequence` | `u32` | Increments for each new command. |
| 4 | `forward_speed_mps` | `f32` | Signed target linear speed in m/s, positive forward. |
| 8 | `yaw_rate_radps` | `f32` | Signed target yaw rate in rad/s, positive counterclockwise viewed from above. |
| 12 | `drive_flags` | `u8` | Bit 0 permits drive. Clear it to request a stop. Bits 1–7 are reserved and must be zero. |

| Bit | Flag | Meaning when set |
| --- | --- | --- |
| 0 | `enable` | The Pi has permitted drive output. When cleared, the MCU should stop motion and disable the drives. |
| 1-7 | `N/A` | Reserved |

#### `DriveFeedback`

* Type `0x02`
* Payload `33 bytes`
* Frequency `50 Hz`
* Timeout `5 samples (100 ms)`

| Payload offset | Field | Type | Meaning |
| --- | --- | --- | --- |
| 0 | `boot_id` | `u32` | Identifier that changes on each firmware boot. |
| 4 | `sample_time_us` | `u64` | Encoder measurement time in µs since boot. |
| 12 | `left_wheel_position_rad` | `f32` | Accumulated left wheel angle in radians since boot. |
| 16 | `right_wheel_position_rad` | `f32` | Accumulated right wheel angle in radians since boot. |
| 20 | `left_wheel_speed_radps` | `f32` | Measured left wheel speed in rad/s. |
| 24 | `right_wheel_speed_radps` | `f32` | Measured right wheel speed in rad/s. |
| 28 | `last_applied_sequence` | `u32` | Sequence of the most recent applied drive command. |
| 32 | `feedback_flags` | `u8` | Drive status flags below. |

| Bit | Flag | Meaning when set |
| --- | --- | --- |
| 0 | `enabled` | The MCU has enabled drive output. A permitted zero target can still be enabled. |
| 1 | `command_timeout` | The MCU stopped locally because fresh drive commands stopped arriving. |
| 2-7 | `N/A` | Reserved |

#### `TimeSyncRequest`

* Type `0x03`
* Payload `4 bytes`

| Payload offset | Field | Type | Meaning |
| --- | --- | --- | --- |
| 0 | `request_id` | `u32` | Identifies the request. |

#### `TimeSyncReply`

* Type `0x04`
* Payload `16 bytes`
* Timeout `100 ms (5 retries)`

| Payload offset | Field | Type | Meaning |
| --- | --- | --- | --- |
| 0 | `request_id` | `u32` | Echoes the request ID so the Pi can match the reply. |
| 4 | `boot_id` | `u32` | Identifier that changes on each firmware boot. |
| 8 | `time_us` | `u64` | MCU uptime in µs, captured while handling the request. |

#### `ImuFeedback`

* Firmware → control
* Type `0x05`
* Payload `36 bytes`

| Payload offset | Field | Type | Meaning |
| --- | --- | --- | --- |
| 0 | `boot_id` | `u32` | Current MCU boot ID, shared with drive feedback and time sync. |
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

