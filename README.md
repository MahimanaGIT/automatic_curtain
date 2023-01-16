<!-- </br>

<div align="center">
  <a href="https://novu.co" target="_blank">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://user-images.githubusercontent.com/8872447/165779319-34962ccc-3149-466c-b1da-97fd93254520.png">
    <img src="https://user-images.githubusercontent.com/8872447/165779274-22a190da-3284-487e-bd1e-14983df12cbb.png" width="280" alt="Logo"/>
  </picture>
  </a>
</div> -->

<h1 align="center">Automatic Curtain</h1>

<div align="center">
Elevate your home with IoT-enabled smart curtain controller for effortless living
</div>

## ⭐️ Why Automatic Curtain?
Automating blinds can provide convenience and energy efficiency. They can be programmed to open and close at specific times of the day, allowing natural light in to reduce the need of artificial lighting. They can also be controlled remotely, making it easy to adjust the amount of light and privacy in a room without having to physically adjust the blinds. Additionally, automatic blinds can help to regulate the temperature in a room.

## 🔥 Watch the video
[![Video](https://img.youtube.com/vi/RMJ6SqKFGqU/0.jpg)](https://www.youtube.com/watch?v=YOUTUBE_VIDEO_ID_HERE)

## ✨ Features

- 😎 Easy to set up and integrate
- 📱 Remote control - the ability to open and close the curtains with precision using Alexa app
- 🛡 Safety feature - the ability to detect obstacles and stop the motor to prevent injury or damage to the curtains
- 💅 Manual Override - the ability to manually operate the curtains in case of Alexa not available
- 🚀 Motorized operation - the ability to open and close the curtains using a motor, rather than manual operation
- 📦 Smart home integration - the ability to connect the blinds to other smart home devices such as thermostat using alexa. Provides added convenience and energy efficiency

## 📋 [Doxygen Documentation](https://mahimanagit.github.io/automatic_curtain/)

## 🚀 Getting Started
### For how to use and build your own, refer [🗒 Manual](resources/MANUAL/manual.pdf)
### 👨‍💻 [Coding Style](https://google.github.io/styleguide/cppguide.html)
### Steps to setup for development:
1. Clone the repo
2. Setup clang format:
   - Install clang format extension in VS Code: 
    ```
      Name: Clang-Format
      Id: xaver.clang-format
      Description: Use Clang-Format in Visual Studio Code
      Version: 1.9.0
      Publisher: xaver
      VS Marketplace Link: https://marketplace.visualstudio.com/items?itemName=xaver.clang-format
    ```
   - Add following lines to **settings.json** in your VS Code:
    ```
    "editor.formatOnSave": true
    ```
    Now, everytime you save your code will automatically be formatted according to clang
    
3. Setup ESP32 Arduino Core using [this guide](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/)
   
4. Setup Libraries
   - The link to all the libraries can be found at the end of this page. A compressed folder of all the libraries can be found in the [folder](resources/LIBRARY_ZIP/ac_lib.zip)
   - You can download the libraries using the links or just copy the extracted libraries from the above compressed folder in the **~/Arduino/libraries** or your customized library folder.

### Optional setup for development:
1. Setup doxygen:
   - Install doxygen and graphviz (doxygen uses dot from graphviz to generate graphs):
      ```
      sudo apt-get install doxygen
      sudo apt-get install graphviz 
      ```
   - Currently, the **DOT_PATH** is hardcoded in **Doxyfile** as `/usr/bin/dot` on line no. 2398, one can check if the dot path in their system is same or not by running
      ```
      which dot
      ```
      If the path is different, update it in the doxyfile
   - Generate doxygen files by running the following command in the terminal:
      ```
      doxygen Doxyfile
      ```
    - The files will be available in `doxygen` folder, click on or open `index.html` in **html** folder with any browser and you would be able to browse the documentation


## 🛡️ License

Automatic Curtain is licensed under the MIT License - see the [LICENSE](LICENSE.md) file for details.


## 🗒 References:

Thanks to the following repos for making the journey easier for the project:

1. [AsyncTCP](https://github.com/me-no-dev/AsyncTCP.git)
2. [Fauxmo](https://github.com/vintlabs/fauxmoESP.git)
3. [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer.git)
4. [FastLED](https://github.com/FastLED/FastLED.git)
5. [TMCStepper](https://github.com/teemuatlut/TMCStepper.gi)

For awesome readme help, referenced [novu](https://github.com/novuhq/novu)

## ✏️ Contributors 

- Mahimana Bhatt (mahimanabhatt@gmail.com)
- Dhiraj Deshmukh (deshmukhdhiraj15@gmail.com)

## 💻 Need Help?

We are more than happy to help you. If you are getting any errors or facing problems while working on this project, join our [slack](https://join.slack.com/t/mad-myj2315/shared_invite/zt-1n3oqsv1z-pdAomyv074gVCvfYfFaRgA) and ask for help. We are open to discussing anything related to the project.
