//============================================================================
// Name        : FinalProject.cpp
// Author      : Elizabeth A. Kelly
// Description : Sweng452 FinalProject - Speeder Capture
//============================================================================

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <fstream>
#include <sstream>

#include <fcntl.h>		// open() function for Serial Port
#include <errno.h>		// Error handling functions
#include <termios.h> 	// Terminal control definitions for Serial Port
#include <unistd.h>	 	// read() and close() functions for Serial Port

using namespace std;

int main() {

	// Open the serial port for the radar module
	int serial_port = open("/dev/ttyACM0", O_RDWR);

	// Check for any errors with opening the port
	if (serial_port < 0)
		printf("Error %i from open: %s\n", errno, strerror(errno));
	else
		printf("Serial Port Connection Successful...\n");

	// Struct used to configure the serial port
	struct termios tty;

	// Set the Baud Rate input and output
	cfsetspeed (&tty, B9600);

	// Read data from radar module
	while(true){

		string speeder;
		string filename;
		string timestamp;

		// Size of the port buffer
		int buffer_size = 125;

		// Buffer to hold the port data
		char port_buffer[buffer_size];

		// Read data from the port and place in the buffer
		int bytes_read = read(serial_port, &port_buffer, buffer_size);

		// Convert buffer to string
		if(bytes_read > 1 && port_buffer[1] == 'm'){
			int i = 6;
			string string_value = "";
			while (i < bytes_read-1){
				string_value += port_buffer[i];
				++i;
			}

			// Convert String to Float to get vehicle speed
			float speed_value;
			speed_value = std::stof(string_value);

			// fetch current date and time
			time_t now = time(0);
			tm *local_time = localtime(&now);

			// format date as YYYY-MM-DD
			char date_char[11];
			strftime(date_char,11,"%F", local_time);

			// format time as HH:MM:SS
			char time_char[9];
			strftime(time_char,9,"%T",local_time);

			// Print speed and time
			printf("\nSpeed is:  %.1f mph\t Time: %s\n", abs(speed_value), time_char);


			// logic for the filenames
			for (int i = 0; i < 10; ++i)
				filename += date_char[i];
			filename += "_";
			for (int i = 0; i < 8; ++i)
				filename += time_char[i];

			// if speed is excessive capture picture and tag as speeder
			if(abs(speed_value) >= 35.0) {

				// capture image
				string str = "libcamera-still -o " + filename + ".jpg -n -t 1";
				const char *capture_image = str.c_str();
				system(capture_image);


				// call python program to upload image to cloud storage bucket
				string image_string = "python3 UploadFile.py " + filename + ".jpg 452imagefiles";
				const char *image_upload = image_string.c_str();
				system(image_upload);

				// tag data as speeder
				speeder = "Y";
			}
			else
				speeder = "N";

			// Determine direction
			string direction;
			if(speed_value < 0)
				direction = "Outbound";
			else
				direction = "Inbound";

			// Format and output JSON file
			string json = "{\n  \"Speed\": ";
			if(string_value[0] == '-')
				string_value.erase(0,1);
			json += string_value;
			json += ",\n  \"Direction\": \"";
			json += direction;
			json += "\",\n  \"Date\": \"";
			for (int i = 0; i < 10; ++i)
				json += date_char[i];
			json += "\",\n  \"Time\": \"";
			for (int i = 0; i < 8; ++i)
				json += time_char[i];
			json += "\",\n  \"Speeder\": \"";
			json += speeder;
			json += "\",\n  \"ImageFileName\": \"";
			if(speeder == "Y"){
				json += filename;
				json += ".jpg\"\n}";
			}
			else
				json += "not applicable\n}";

			filename += ".json";

			//exception handling for file output
			try {

			//open file for writing
				ofstream outputFile;
				outputFile.open(filename);

				//check if file was successfully opened for writing
				if (outputFile.is_open()){
					int len = json.length();

					//store string contents in file
					for (int i = 0; i < len; i++) {
						outputFile << json[i];
					}
					outputFile.close();
				}
				else printf("OutputFile failed to open");
			}
			catch (const char* msg) {
				cerr << msg << endl;
			}

			// call python program to upload json to cloud storage bucket
			string str_json_upload = "python3 UploadFile.py " + filename + " 452files_unprocessed";
			const char *json_upload = str_json_upload.c_str();
			system(json_upload);

		}

		tcflush(serial_port, TCIFLUSH);
	}
	return 0;
}

//============================================================================
// References:
//
// How to read data from the RPi USB serial port 
// mbedded.ninja Blog		
// "Linux Serial Ports Using C/C++"
// https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/
//
//============================================================================
