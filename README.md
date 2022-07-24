# Automatic Curtain

1. Setup doxygen:
   - Install doxygen:
      ```
      sudo apt-get install doxygen
      ```
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