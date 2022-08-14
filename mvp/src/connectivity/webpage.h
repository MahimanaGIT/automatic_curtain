/**
 * @file webpage.h
 * @author Mahimana Bhatt (mahimanabhatt@gmail.com)
 * @brief Contains resources related to webpage for eg. html code
 * @version 0.1
 * @date 2022-07-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef ElegantOTAWebpage_h
#define ElegantOTAWebpage_h

const char index_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
    <head>
    <title>MaD</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
            body {font-family: Arial, Helvetica, sans-serif;}* {box-sizing: border-box;}

            input[type=text], [type=email], [type=password] {
                display: inline-block;
                padding: 12px;
                border: 1px solid #ccc;
                border-radius: 4px;
                box-sizing: border-box;
                margin-top: 6px;
                margin-bottom: 16px;
                resize: vertical;
                text-align: left;
            }
            
            label{
                display: inline-block;
                width: fit-content;
                text-align: left;
            }
            
            input[type=submit] {
                background-color: #4CAF50;
                color: white;
                padding: 12px 20px;
                border: none;
                border-radius: 4px;
                cursor: pointer;
            }

            input[type=submit]:hover {
                background-color: #09d813;
            }

            .container {
                margin: auto;
                width: fit-content;
                border-radius: 5px;
                background-color: #f2f2f2;
                padding: 20px;
                text-align: center;
            }
        </style>
    </head>
    <body>
        <div class="container">
            <h1>MaD Automatic Curtain Setup</h1><br>
            <form action="/submit">
                <label for="device_name">Device Name</label><br>
                <input type="text" id="device_name" name="device_name" required="required" size="32" placeholder="Enter device name to be displayed in Alexa.."><br>
                <label for="wifi">Wifi SSID</label><br>
                <input type="text" id="wifi_ssid" name="wifi_ssid" required="required" size="32" placeholder="Enter Wifi SSID.."><br>
                <label for="wifi_password">Wifi Password</label><br>
                <input type="password" id="wifi_password" name="wifi_password" size="32" placeholder="Enter Wifi Password.."><br>
                <input type="submit" value="Submit">
            </form>
        </div>
    </body>
</html>
)=====";

const char dialog_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
    <head>
        <title>MaD</title>
        <style>
            body {font-family: Arial, Helvetica, sans-serif;}* {box-sizing: border-box;}

            h1 {
                text-align: center;
                font-family: Tahoma, Arial, sans-serif;
                color: #06D85F;
                margin: 80px 0;
            }

            .overlay {
                position: fixed;
                top: 0;
                bottom: 0;
                left: 0;
                right: 0;
                background: #f2f2f2;
            }

            .overlay:target {
                visibility: visible;
                opacity: 1;
            }

            .popup {
                margin: 70px auto;
                padding: 20px;
                background: #fff;
                border-radius: 5px;
                width: 30%;
            }

            .popup h2 {
                margin-top: 0;
                color: #333;
                font-family: Tahoma, Arial, sans-serif;
            }
            .popup .content {
                max-height: 30%;
                overflow: auto;
            }

            @media screen and (max-width: 700px){
                .popup{
                    width: 70%;
                }
            }
        </style>
    </head>
    <body>
        <div id="popup1" class="overlay">
            <div class="popup">
            <h2>Submission Successful</h2>
                <div class="content">
                    You can close this tab now.
                </div>
            </div>
        </div>
    </body>
</html>
)=====";

#endif