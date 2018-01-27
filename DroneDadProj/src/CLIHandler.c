/*
 * CLIHandler.c
 *
 * Created: 1/24/2018 7:34:10 PM
 *  Author: John
 */ 

#include "asf.h"

#include <string.h>

#include "CLIHandler.h"
#include "Version.h"

const char* CMD_HELP = "help";
const char* CMD_VER_BL = "ver_bl";
const char* CMD_VER_APP = "ver_app";
const char* CMD_GPIO_SET = "gpio_set";
const char* CMD_GPIO_CLEAR = "gpio_clear";
const char* CMD_GPIO_GET = "gpio_get";
const char* CMD_MAC = "mac";
const char* CMD_IP = "ip";
const char* CMD_READ_GYRO = "read_gyro";
const char* CMD_READ_ACCEL = "read_accel";
const char* CMD_ADC_GET = "adc_get";
const char* CMD_MCU_TEMP = "mcu_temp";
const char* CMD_I2C_SCAN = "i2c_scan";

void init_cmd_list() {
	cmd_list = (struct option_list) {
		{{ CMD_HELP, "", "Display all commands" } ,
		{ CMD_VER_BL, "", "Print bootloader firmware information." } ,
		{ CMD_VER_APP, "", "Print application version information." } ,
		{ CMD_GPIO_SET, "[port] [pin_num]", "Set GPIO pin at a given port high." } ,
		{ CMD_GPIO_CLEAR, "[port] [pin_num]", "Set GPIO pin at a given port low." } ,
		{ CMD_GPIO_GET, "[port] [pin_num]", "Get state of a GPIO pin." } ,
		{ CMD_MAC, "", "Print the MAC address." } ,
		{ CMD_IP, "", "Print the IP Address." } ,
		{ CMD_READ_GYRO, "[reading count] [interval in ms]", "Read from the Gyroscope sensor." } ,
		{ CMD_READ_ACCEL, "[reading count] [interval in ms]", "Read from the Accelerometer sensor." } ,
		{ CMD_ADC_GET, "[port] [pin_num]", "Get the ADC value of an input pin." } ,
		{ CMD_MCU_TEMP, "", "Print the temperature reading of the on-board MCU temperature sensor." } ,
		{ CMD_I2C_SCAN, "", "Print out list of addresses of I2C devices on bus." }}
	};
}

// TODO: If arg1 or arg2 are non-digit, we need to throw an error.
void handle_user_input(char* input) {
	char* cmd = strtok(input, " ");
	
	if(strcmp(CMD_HELP, cmd) == 0) { 
		handle_help(); 
	}
	else if(strcmp(CMD_VER_BL, cmd) == 0) { 
		handle_ver_bl(); 
	}
	else if(strcmp(CMD_VER_APP, cmd) == 0) { 
		handle_ver_app();
	}
	else if(strcmp(CMD_GPIO_SET, cmd) == 0) { 
		char* arg1 = strtok(NULL, " ");
		char* arg2 = strtok(NULL, " ");
		int port = atoi(arg1);
		int pin_num = atoi(arg2);
		
		if(arg1 == 0 || arg2 == 0) {
			printf("Not enough arguments! Try \"help\".\r\n");
			return;
		}
		
		handle_gpio_set(port, pin_num);
	}
	else if(strcmp(CMD_GPIO_CLEAR, cmd) == 0) { 
		char* arg1 = strtok(NULL, " ");
		char* arg2 = strtok(NULL, " ");
		int port = atoi(arg1);
		int pin_num = atoi(arg2);
		
		if(arg1 == 0 || arg2 == 0) {
			printf("Not enough arguments! Try \"help\".\r\n");
			return;
		}
		
		handle_gpio_clear(port, pin_num); 
	}
	else if(strcmp(CMD_GPIO_SET, cmd) == 0) {
		char* arg1 = strtok(NULL, " ");
		char* arg2 = strtok(NULL, " ");
		
		if(arg1 == 0 || arg2 == 0) {
			printf("Not enough arguments! Try \"help\".\r\n");
			return;
		}

		int port = arg1;
		int pin_num = atoi(arg2);

		if ((strcmp(port,"A") != 0) || (strcmp(port,"B") != 0)){
			printf("Enter a valid Port! Try \"help\".\r\n");
		}

		if (pin_num <= 0 && pin_num > 32) {
			printf("Enter a valid Pin Number! Try \"help\".\r\n");
		}

		handle_gpio_set(port, pin_num);
	}
	else if(strcmp(CMD_MAC, cmd) == 0) { 
		handle_mac(); 
	}
	else if(strcmp(CMD_IP, cmd) == 0) { 
		handle_ip(); 
	}
	else if(strcmp(CMD_READ_GYRO, cmd) == 0) {
		char* arg1 = strtok(NULL, " ");
		char* arg2 = strtok(NULL, " ");
		int num_readings = atoi(arg1);
		int interval_ms = atoi(arg2);
		
		if(arg1 == 0 || arg2 == 0) {
			printf("Not enough arguments! Try \"help\".\r\n");
			return;
		}
		
		handle_read_gyro(num_readings, interval_ms); 
	}
	else if(strcmp(CMD_READ_ACCEL, cmd) == 0) {
		char* arg1 = strtok(NULL, " ");
		char* arg2 = strtok(NULL, " ");
		int num_readings = atoi(arg1);
		int interval_ms = atoi(arg2);
		
		if(arg1 == 0 || arg2 == 0) {
			printf("Not enough arguments! Try \"help\".\r\n");
			return;
		}
		
		handle_read_accel(num_readings, interval_ms); 
	}
	else if(strcmp(CMD_ADC_GET, cmd) == 0) {
		char* arg1 = strtok(NULL, " ");
		char* arg2 = strtok(NULL, " ");
		int port = atoi(arg1);
		int pin_num = atoi(arg2);
		
		if(arg1 == 0 || arg2 == 0) {
			printf("Not enough arguments! Try \"help\".\r\n");
			return;
		}
		
		handle_adc_get(port, pin_num); 
	}
	else if(strcmp(CMD_MCU_TEMP, cmd) == 0) { 
		handle_mcu_temp(); 
	}
	else if(strcmp(CMD_I2C_SCAN, cmd) == 0) { 
		handle_i2c_scan(); 
	}
	else {
		printf("Invalid command! Type \"help\" for a list of available commands.\r\n");
	}
}

void handle_help() {
	printf("Command List:\r\n");
	for(int i = 0; i < OPTION_COUNT; i++) {
		printf("\t%s %s\r\n\t\t%s\r\n", cmd_list.options[i].command, 
			cmd_list.options[i].params,
			cmd_list.options[i].description);
	}
}


void handle_ver_bl() {
	printf("Bootloader Version: %s\r\n", BL_VERSION_STRING);
}

void handle_ver_app() {
	printf("Application Version: %s\r\n", APP_VERSION_STRING);
}

void handle_gpio_set(int port, int pin_num) {
	// TODO: Must implement this feature for at least two pins.
	printf("Not implemented yet!\r\n");
}

void handle_gpio_clear(int port, int pin_num) {
	// TODO: Must implement this feature for at least two pins.
	printf("Not implemented yet!\r\n");
}

void handle_gpio_get(int port, int pin_num) {
	// TODO: Must implement this feature for at least two pins.
	printf("Not implemented yet!\r\n");
}

void handle_mac() {
	char* dummy_mac = "01:23:45:67:89:ab";
	printf("MAC Address: %s\r\n", dummy_mac);
}

void handle_ip() {
	char* dummy_ip = "255.255.255.255";
	printf("IP Address: %s\r\n", dummy_ip);
}

void handle_read_gyro(int num_readings, int interval_ms) {
	printf("Reading from Gyro...\r\n");
	// Dummy data
	for(int i = 0; i < num_readings; i++) {
		printf("%d\r\n", 9);
		delay_ms(interval_ms);
	}
	printf("Done!\r\n");
}

void handle_read_accel(int num_readings, int interval_ms) {
	printf("Reading from Accelerometer...\r\n");
	// Dummy data
	for(int i = 0; i < num_readings; i++) {
		printf("%d\r\n", 8);
		delay_ms(interval_ms);
	}
	printf("Done!\r\n");
}

void handle_adc_get(int port, int pin_num) {
	// TODO: Must implement this feature for at least one pin.
	printf("Not implemented yet!\r\n");
}

void handle_mcu_temp() {
	printf("Not implemented yet!\r\n");
}

void handle_i2c_scan() {
	printf("Not implemented yet!\r\n");
}
