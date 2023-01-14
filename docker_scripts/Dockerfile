# Base Image
FROM  ubuntu:20.04
LABEL Name=automatic_curtain Version=1.4

# setting Time Zone/Region
ENV TZ=America/New_York
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Install python dependencies (for uploading arduino code uses python infrastructure)
RUN apt-get update \
    && apt-get install -y curl git build-essential ffmpeg python3 python3-pip sudo \
    && apt-get clean \
    && ln /usr/bin/python3 /usr/bin/python
RUN pip3 install pyserial

# setting up user with UID and GID 
RUN export uid=1030 gid=1030 && \
    mkdir -p /home/mad_projects && \
    mkdir -p /etc/sudoers.d && \
    echo "mad_projects:x:${uid}:${gid}:MAD_PROJECTS,,,:/home/mad_projects:/bin/bash" >> /etc/passwd && \
    echo "%mad_projects ALL = (ALL:ALL) ALL" >> /etc/sudoers && \
    echo "mad_projects:pass" | chpasswd && \
    echo "mad_projects:x:${uid}:" >> /etc/group && \
    chown ${uid}:${gid} -R /home/mad_projects 


# Add mad_projects user to the dialout group to be ale to write the serial USB device
RUN sed "s/^dialout.*/&mad_projects/" /etc/group -i \
&&  sed "s/^root.*/&mad_projects/" /etc/group -i \
&&  sed "s/^sudo.*/&mad_projects/" /etc/group -i




# Installing arduino-cli
ENV HOME /home/mad_projects
WORKDIR /home/mad_projects
RUN mkdir -p ./arduino
WORKDIR /home/mad_projects/arduino
RUN curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh

# Installing ESP32 and ESP8266 core for arduino-cli
ENV PATH="${PATH}:/home/mad_projects/arduino/bin"
ADD ./arduino-cli.yaml /home/mad_projects/arduino-cli.yaml
RUN arduino-cli --config-file /home/mad_projects/arduino-cli.yaml
RUN arduino-cli core update-index 
RUN arduino-cli core install esp32:esp32
RUN arduino-cli core install esp8266:esp8266

# Installing libraries needed for project:automatic_curtains
ENV ARDUINO_LIBRARY_ENABLE_UNSAFE_INSTALL=true
RUN arduino-cli lib install --git-url \
https://github.com/me-no-dev/AsyncTCP.git \
https://github.com/vintlabs/fauxmoESP.git \
https://github.com/me-no-dev/ESPAsyncWebServer.git \
https://github.com/FastLED/FastLED.git \
https://github.com/teemuatlut/TMCStepper.git
ENV ARDUINO_LIBRARY_ENABLE_UNSAFE_INSTALL=false


# Container welcome command execution
WORKDIR /home/mad_projects
CMD ["echo", "Welcome to MAD-automaticcurtain-docker"]