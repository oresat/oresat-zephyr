#include "protocard.h"

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
int blinky(void)
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
		return 0;
	}

  while (1) 
  {
   	while (1) 
    {
      ret = gpio_pin_toggle_dt(&led);
      if (ret < 0) 
      {
        return 0;
      }

      led_state = !led_state;
      printk("LED state: %s\n", led_state ? "ON" : "OFF");
      k_msleep(SLEEP_TIME_MS);
	  }
  }

}

void blinky_entry(void *unused0, void *unused1, void *unused2)
{
  (void) unused0;
  (void) unused1;
  (void) unused2;

  blinky();
}

K_THREAD_STACK_DEFINE(blinky_stack_area, BLINKY_STACK_SIZE);
K_THREAD_DEFINE(
  blinky_tid, BLINKY_STACK_SIZE,
  blinky_entry, NULL, NULL, NULL,
  BLINKY_PRIORITY, 0, 0
);

int startThreads(void)
{
  k_thread_start(blinky_tid);

  return 0;
}
