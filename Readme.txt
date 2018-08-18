
+Create an MQTT Broker With CloudMQTT : https://www.youtube.com/watch?v=DPmFJLXhj80&t=1s
+MQTT + CloudMQTT + App de Android LinearMQTT: https://www.youtube.com/watch?v=MuTaFhuMdAo

+App esp8266 smartconfig


-Initially, after loading the code,we open the smartconfig app--> enter the wifi name and password then press "confirm". (The phone must turn on bluetooth). Pressing the button to reset the value to 0. After that, we will start weighing. 
We have three opsition: button 1(GPIO 2): 2.3v, button 2(GPIO 23):  12.5v, button 3(GPIO 32): 5v.
The value of the loadcell and the gas sensor will be transmitted directly to the oled monitor and the server.
-The app reads the data that described through the youtube link, which can be configured as an image on github. Note the topic name .

-We can calib the value of loadcel to get the correct result by editing the value in line 88: LoadCell.setCalFactor (100000);
