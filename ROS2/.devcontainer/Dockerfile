FROM ros:jazzy-ros-base

RUN apt-get update && apt-get install -y --no-install-recommends \
    clangd \
    git \
    lldb \
    openssh-server \
    python3-colcon-common-extensions \
    python3-rosdep \
    sudo \
    systemd \
    systemd-sysv \
    && rm -rf /var/lib/apt/lists/* \
    && mkdir -p /run/sshd \
    && systemctl enable ssh

RUN printf '\nsource /opt/ros/jazzy/setup.bash\nif [ -f /workspace/install/setup.bash ]; then source /workspace/install/setup.bash; fi\n' >> /etc/bash.bashrc

STOPSIGNAL SIGRTMIN+3
CMD ["/sbin/init"]
