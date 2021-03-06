# SpeederCapture

A simple Raspberry Pi project that measures the speed of a passing vehicle and captures a picture of the vehicle if it is exceeding a certain speed. 
The project utilizes an OmniPreSense OPS243-C Single Board Radar Sensor, Raspberry Pi4, and Raspberry Pi Camera Module V2. 

The primary program for reading the radar sensor data is written using C++. 
The program records the vehicle speed, direction, date, time, if the vehicle was a "speeder", and the image file name (if applicable). 
These attributes are written to a .json file. 

The camera uses the libcamera library and outputs the vehicle image in .jpg form. 

Both the .json file and .jpg image are uploaded to a Google Cloud storage bucket for storage and additional processing (future add-on program). 
The upload is accomplished with a simple Python3 program. 
The Python3 program references a .json file which contains the credentials for accessing the GCP project. The credentials .json file is not saved within this respository to ensure the GPC project credentials remain secret.  
