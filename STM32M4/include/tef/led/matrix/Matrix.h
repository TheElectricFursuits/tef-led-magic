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

#ifdef HAL_DMA_MODULE_ENABLED

namespace TEF {
namespace LED {
namespace Matrix {


// TODO write a nice readme doc!!
struct HUB75_conf_t {
	DMA_Stream_TypeDef * dma_stream;
	DMA_TypeDef  * dma;
	uint32_t       dma_lifcr;

	TIM_TypeDef  * clk_timer;

	GPIO_TypeDef * latch_gpio;
	uint32_t       latch_pin;

	GPIO_TypeDef * rowselect_gpio;
	uint32_t       rowselect_shift;

	uint32_t       data_ptr;
};

template<int c_height, int c_width, int c_depth>
class HUB75 : public GenericMatrix {
private:
	std::array<FrameBuffer<c_height, c_width, c_depth>, 2> frames;
	bool frame_no;

	int virt_row_counter;

	const HUB75_conf_t config;

private:
	inline void IRQ_Set_Rowout(int row_num) {
		const uint8_t row_select_mask = (c_height == 16 ? 0b1111 : 0b111);

		config.rowselect_gpio->ODR &= ~(row_select_mask << config.rowselect_shift);
		config.rowselect_gpio->ODR |= (row_num & row_select_mask) << config.rowselect_shift;
	}

	inline void IRQ_Config_DMA() {
		while(config.dma_stream->CR & DMA_SxCR_EN) {}

		config.dma->LIFCR = config.dma_lifcr;

		config.dma_stream->NDTR = c_width + 1;
		config.dma_stream->PAR  = config.data_ptr;

		config.dma_stream->M0AR = frames[frame_no ? 1 : 0].row_start_address(virt_row_counter);

		config.dma_stream->CR |= 1;
	}

	inline void IRQ_Start_CLK() {
		config.clk_timer->RCR  = c_width - 1;
		config.clk_timer->CR1 |= 1;
	}

public:
	HUB75(const HUB75_conf_t &config) :
		GenericMatrix(c_width, c_height),
		frames(), frame_no(0), config(config)
	{ }

	void init() {
		for(unsigned int i=0; i<frames.size(); i++)
			frames[i].clear();
	}

	void Timer_IRQHandler() {
		config.latch_gpio->ODR |= config.latch_pin;

		config.dma_stream->CR &= ~(1);

		IRQ_Set_Rowout(virt_row_counter);

		if(++virt_row_counter >= FrameBuffer<c_width, c_height, c_depth>::virtual_row_count())
			virt_row_counter = 0;

		IRQ_Config_DMA();

		config.latch_gpio->ODR &= ~config.latch_pin;

		IRQ_Start_CLK();
	}

	void switch_frame() {
		frame_no = !frame_no;
	}

	void clear() {
		frames[frame_no ? 0 : 1].clear();
	}

	void set_colour(int x, int y, const ColourGradient &c) {
		frames[frame_no ? 0 : 1].set_colour(x, y, c.get_colour_at(x));
	}
};


}
}
}

#endif

#endif /* TEF_LED_MAGIC_INCLUDE_TEF_LED_MATRIX_MATRIX_H_ */
