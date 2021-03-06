/*
 * LEDDriver.c
 *
 * Created: 4/9/2018 9:30:08 PM
 *  Author: John
 */ 

/*
Register Table

Register  Name         Function
0x0       Input 1      RO LED0-7 Input Register
0x1       Reg. 1       RO None
0x2       PSC0         RW Frequency Prescaler 1
0x3       PWM0         RW PWM Register 0 (Duty Cycle)
0x4       PSC1         RW Frequency Prescaler 2
0x5       PWM1         RW PWM Register 1 (Duty Cycle)
0x6       LS0          RW LED0-3 Selector
0x7       LS1          RW LED4-7 Selector
0x8       Reg. 8       RW None
0x9       Reg. 9       RW Nonw
*/

#include <asf.h>

// Our I2C bus requires a delay after each write.
#define DELAY_VALUE_US 50

#define LED_OFF  0x0
#define LED_ON   0x1
#define LED_DIM0 0x2
#define LED_DIM1 0x3

#define LED_BANK_1_REG 0x6
#define LED_BANK_2_REG 0x7

#define LED_0_OFFSET 0x00
#define LED_1_OFFSET 0x02
#define LED_2_OFFSET 0x04
#define LED_3_OFFSET 0x06
#define LED_4_OFFSET 0x00
#define LED_5_OFFSET 0x02
#define LED_6_OFFSET 0x04
#define LED_7_OFFSET 0x06

#define DIM0_REG  0x2
#define DUTY0_REG 0x3

#define DIM1_REG  0x4
#define DUTY1_REG 0x5

#define MAX_DIM_PERIOD 1600
#define MAX_PERCENTAGE 100

#define LP3944_SLAVE_ADDR 0x60
#define LP3944_WRITE_DATA_LENGTH 2
#define LP3944_READ_DATA_LENGTH 1
#define LP3944_REG_ADDR_FIELD 0
#define LP3944_DATA_FIELD 1

typedef struct {
	uint8_t led_bank_reg;
	uint8_t led_offset;
} lp3944_led_data;

const lp3944_led_data LP3944_LED0 = {
	.led_bank_reg = LED_BANK_1_REG,
	.led_offset = LED_0_OFFSET
};

const lp3944_led_data LP3944_LED1 = {
	.led_bank_reg = LED_BANK_1_REG,
	.led_offset = LED_1_OFFSET
};

const lp3944_led_data LP3944_LED2 = {
	.led_bank_reg = LED_BANK_1_REG,
	.led_offset = LED_2_OFFSET
};

const lp3944_led_data LP3944_LED3 = {
	.led_bank_reg = LED_BANK_1_REG,
	.led_offset = LED_3_OFFSET
};

const lp3944_led_data LP3944_LED4 = {
	.led_bank_reg = LED_BANK_2_REG,
	.led_offset = LED_4_OFFSET
};

const lp3944_led_data LP3944_LED5 = {
	.led_bank_reg = LED_BANK_2_REG,
	.led_offset = LED_5_OFFSET
};

const lp3944_led_data LP3944_LED6 = {
	.led_bank_reg = LED_BANK_2_REG,
	.led_offset = LED_6_OFFSET
};

const lp3944_led_data LP3944_LED7 = {
	.led_bank_reg = LED_BANK_2_REG,
	.led_offset = LED_7_OFFSET
};

struct i2c_master_module i2c_master_instance;
void configure_i2c(void)
{
	/* Initialize config structure and software module */
	struct i2c_master_config config_i2c_master;
	i2c_master_get_config_defaults(&config_i2c_master);
	/* Change buffer timeout to something longer */
	config_i2c_master.buffer_timeout    = 65535;
	config_i2c_master.pinmux_pad0       = PINMUX_PA22C_SERCOM3_PAD0;
	config_i2c_master.pinmux_pad1       = PINMUX_PA23C_SERCOM3_PAD1;
	config_i2c_master.generator_source  = GCLK_GENERATOR_0;
	/* Initialize and enable device with config */
	while(i2c_master_init(&i2c_master_instance, SERCOM3, &config_i2c_master) != STATUS_OK);
	i2c_master_enable(&i2c_master_instance);
}


static uint8_t lp3944_wr_buffer[LP3944_WRITE_DATA_LENGTH];
static struct i2c_master_packet lp3944_wr_packet = {
	.address          = LP3944_SLAVE_ADDR,
	.data_length      = LP3944_WRITE_DATA_LENGTH,
	.data             = lp3944_wr_buffer,
	.ten_bit_address  = false,
	.high_speed       = false,
	.hs_master_code   = 0x00,
};

static uint8_t lp3944_rd_buffer[1];
static struct i2c_master_packet lp3944_rd_packet = {
	.address          = LP3944_SLAVE_ADDR,
	.data_length      = 1,
	.data             = lp3944_rd_buffer,
	.ten_bit_address  = false,
	.high_speed       = false,
	.hs_master_code   = 0x00,
};

void _lp3944_i2c_write(uint8_t addr, uint8_t value){
	enum status_code i2c_status;
	lp3944_wr_packet.address = LP3944_SLAVE_ADDR;
	lp3944_wr_packet.data_length = LP3944_WRITE_DATA_LENGTH;
	lp3944_wr_buffer[LP3944_REG_ADDR_FIELD] = addr;
	lp3944_wr_buffer[LP3944_DATA_FIELD] = value;
	lp3944_wr_packet.data = lp3944_wr_buffer;
	//delay_us(DELAY_VALUE_US);
	i2c_status = i2c_master_write_packet_wait(&i2c_master_instance, &lp3944_wr_packet);
	delay_us(DELAY_VALUE_US);
	if(i2c_status != STATUS_OK) {
		// Uhoh
		printf("Failure in %s when writing to i2c (%d)", __func__, i2c_status);
	}
}

void _lp3944_i2c_read(uint8_t addr, uint8_t* value) {
	enum status_code i2c_status;
	lp3944_wr_packet.address = LP3944_SLAVE_ADDR;
	lp3944_wr_packet.data_length = LP3944_READ_DATA_LENGTH;
	lp3944_wr_buffer[LP3944_REG_ADDR_FIELD] = addr;
	lp3944_wr_packet.data = lp3944_wr_buffer;
	//delay_us(DELAY_VALUE_US);
	i2c_status = i2c_master_write_packet_wait_no_stop(&i2c_master_instance, &lp3944_wr_packet);
	delay_us(DELAY_VALUE_US);
	if(i2c_status != STATUS_OK) {
		//uhoh
		int i = 0;
	}
	else {
		lp3944_rd_packet.address = LP3944_SLAVE_ADDR;
		lp3944_rd_packet.data_length = LP3944_READ_DATA_LENGTH;
		//delay_us(DELAY_VALUE_US);
		i2c_status = i2c_master_read_packet_wait_no_stop(&i2c_master_instance, &lp3944_rd_packet);
		delay_us(DELAY_VALUE_US);
		if(i2c_status != STATUS_OK) {
			// Uhoh
			int i = 0;
		}
		int i = 0;
	}

	i2c_master_send_stop(&i2c_master_instance);
	
	delay_us(1);
	
	(*value) = lp3944_rd_packet.data[0];
}

void set_led(lp3944_led_data led, uint8_t value) {
	uint8_t new_value = 0;
	uint8_t previous_value = 0;
	
	// Get the previous configuration
	_lp3944_i2c_read(led.led_bank_reg, &previous_value);
	
	// Clear previous value for LED.
	previous_value &= ~(0x3 << led.led_offset);
	// Store new value for LED
	new_value = (value << led.led_offset) | previous_value;
	
	// Write the updated configuration.
	_lp3944_i2c_write(led.led_bank_reg, new_value);
}

void _set_duty_cycle(uint8_t percentage, uint8_t pwm_reg) {
	if(percentage > MAX_PERCENTAGE) {
		// Not allowed.
		return;
	}
	
	uint8_t reg_value = ((percentage * 256) / 100); // See datasheet
	_lp3944_i2c_write(pwm_reg, reg_value);
}

void set_duty_cycle_0(uint8_t percentage) {
	_set_duty_cycle(percentage, DUTY0_REG);
}

void set_duty_cycle_1(uint8_t percentage) {
	_set_duty_cycle(percentage, DUTY1_REG);
}

void _set_dim_rate(uint16_t dim_period_ms, uint8_t dim_reg) {
	if(dim_period_ms > MAX_DIM_PERIOD) {
		// Not allowed
		return;
	}
	
	// We cast down to a uin8_t from uin16_t.
	// This is safe because our max input is 1600, which equates to a reg_value of 255
	uint8_t reg_value = (((dim_period_ms * 160) / 1000) - 1); // See datasheet
	_lp3944_i2c_write(dim_reg, reg_value);
}

void set_dim_period_0(uint16_t dim_period_ms) {
	_set_dim_rate(dim_period_ms, DIM0_REG);
}

void set_dim_period_1(uint16_t dim_period_ms) {
	_set_dim_rate(dim_period_ms, DIM1_REG);
}

void lp3944_reset() {
	port_pin_set_output_level(PIN_PB02, false);
	port_pin_set_output_level(PIN_PB02, true);
}

void lp3944_init() {
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(PIN_PB02, &config_port_pin);
	port_pin_set_output_level(PIN_PB02, true);
}