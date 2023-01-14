#!/bin/bash

# filename to which the compilation output is routed 
output_file="/home/mad_projects/automatic_curtain/docker_scripts/compile_output.txt"

# arduino cli command to compile the code 
arduino-cli compile \
--fqbn esp32:esp32:esp32doit-devkit-v1 \
--verify /home/mad_projects/automatic_curtain/mvp > $output_file 2>&1

exit 0