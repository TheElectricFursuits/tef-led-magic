/*
 * Matrix.h
 *
 *  Created on: 23 Jun 2020
 *      Author: xasin
 */

#ifndef TEF_LED_MAGIC_INCLUDE_TEF_LED_MATRIX_MATRIX_H_
#define TEF_LED_MAGIC_INCLUDE_TEF_LED_MATRIX_MATRIX_H_

#include "MatrixFrameBuffer.h"
#include <tef/led/GenericMatrix.h>

#include "main.h"


// TODO Check the version of DMA and
// add a few porting macros. STM32F4 HAL code seems to have a different
// DMA unit :/

////////  NOTE: THIS WILL ONLY WORK WITH STM32F7 CODE FOR NOW
#ifdef HAL_DMA_MODULE_ENABLED

namespace TEF {
namespace LED {
namespace Matrix {

struct HUB75_conf_t {
	DMA_Stream_TypeDef * dma_stream;
	DMA_TypeDef  * dma;
	uint32_t       dma_lifcr;

	TIM_TypeDef  * clk_timer;

	GPIO_TypeDef * latch_gpio;
	uint32_t       latch_pin;

	GPIO_TypeDef * rowselect_gpio;
	uint32_t       rowselect_shift;

	uint32_t       data_ptr;	// Pointer, cast to uint32_t, to the GPIO ODR register

	volatile uint32_t * duty_register;
	uint32_t	   duty_max;
};

struct HUB75_row_output_params_t {
	int physical_row;
	int bit_number;
	int on_time_div;
};

template<int c_height, int c_width, int c_depth, int c_full_drawn_depth>
class HUB75 : public GenericMatrix {
private:
	std::array<FrameBuffer<c_height, c_width, c_depth>, 2> frames;
	std::array<Colour, c_height*c_width> colour_buffer;

	bool frame_no;

	int virt_row_counter;

	const HUB75_conf_t config;

private:
	uint8_t previous_rowselect;

	static constexpr int total_virtual_bitdepth() {
		return (1<<c_full_drawn_depth) - 1 + c_depth - c_full_drawn_depth;
	}

	static constexpr int total_virtual_rows() {
		return (c_height/2) * total_virtual_bitdepth();
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshift-count-negative"

	template<int bpm_bitdepth>
	uint8_t calculate_bpm(int virtual_bit_no) {
		uint32_t slice_bits  = virtual_bit_no + 1;

		int i = bpm_bitdepth-1;
		for(uint32_t mask = 1 << (bpm_bitdepth-1); mask != 0; mask >>= 1) {
			if(mask & slice_bits)
				return i;

			i--;
		}

		return 0;
	}

#pragma GCC diagnostic pop

	inline HUB75_row_output_params_t calculate_row_params(uint32_t virtual_row_number) {
		int physical_row_no = virtual_row_number / total_virtual_bitdepth();
		int virtual_bit_no  = virtual_row_number - physical_row_no * total_virtual_bitdepth();

		// In this setting, the virtual row is shown for only a fractional part
		if(virtual_bit_no < (c_depth - c_full_drawn_depth)) {
			return HUB75_row_output_params_t({
				physical_row_no,
				virtual_bit_no,
				c_depth - c_full_drawn_depth - virtual_bit_no,
			});
		}
		else {
			return HUB75_row_output_params_t({
				physical_row_no,
				c_depth - c_full_drawn_depth + calculate_bpm<c_full_drawn_depth>(virtual_bit_no - c_depth + c_full_drawn_depth),
				0
			});
		}
	}

	inline void IRQ_Set_Rowout(int physical_row_no) {
		const uint8_t row_select_mask = (c_height == 16 ? 0b111 : 0b1111);

		uint8_t next_rowselect = physical_row_no & row_select_mask;
		if(next_rowselect == previous_rowselect)
			return;

		previous_rowselect = next_rowselect;

		config.rowselect_gpio->ODR &= ~(row_select_mask << config.rowselect_shift);
		config.rowselect_gpio->ODR |=  next_rowselect << config.rowselect_shift;
	}

	inline void IRQ_Config_DMA(uint32_t row_address) {
		while(config.dma_stream->CR & DMA_SxCR_EN) {}

		config.dma->LIFCR = config.dma_lifcr;

		config.dma_stream->NDTR = c_width + 1;
		config.dma_stream->PAR  = config.data_ptr;

		config.dma_stream->M0AR = row_address;

		config.dma_stream->CR |= 1;
	}

	inline void IRQ_Start_CLK() {
		config.clk_timer->RCR  = c_width - 1;
		config.clk_timer->CR1 |= 1;
	}

public:
	HUB75(const HUB75_conf_t &config) :
		GenericMatrix(c_width, c_height),
		frames(), frame_no(0), config(config), previous_rowselect(0)
	{ }

	void init() {
		for(unsigned int i=0; i<frames.size(); i++)
			frames[i].clear();
	}

	void Timer_IRQHandler() {
		config.latch_gpio->ODR |= config.latch_pin;

		config.dma_stream->CR &= ~(1);

		IRQ_Set_Rowout(virt_row_counter / total_virtual_bitdepth());

		if(++virt_row_counter >= total_virtual_rows())
			virt_row_counter = 0;

		auto rdc = calculate_row_params(virt_row_counter);

		IRQ_Config_DMA(frames[frame_no ? 1 : 0].row_start_address(rdc.physical_row, rdc.bit_number));
		*config.duty_register = config.duty_max >> rdc.on_time_div;

		config.latch_gpio->ODR &= ~config.latch_pin;

		IRQ_Start_CLK();
	}

	void switch_frame() {
		frame_no = !frame_no;
	}

	void clear() {
		frames[frame_no ? 0 : 1].clear();

		for(auto &c : colour_buffer) c = 0;
	}

	void set_colour(int x, int y, const Colour &c) {
		if(x < 0 || x >= c_width)
			return;
		if(y < 0 || y >= c_height)
			return;

		frames[frame_no ? 0 : 1].set_colour(x, y, c);
		colour_buffer[x + y * c_width] = c;
	}

	void overlay_colour(int x, int y, const Colour &c) {
		if(x < 0 || x >= c_width)
			return;
		if(y < 0 || y >= c_height)
			return;

		if(c.alpha <= 0.1)
			return;
		else if(c.alpha >= 0.99)
			set_colour(x, y, c);
		else {
			Colour temp = colour_buffer[x + y * c_width];
			temp.merge_overlay(c);

			set_colour(x, y, temp);
		}
	}
};


}
}
}

#endif

#endif /* TEF_LED_MAGIC_INCLUDE_TEF_LED_MATRIX_MATRIX_H_ */
