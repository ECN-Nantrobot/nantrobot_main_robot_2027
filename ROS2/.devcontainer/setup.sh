#!/usr/bin/env bash
set -eo pipefail

source /opt/ros/jazzy/setup.bash
set -u
workspace="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

if [ ! -f /etc/ros/rosdep/sources.list.d/20-default.list ]; then
  if [ "$EUID" -eq 0 ]; then
    rosdep init
  else
    sudo rosdep init
  fi
fi
rosdep update
if [ "$EUID" -eq 0 ]; then
  apt-get update
else
  sudo apt-get update
fi
rosdep install --from-paths "$workspace/src" --ignore-src --rosdistro jazzy -y

cd "$workspace"
database=src/nantrobot_control/compile_commands.json
if [ ! -e "$database" ] && [ ! -L "$database" ]; then
  ln -s ../../build/nantrobot_control/compile_commands.json "$database"
fi
colcon build --cmake-args -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
