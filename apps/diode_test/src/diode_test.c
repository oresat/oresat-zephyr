#include "diode_test.h"
#include "CANopen.h"
#include "OD.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/dt-bindings/gpio/gpio.h>
#include <zephyr/drivers/dac.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

DTC dtc = { 0 };

//sample_t sample[SAMPLES];
#define BUFFER_DEPTH            sizeof(sample)/sizeof(sample_t)

#define ZEPHYR_USER_NODE DT_PATH(zephyr_user)

#if (DT_NODE_HAS_PROP(ZEPHYR_USER_NODE, dac) && \
	DT_NODE_HAS_PROP(ZEPHYR_USER_NODE, dac_channel_id) && \
	DT_NODE_HAS_PROP(ZEPHYR_USER_NODE, dac_resolution))
#define DAC_NODE DT_PHANDLE(ZEPHYR_USER_NODE, dac)
#define DAC_CHANNEL_ID DT_PROP(ZEPHYR_USER_NODE, dac_channel_id)
#define DAC_RESOLUTION DT_PROP(ZEPHYR_USER_NODE, dac_resolution)
#else
#error "Unsupported board: see README and check /zephyr,user node"
#define DAC_NODE DT_INVALID_NODE
#define DAC_CHANNEL_ID 0
#define DAC_RESOLUTION 0
#endif

#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
	!DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

// Data of ADC io-channels specified in devicetree. 
static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
			     DT_SPEC_AND_COMMA)
};

/*
 * dtc_callCtrlThreadFunctions
 * gets called every time the ctrl_thread polls OD dtc_ctrl 
 */
int dtc_callCtrlThreadFunctions(void)
{
  
  // TODO: call these functions based on status object
  dtc_muxSelect();
  dtc_dacSet();

  return 0;
}

/*
 * dtc_init
 * initializes diode test card
 */
int dtc_init(void)
{
  // fill array with function pointers
  // indeces are used to call functions from the OD ctrl
  // dtc.pfunc[MAX_FUNCTIONS] is initialized to NULL
  dtc.pfunc[1] = &dtc_dacStart;
  dtc.pfunc[2] = &dtc_dacStop;
  dtc.pfunc[3] = &dtc_gptStart;
  dtc.pfunc[4] = &dtc_gptStop;
  dtc.pfunc[5] = &dtc_adcStart;
  dtc.pfunc[6] = &dtc_adcStop;
  dtc.pfunc[7] = &dtc_muxEnable;
  dtc.pfunc[8] = &dtc_muxDisable;
  dtc.pfunc[9] = &dtc_clearErrors;
  //        ^  lastFunctionIndex

  int i = 1; // first function in dtc.pfunc
  // determine last index of functions 
  for(; *dtc.pfunc[i] && i < MAX_FUNCTIONS; ++i){}
  dtc.lastFunctionIndex = i;

  // create pointers to OD objects
  dtc.pctrl = &OD_RAM.x4000_dtc.ctrl;
  dtc.pmux_select = &OD_RAM.x4000_dtc.mux_select;
  dtc.pdac = &OD_RAM.x4000_dtc.dac;
  dtc.pstatus = &OD_RAM.x4000_dtc.status;
  dtc.perror = &OD_RAM.x4000_dtc.error; 

  dtc.pled_current = &OD_RAM.x4001_adcsample.led_current;
  dtc.pled_swir_pd_current = &OD_RAM.x4001_adcsample.led_swir_pd_current;
  dtc.puv_pd_current = &OD_RAM.x4001_adcsample.uv_pd_current;
 // dtc.ptsen = &OD_RAM.x3003_system.temperature; // wtf!!!
  
  dtc.padcsample = &OD_RAM.x4001_adcsample.led_current;

  // start drivers
  dtc_dacStart();
  dtc_adcStart();
  dtc_gptStart();
  return 0;
}

/*
 * DAC configuration
 */
static const struct device *const dac_dev = DEVICE_DT_GET(DAC_NODE);

static const struct dac_channel_cfg dac_ch_cfg = {
	.channel_id  = DAC_CHANNEL_ID,
	.resolution  = DAC_RESOLUTION,
	.buffered = true
};

/*
 *  dtc_dacStart 
 *  wrapper that calls the ChibiOS version  
 */
int dtc_dacStart(void)
{
  if (!device_is_ready(dac_dev)) {
		printk("DAC device %s is not ready\n", dac_dev->name);
		return 1;
	}

	int ret = dac_channel_setup(dac_dev, &dac_ch_cfg);

	if (ret != 0) {
		printk("Setting up of DAC channel failed with code %d\n", ret);
		return ret;
	}

	printk("Generating sawtooth signal at DAC channel %d.\n", DAC_CHANNEL_ID);
  
  return 0;
}

/*
 * dtc_dacStop
 * wrapper that calls the ChibiOS version
 */
int dtc_dacStop(void)
{
  //dacStop(&DACD1);

  return 0;
}

/*
 * dtc_dacSet
 * sets the dac with the value contained in the OD dtc_dac object 
 */
int dtc_dacSet(void)
{
  if(*dtc.pdac <= 0xFFF)
  {
 //   osalSysLock();
			int ret = dac_write_value(dac_dev, DAC_CHANNEL_ID, *dtc.pdac);
      if (ret != 0) {
				printk("dac_write_value() failed with code %d\n", ret);
				return ret;
			}
//	    printk("writing %d to dac.\n", *dtc.pdac);
 //   osalSysUnlock();
  }
  else
  {
    //TODO: set bit in error object
  }

  return 0;
}



/*
  adcStart(&ADCD1, NULL);
  if(ADCD1.state == ADC_READY)
  {
    (*dtc.pstatus) |= (1 << CTRL_ADC_EN); // sets the status bit
    adcSTM32SetCCR(ADC_CCR_TSEN); // enable temperature sensor
    adcStartConversion(&ADCD1, &adcgrpcfg1, (adcsample_t *)sample, BUFFER_DEPTH);
  }
  else
  {
    (*dtc.perror) = (*dtc.perror) | ERROR_ADC_START;
  }
  return 0;
}
//*/

/*
 * dtc_adcStop
 * wrapper for ChibiOS gpt driver function calls
 */
int dtc_adcStop(void)
{
  /*
  adcStop(&ADCD1);
  if(ADCD1.state == ADC_STOP)
  {
    (*dtc.pstatus) &= ~(1 << CTRL_ADC_EN);
  }
  else
  {
    (*dtc.perror) = (*dtc.perror) | ERROR_ADC_STOP;
  }
  //*/
  return 0;
}

/*
 * dtc_clearErrors
 * clears all errors
 */
int dtc_clearErrors(void)
{
  (*dtc.perror) = 0;

  return 0;
}

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static int blinky(void)
{
  int ret;
	bool led_state = true;

	if (!gpio_is_ready_dt(&led)) 
  {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) 
  {
		return 1;
	}

  while (1) 
  {
    ret = gpio_pin_toggle_dt(&led);
    if (ret < 0) 
    {
      return 1;
    }

    led_state = !led_state;
  //  printk("LED state: %s\n", led_state ? "ON" : "OFF");
    k_msleep(SLEEP_TIME_MS);
  }

}

static void blinky_entry(void *unused0, void *unused1, void *unused2)
{
  (void) unused0;
  (void) unused1;
  (void) unused2;

  if(blinky())
  {
    printk("ERROR: blinky thread exited!");
  }
}

K_THREAD_STACK_DEFINE(blinky_stack_area, BLINKY_STACK_SIZE);
K_THREAD_DEFINE(
  blinky_tid, BLINKY_STACK_SIZE,
  blinky_entry, NULL, NULL, NULL,
  BLINKY_PRIORITY, 0, 0
);

int startBlinkyThread(void)
{
  k_thread_start(blinky_tid);

  return 0;
}

/*
 * ADC streaming callback
 * writes to the OD on completion of an adc sample
 */

#define CHANNEL_COUNT ARRAY_SIZE(adc_channels)
#define BUFFER_ARRAY_LEN 4

uint16_t channel_reading[BUFFER_ARRAY_LEN][CHANNEL_COUNT];
uint16_t adcsample[BUFFER_ARRAY_LEN];
uint16_t temp_buf[BUFFER_ARRAY_LEN];
uint32_t cb_count = 0;
uint8_t sample_count = 0;

static enum adc_action adc_callback(
    const struct device *dev, 
    const struct adc_sequence *sequence, 
    uint16_t sampling_index
)
{
  (void)dev;
  (void)sampling_index;
  (void)sequence;;

  ++cb_count;

  return ADC_ACTION_CONTINUE;
  //return ADC_ACTION_REPEAT;
  //return ADC_ACTION_FINISH
}

const struct adc_sequence_options options = {
  .callback = adc_callback,
  .extra_samplings = BUFFER_ARRAY_LEN - 1,
//    .interval_us = 100 * 1000UL,
  .interval_us = 0
};

struct adc_sequence sequence = {
  .buffer = adcsample,
  .buffer_size = sizeof(adcsample),
  .options = &options,
};

int work_counter = 0;
void adc_work_handler(struct k_work *work)
{
  (void)work;
  int err = 0;

  ++work_counter;
  
  for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) 
  {
    (void)adc_sequence_init_dt(&adc_channels[i], &sequence);
    temp_buf[i] = adcsample[i];
    err = adc_read_dt(&adc_channels[i], &sequence);
    if (err < 0) 
    {
      printk("Could not read (%d)\n", err);
    }
  }

}

K_WORK_DEFINE(adc_work, adc_work_handler);

void adc_timer_handler(struct k_timer *dummy)
{
  (void)dummy;

  k_work_submit(&adc_work);
}

K_TIMER_DEFINE(adc_timer, adc_timer_handler, NULL);

/*
 * dtc_gptStart
 * wrapper for ChibiOS gpt driver function calls
 */

int dtc_gptStart(void)
{
  k_timer_start(&adc_timer, K_MSEC(200), K_MSEC(200));
  (*dtc.pstatus) |= (1 << CTRL_GPT_EN); // sets the status bit
  
  return 0;
}
/*
 * dtc_gptStop
 * wrapper for ChibiOS gpt driver function calls
 */
int dtc_gptStop(void)
{
  (*dtc.pstatus) &= ~(1 << CTRL_GPT_EN); // clears the status bit
  
  return 0;
}

/*
 * dtc_adcStart
 * wrapper for ChibiOS gpt driver function calls
 */
int dtc_adcStart(void)
{
  int err;

  printk("\nADC buffer size: %d\n", sequence.buffer_size);

  for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) 
  {
    if (!adc_is_ready_dt(&adc_channels[i])) 
    {
      printk("ADC controller device %s not ready\n", adc_channels[i].dev->name);
      return 0;
    }

    printk("ADC controller device %s ready!\n", adc_channels[i].dev->name);
  
    sequence.channels |=  BIT(adc_channels[i].channel_id);
  
    err = adc_channel_setup_dt(&adc_channels[i]);
    if (err < 0) 
    {
      printk("Could not setup channel #%d (%d)\n", i, err);
      return 0;
    }
    printk("Successfullly setup channel #%d\n", i);
  }

  //k_timer_start(&adc_timer, K_MSEC(200), K_MSEC(200));
  
  return 0;
}

/*
 * diode test card watch thread
 *
 * watches dtc OD objects and dumps them to serial output
 */
static int watch(void)
{
  
  while (1) 
  {
    printk( "\r\nctrl:                  %04u \r\n", *dtc.pctrl);
    printk(     "mux_select:            %04u \r\n", *dtc.pmux_select);
    printk(     "dac:                   %04u \r\n", *dtc.pdac);
    printk(     "status:              0x%04X \r\n", *dtc.pstatus);
    printk(     "error:               0x%04X \r\n", *dtc.perror);
    printk(     "led_current:           %04u \r\n", *dtc.pled_current);
    printk(     "led_swir_pd_current:   %04u \r\n", *dtc.pled_swir_pd_current);
    printk(     "uv_pd_current:         %04u \r\n", *dtc.puv_pd_current);
    printk(     "tsen:                  %04u \r\n", *dtc.ptsen);
    printk(     "cb count:              %04u \r\n", cb_count);
    printk(     "work counter:          %04u \r\n", work_counter);
    printk(     "adc:    ");

    for(uint8_t j = 0; j < BUFFER_ARRAY_LEN; ++j)
    {
      printk("%04u ", temp_buf[j]);
    }
    printk("\r\n");
   
    k_msleep(SLEEP_TIME_MS * 5);
  }

  return 0;
}

static void watch_entry(void *unused0, void *unused1, void *unused2)
{
  (void) unused0;
  (void) unused1;
  (void) unused2;

  if(watch())
  {
    printk("ERROR: watch thread exited!");
  }
}

K_THREAD_STACK_DEFINE(watch_stack_area, WATCH_STACK_SIZE);
K_THREAD_DEFINE(
  watch_tid, WATCH_STACK_SIZE,
  watch_entry, NULL, NULL, NULL,
  WATCH_PRIORITY, 0, 0
);

int startWatchThread(void)
{
  k_thread_start(watch_tid);

  return 0;
}

/*
 * dtc_muxEnable
 * enables the diode mux (logic low enable)
 */
//static const struct gpio_dt_spec mux_en = GPIO_DT_SPEC_GET(MUX_EN_NODE, gpios);
int  dtc_muxEnable(void)
{ 
  //palClearPad(GPIOB, DTC_MUX_EN); // logic low enable
  (*dtc.pstatus) |= (1 << CTRL_MUX_EN);
  return 0;
}
/*
 * dtc_muxDisable
 * disables the diode mux (logic high disable)
 */
int dtc_muxDisable(void)
{
  //palSetPad(GPIOB, DTC_MUX_EN); // logic high disable
  (*dtc.pstatus) &= ~(1 << CTRL_MUX_EN);
  
  return 0;
}

/*
 * dtc_muxSelect
 * select desired diode (0 - 7)
 */
int dtc_muxSelect(void)
{
 if(*dtc.pmux_select < DTC_NUM_DIODES)
  {
    //osalSysLock();
    /*
    palWriteGroup(
      GPIOB, 
      PAL_PORT_BIT(DTC_MUX_A0) | PAL_PORT_BIT(DTC_MUX_A1) | PAL_PORT_BIT(DTC_MUX_A2), 
      0, 
      (*dtc.pmux_select << DTC_MUX_A0)
    );
    //*/
    *dtc.pstatus = (*dtc.pstatus & (~CTRL_MUX_MASK)) | (*dtc.pmux_select << CTRL_MUX_A0);
   // osalSysUnlock();
  }

 return 0;
}

/*
 * app control thread 
 */

static int control(void)
{
  dtc_muxEnable();  
  
  while (1) 
  {
    // TODO: make this thread interrupt driven instead of polling
    // might need interrupts for changes in mux_select and dac as well
    if(*dtc.pctrl > 0) 
    {
      if(*dtc.pctrl <= dtc.lastFunctionIndex)
      {
        dtc.pfunc[*dtc.pctrl]();
      }
      else
      { 
        // TODO: set error bit in OD
      }
      
      *dtc.pctrl = 0;
    }
    
    dtc_callCtrlThreadFunctions();

    k_msleep(200);
  }

  return 0;
}

static void control_entry(void *unused0, void *unused1, void *unused2)
{
  (void) unused0;
  (void) unused1;
  (void) unused2;

  if(control())
  {
    printk("ERROR: watch thread exited!");
  }
}

K_THREAD_STACK_DEFINE(control_stack_area, CONTROL_STACK_SIZE);
K_THREAD_DEFINE(
  control_tid, CONTROL_STACK_SIZE,
  control_entry, NULL, NULL, NULL,
  CONTROL_PRIORITY, 0, 0
);

int startControlThread(void)
{
  k_thread_start(control_tid);

  return 0;
}

