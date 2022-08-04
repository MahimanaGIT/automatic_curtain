# Automatic Curtain

### [Coding Style](https://google.github.io/styleguide/cppguide.html)

### Steps to setup:
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