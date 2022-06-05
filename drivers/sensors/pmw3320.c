#include "spi_master.h"
#include "pmw3320.h"
#include "wait.h"
#include "debug.h"
#include "print.h"

#define CPISTEP 250

// Registers
// clang-format off
#define REG_Product_ID  	  0x00
#define REG_Revision_ID 	  0x01
#define REG_Motion  		  0x02
#define REG_Delta_X 		  0x03 //modify lib to fit shorter deltas.
//Delta_X_L, Delta_X_H, Delta_Y_H, Delta_Y_L
#define REG_Delta_Y 		  0x04
#define REG_SQUAL 			  0x05
#define REG_Shutter_Upper     0x06
#define REG_Shutter_Lower     0x07
#define REG_Maximum_Raw_data  0x08
#define REG_Raw_Data_Avg      0x09
#define REG_Minimum_Raw_data  0x0A
#define REG_Pix_Grab 		  0x0B
#define REG_DeltaXY_U4b		  0x0C
#define REG_Resolution        0x0D 
#define REG_Run_Downshift 	  0x0e
#define REG_Rest1_Period  	  0x0f
#define REG_Rest1_Downshift   0x10
#define REG_Rest2_Period      0x11
#define REG_Rest2_Downshift   0x12
#define REG_Rest3_Period 	  0x13
#define REG_MIN_SQUAL_RUN	  0x17
#define REG_Axis_Control	  0x1a //orientation register. address 0x0d at PMW3360. Steps of 90 degrees clockwise.
//looking at the elecom when it's headed right: normal is: X <^ Y. MSB steps are 110, 011, 101.
#define REG_Performance 	  0x22 //no worky
#define REG_Low_Mot_Jit		  0x23
#define REG_Shut_Max_HI		  0x36
#define REG_Shut_Max_LO		  0x37
#define REG_Framerate 		  0x39
#define REG_Power_Up_Reset    0x3A
#define REG_Shutdown 		  0x3b
#define REG_NOT_Rev_ID        0x3F
#define REG_Motion_Control    0x41
#define REG_Burst_Read_First  0x42
#define REG_Rest_Mode_Status  0x45
#define REG_NOT_Product_ID    0x4F
#define REG_Motion_Burst      0x63

// clang-format on
//int8_t xa[3];
//int8_t ya[3];
uint8_t burstbuffer[12] = {0};
// limits to 0--13, resulting in a CPI range of 250 -- 3500 (as only steps of 250 are possible).
#ifndef MAX_CPI
#    define MAX_CPI 0x0E
#endif

bool _inBurst = false;
int8_t i;
uint16_t aax, aay;
int8_t ax, ay;
#ifdef CONSOLE_ENABLE
void print_byte(uint8_t byte) {
    uprintf("%c%c%c%c%c%c%c%c|", (byte & 0x80 ? '1' : '0'), (byte & 0x40 ? '1' : '0'), (byte & 0x20 ? '1' : '0'), (byte & 0x10 ? '1' : '0'), (byte & 0x08 ? '1' : '0'), (byte & 0x04 ? '1' : '0'), (byte & 0x02 ? '1' : '0'), (byte & 0x01 ? '1' : '0')); //dprintf
}
#endif
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

#define BEGIN_COM writePinLow(PMW3320_CS_PIN); wait_us(1)
#define END_COM   wait_us(1); writePinHigh(PMW3320_CS_PIN)

bool pmw3320_spi_start(void) {
    bool status = spi_start(PMW3320_CLOCK_SPEED, PMW3320_CS_PIN, PMW3320_SPI_LSBFIRST, PMW3320_SPI_MODE, PMW3320_SPI_DIVISOR);
    // tNCS-SCLK, 120ns
    wait_us(1);
    return status;
}

spi_status_t pmw3320_write(uint8_t reg_addr, uint8_t data) {
    pmw3320_spi_start();

    if (reg_addr != REG_Motion_Burst) {
        _inBurst = false;
    }

    // send address of the register, with MSBit = 1 to indicate it's a write
    spi_status_t status = spi_write(reg_addr | 0x80);
    status              = spi_write(data);

    // tSCLK-NCS for write operation is 35us
    wait_us(35);
    spi_stop();

    // tSWW/tSWR (=180us) minus tSCLK-NCS. Could be shortened, but is looks like a safe lower bound
    wait_us(145);
    return status;
}

uint8_t pmw3320_read(uint8_t reg_addr) {
	if(reg_addr != REG_Motion_Burst)
	{
	_inBurst = false;
	}
    pmw3320_spi_start();
    // send adress of the register, with MSBit = 0 to indicate it's a read
    spi_write(reg_addr & 0x7f);
    // tSRAD (=160us)
    wait_us(160);
    uint8_t data = spi_read();

    // tSCLK-NCS for read operation is 120ns
    wait_us(1);
    spi_stop();

    //  tSRW/tSRR (=20us) minus tSCLK-NCS
    wait_us(19);
    return data;
}

bool pmw3320_init(void) {
    _inBurst = false;
    //setPinOutput(PMW3320_CS_PIN);
    //writePinHigh(PMW3320_CS_PIN);
    spi_init();

    pmw3320_spi_start();
    END_COM;
	BEGIN_COM;
    END_COM;

    pmw3320_write(REG_Shutdown, 0xb6); // Shutdown first
	spi_stop();
	wait_ms(300);
	
    pmw3320_spi_start();
	BEGIN_COM;
    wait_us(40);
    spi_stop();
    wait_us(40);

    // power up, need to first drive NCS high then low, see above.
    pmw3320_write(REG_Power_Up_Reset, 0x5a);
	spi_stop();
    wait_ms(50);
	
	pmw3320_spi_start();
    // read registers and discard
    pmw3320_read(REG_Motion);
    pmw3320_read(REG_Delta_X);
    pmw3320_read(REG_Delta_Y);
    
    spi_stop();
	
    wait_ms(10);	
    pmw3320_set_cpi(PMW3320_CPI);
    wait_ms(10);
	pmw3320_write(REG_Axis_Control, 0b10100000);
    //pmw3320_write(REG_Config2, 0x00);
	
    //pmw3320_write(REG_Angle_Tune, constrain(ROTATIONAL_TRANSFORM_ANGLE, -127, 127));

    //pmw3320_write(REG_Lift_Config, PMW3320_LIFTOFF_DISTANCE);
	uint8_t prodID = pmw3320_read(REG_Product_ID);
    bool init_success = (prodID == 0x3b);
	#ifdef CONSOLE_ENABLE
		if (init_success) {
			xprintf("pmw3320 ok");
		} else {
			xprintf("pmw3320 not ok! %x\n", prodID);
		}
	#endif

    return init_success;
}

uint16_t pmw3320_get_cpi(void) {
    uint8_t cpival = pmw3320_read(REG_Resolution);
    return (uint16_t)(((cpival + 1) & 0xFF) * CPISTEP);
}

void pmw3320_set_cpi(uint16_t cpi) {
    uint8_t cpival = constrain((cpi / CPISTEP) - 1, 0, MAX_CPI);
    pmw3320_write(REG_Resolution, cpival);
	
}

report_pmw3320_t pmw3320_read_burst(void) {
	report_pmw3320_t report = {0};
	//int8_t xyH = 0;
    if (!_inBurst) {
		#ifdef CONSOLE_ENABLE
			//uprintf("burst on");
		#endif
        pmw3320_write(REG_Burst_Read_First, REG_Motion);
        _inBurst = true;
    }

	i = 0;
    pmw3320_spi_start();
    while(spi_write(REG_Motion_Burst) == SPI_STATUS_TIMEOUT);
    wait_us(35); // waits for tSRAD_MOTBR
	if(spi_receive(burstbuffer, 3) != SPI_STATUS_SUCCESS)
		xprintf("OHSHITNIGGA\n");
	wait_us(5);
	spi_stop();
	report.motion = (int8_t)burstbuffer[0];
	ax = (int8_t)burstbuffer[1];
	ay = (int8_t)burstbuffer[2];
	
	/*
    report.motion = spi_read();
    ax = spi_read();
	ay = spi_read();
	while (++i < 7)
		spi_read();
	*/
	//xyH = spi_read();
    report.mdx = 0;//xyH >> 4;//spi_read();
    report.mdy = 0;//xyH & 0x0F;//spi_read();
	if(ax < -127) ax = -127;
	if(ay < -127) ay = -127;
	report.dx  = ax;
    report.dy  = ay;
    if (report.motion & 0b111) { // panic recovery, sometimes burst mode works weird.
        _inBurst = false;
    }

    
	
	while(++i < 10) burstbuffer[i] = 0;
	
	#ifdef CONSOLE_ENABLE
		if (debug_mouse && ((report.motion & 0x80) != 0) && ((report.motion & 0x08) == 0)) {
			//print_byte(xyH);
			xprintf("%d | ", ax);
			//print_byte(report.mdx);//useless
			xprintf("%d", ay);
			//print_byte(report.mdy);//useless
			//uprintf("%d, %d\n", report.dx, report.dy);
			uprintf("\n");//should be dprintf.
		}
	#endif
	
    report.isMotion    = (report.motion & 0x80) != 0;
    report.isOnSurface = (report.motion & 0x08) == 0;
    //report.dx |= (report.mdx << 8);
    //report.dx = report.dx * -1;
    //report.dy |= (report.mdy << 8);
    //report.dy = report.dy * -1;
	
    return report;
}
