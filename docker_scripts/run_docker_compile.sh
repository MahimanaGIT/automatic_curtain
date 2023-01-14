#!/bin/bash 

# Pulling or updating the docker image
docker pull drdeshmukh97/mad_projects_automatic_curtain:1.4

# Remove if container is running with arduino-cli-automatic-curtain name
docker rm arduino-cli-automatic-curtain

# path to repository
repo_path=$(dirname $(dirname $(realpath ${BASH_SOURCE})))
echo $repo_path

# launching docker container and running compilation script at entrypoint
docker run \
-u 1030 \
-h mad_projects \
-v $repo_path:/home/mad_projects/automatic_curtain \
--device /dev/ttyUSB0 \
--name arduino-cli-automatic-curtain \
-it  \
--entrypoint "/home/mad_projects/automatic_curtain/docker_scripts/compile_code.sh" \
drdeshmukh97/mad_projects_automatic_curtain:1.4


exit 0
