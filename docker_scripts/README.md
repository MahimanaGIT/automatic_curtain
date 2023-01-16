# Start serial monitor to debug

## Install screen package 
 ```
$ sudo apt install screen
 ```
## To start serial monitor 
 ```
$ screen /dev/ttyUSB0 115200
 ```
## To exit serial monitor from screen 

- Press ctrl+a (to enter command mode)
- Then type ":quit" and press "enter"
- Incase you exit terminal you might not be able to start the screen serial monitor again as process is running in background
- To kill the background "screen" process run $ screen -ls  output looks like : 27997.pts-0.dhirajsinh-GS65-Stealth-9SE	(01/13/2023 07:07:30 PM
- Then kill process by : 
  ```
  $ kill -9 27997
  ```
- Now the screen process can be started again


**Note** : You should have docker installed on your system to run the scripts, compile/upload code Follow the guide to install docker : https://docs.docker.com/engine/install/ubuntu/

**Note** : Internet connect might be needed if you dont have docker image .  

# Compile the code
 ```
$ cd <project_repo>/docker_scripts/
$ sudo ./run_docker_compile.sh
 ```

- And for compilation output see ->  <project_repo>/docker_scripts/compile_output.txt

# Upload the code
 ```
$ cd <project_repo>/docker_scripts/
$ sudo ./run_docker_upload.sh
 ```

- And for upload output see ->  <project_repo>/docker_scripts/upload_output.txt
