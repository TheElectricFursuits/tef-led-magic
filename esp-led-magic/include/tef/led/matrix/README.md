
# TEF HUB75 Matrix Driver

The TEF HUB75 Matrix Driver code is meant to drive any standard 1:8 or 1:16 scan ratio LED Matrix with a HUB75 output.
It utilizes DMA to realize efficient driving of the panels, and features an easy to use interface to draw onto the 
panel through the TEF LED GenericMatrix class.

CPU load on a Cortex M7 at 216MHz for 4-bit Colour at approx. 120Hz refresh rate was 0.5% for a 1:8 scanrate display.
The load will double for 1:16 scanrate, but is independent of the length of the panel due to the use of DMA!

Memory requirement is a little high, at roughly 2 * colour depth * width * (height/2), but well manageable for larger cores. All
memory is statically allocated, no dynamic memory is needed.

**Note:** For now this code only works on the STM32F7 DMA. The F4-Series needs a bit of porting, but adjusting it shouldn't be impossible ^^

## Minimum requirements

This code does not take care of setting everything up, and it was meant for one specific set of hardware!
Having said that, this is what you will need:

- One DMA Stream on the same bus as the GPIO (usually DMA2). If it is not on the same APB line, it will not be able to access
  the GPIO registers and won't work! ((This fact is, sadly, not mentioned much in the documentation)).
- One advanced Timer with repetition counter and PWM output.
  - This timer *must* be able to output a DMA trigger to the DMA stream mentioned above (usually something on DMA2) via the
    TIM_DMA_CCx event.
- One regular timer with one PWM output and update interrupt.

*Note:* I highly recommend using the STM32CubeIDE or some other configuration tool. Their Device Code Configuration tool
makes it very easy to check which DMA outputs the timers can have, and to auto-generate most of the configuration.

## Basic setup

If your chip meets the minimum requirements mentioned above, then I have news for you: You'll... Have to set everything up.
In order to support different hardware or timer configurations and to make it less likely for this code to mess up other things, my code
does the bare minimum to the hardware registers:
- It starts the DMA and Clock Timer
- It sets the latch and row-select pins

And... That's it.

That means that before this can work, you will need to do the following (presumably via a configurator like STM32CubeHAL):
- You will need one periodic timer to generate the screen refresh ISR:
  - Its frequency should very roughly be: FPS * (2<<colour depth) * Scan-Height (8 or 16 usually), but if the screen seems flickery 
    it's safe to increase this quite a bit.
  - It is recommended to use this timer to also generate a output enable signal to dim the screen and blank it during the row-switch. The OE on most HUB75 is active low,
    so if your timer can generate a short "high" pulse beginning at the screen refresh ISR, that's perfect. This way you can also limit the screen brightness
    very cheaply, by just varying the duty cycle!
- One Advanced Timer for clock generation
  - It's clock speed should be high. Like, 10MHz for longer screens, but usually 1-2MHz is enough.
  - It will need one PWM output, set to 50% duty cycle. Not sure which clock polarity is necessary, switch it around if the display is flickering a lot.
  - Activate "One Shot Mode", this will auto-disable the timer, rather than the code needing to do it via another ISR.
- One DMA Stream, configured Memory to Peripheral, with its trigger set to the PWM output signal of the aforementioned timer (For example TIM1_CH1)
  - Peripheral Memory address will not increase, the memory address should be set to increase. Both addresses will be configured by this library.
- And a handfull of GPIO channels for outputs:
  - One *byte-aligned* set of 8 GPIO channels, this means Pins 0-7, 8-15, 16-23 or 24-31. Configure the first six of those as Push-Pull output. 
    These will be the data outputs for the "data_ptr" config option.
  - Four adjacent GPIOs for the row select. Needn't be aligned to anything. Configure them to Push-Pull GPIO, set "rowselect_reg" to their register, "rowselect_shift"
    to the number of the first rowselect GPIO.
  - One single Latch output, also Push-Pull. Pass its register and the GPIO *bitmask* (i.e. GPIO_NUM_3), not the shift, to 'latch_reg' and 'latch_pin'
  
Once you're done with all this, just set up the `HUB75_conf_t` structure, then pass it into the constructor of your Matrix Driver!
Call the `handler.Timer_IRQHandler()` function from inside the periodic timer's update interrupt, then use the `set_colour` functions to draw. 
Once you drew an entire frame, make sure to call `switch_frame()` to latch your changes (this is the double-buffering used to prevent artefacting).

Oh, and make sure to enable the Clock Timer's Break output (timer->BDTR |= 1<<15 or similar!), as well as the fitting DMA Channel and PWM outputs (DIER |= TIM_DMA_CCx; CCER |= TIM_CHANNEL_x),
and activate your period timer's ISR (probably via `__HAL_TIM_ENABLE_IT(&timer, TIM_IT_UPDATE)`

Again, I left most of that to the application code so that it can interact with other code elements better without causing mayhem.
