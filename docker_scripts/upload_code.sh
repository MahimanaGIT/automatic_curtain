#!/bin/bash

output_file="/home/mad_projects/automatic_curtain/docker_scripts/upload_output.txt"

arduino-cli compile \
--fqbn esp32:esp32:esp32doit-devkit-v1 \
--port /dev/ttyUSB0 \
--upload \
--verify /home/mad_projects/automatic_curtain/mvp > $output_file 2>&1


exit 0