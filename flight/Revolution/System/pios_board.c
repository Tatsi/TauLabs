/**
 ******************************************************************************
 * @addtogroup Revolution Revolution configuration files
 * @{
 * @brief Configures the revolution board
 * @{
 *
 * @file       pios_board.c
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2011.
 * @brief      Defines board specific static initializers for hardware for the Revolution board.
 * @see        The GNU Public License (GPL) Version 3
 *
 *****************************************************************************/
/* 
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 3 of the License, or 
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License 
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along 
 * with this program; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <pios.h>

#if defined(PIOS_INCLUDE_SPI)

#include <openpilot.h>
#include <pios_spi_priv.h>
#include <uavobjectsinit.h>
#include <hwsettings.h>
#include "manualcontrolsettings.h"

/* SPI1 Interface
 *    - Used for BMA180 accelerometer
 */
void PIOS_SPI_accel_irq_handler(void);
void DMA2_Stream0_IRQHandler(void) __attribute__((alias("PIOS_SPI_accel_irq_handler")));
void DMA2_Stream3_IRQHandler(void) __attribute__((alias("PIOS_SPI_accel_irq_handler")));
static const struct pios_spi_cfg pios_spi_accel_cfg = {
	.regs = SPI1,
	.remap = GPIO_AF_SPI1,
	.init   = {
		.SPI_Mode              = SPI_Mode_Master,
		.SPI_Direction         = SPI_Direction_2Lines_FullDuplex,
		.SPI_DataSize          = SPI_DataSize_8b,
		.SPI_NSS               = SPI_NSS_Soft,
		.SPI_FirstBit          = SPI_FirstBit_MSB,
		.SPI_CRCPolynomial     = 7,
		.SPI_CPOL              = SPI_CPOL_High,
		.SPI_CPHA              = SPI_CPHA_2Edge,
		.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4,
	},
	.use_crc = false,
	.dma = {
		.irq = {
			.flags   = (DMA_IT_TCIF0 | DMA_IT_TEIF0 | DMA_IT_HTIF0),
			.init    = {
				.NVIC_IRQChannel                   = DMA2_Stream0_IRQn,
				.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGH,
				.NVIC_IRQChannelSubPriority        = 0,
				.NVIC_IRQChannelCmd                = ENABLE,
			},
		},
		
		.rx = {
			.channel = DMA2_Stream0,
			.init    = {
                .DMA_Channel            = DMA_Channel_3,
				.DMA_PeripheralBaseAddr = (uint32_t)&(SPI1->DR),
				.DMA_DIR                = DMA_DIR_PeripheralToMemory,
				.DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
				.DMA_MemoryInc          = DMA_MemoryInc_Enable,
				.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
				.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,
				.DMA_Mode               = DMA_Mode_Normal,
				.DMA_Priority           = DMA_Priority_Medium,
				.DMA_FIFOMode           = DMA_FIFOMode_Disable,
                /* .DMA_FIFOThreshold */
                .DMA_MemoryBurst        = DMA_MemoryBurst_Single,
                .DMA_PeripheralBurst    = DMA_PeripheralBurst_Single,
			},
		},
		.tx = {
			.channel = DMA2_Stream3,
			.init    = {
                .DMA_Channel            = DMA_Channel_3,
				.DMA_PeripheralBaseAddr = (uint32_t)&(SPI1->DR),
				.DMA_DIR                = DMA_DIR_MemoryToPeripheral,
				.DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
				.DMA_MemoryInc          = DMA_MemoryInc_Enable,
				.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
				.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,
				.DMA_Mode               = DMA_Mode_Normal,
				.DMA_Priority           = DMA_Priority_High,
				.DMA_FIFOMode           = DMA_FIFOMode_Disable,
                /* .DMA_FIFOThreshold */
                .DMA_MemoryBurst        = DMA_MemoryBurst_Single,
                .DMA_PeripheralBurst    = DMA_PeripheralBurst_Single,
			},
		},
	},
	.sclk = {
		.gpio = GPIOA,
		.init = {
			.GPIO_Pin   = GPIO_Pin_5,
			.GPIO_Speed = GPIO_Speed_100MHz,
			.GPIO_Mode  = GPIO_Mode_AF,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_UP
		},
	},
	.miso = {
		.gpio = GPIOA,
		.init = {
			.GPIO_Pin   = GPIO_Pin_6,
			.GPIO_Speed = GPIO_Speed_50MHz,
			.GPIO_Mode  = GPIO_Mode_AF,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_UP
		},
	},
	.mosi = {
		.gpio = GPIOA,
		.init = {
			.GPIO_Pin   = GPIO_Pin_7,
			.GPIO_Speed = GPIO_Speed_50MHz,
			.GPIO_Mode  = GPIO_Mode_AF,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_UP
		},
	},
	.slave_count = 1,
	.ssel = { {
		.gpio = GPIOA,
		.init = {
			.GPIO_Pin   = GPIO_Pin_4,
			.GPIO_Speed = GPIO_Speed_50MHz,
			.GPIO_Mode  = GPIO_Mode_OUT,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_UP
		},
	} },
};

static uint32_t pios_spi_accel_id;
void PIOS_SPI_accel_irq_handler(void)
{
	/* Call into the generic code to handle the IRQ for this specific device */
	PIOS_SPI_IRQ_Handler(pios_spi_accel_id);
}


/* SPI2 Interface
 *      - Used for gyro communications
 */
void PIOS_SPI_GYRO_irq_handler(void);
void DMA1_Stream3_IRQHandler(void) __attribute__((alias("PIOS_SPI_gyro_irq_handler")));
void DMA1_Stream4_IRQHandler(void) __attribute__((alias("PIOS_SPI_gyro_irq_handler")));
static const struct pios_spi_cfg pios_spi_gyro_cfg = {
	.regs = SPI2,
	.remap = GPIO_AF_SPI2,
	.init = {
		.SPI_Mode              = SPI_Mode_Master,
		.SPI_Direction         = SPI_Direction_2Lines_FullDuplex,
		.SPI_DataSize          = SPI_DataSize_8b,
		.SPI_NSS               = SPI_NSS_Soft,
		.SPI_FirstBit          = SPI_FirstBit_MSB,
		.SPI_CRCPolynomial     = 7,
		.SPI_CPOL              = SPI_CPOL_High,
		.SPI_CPHA              = SPI_CPHA_2Edge,
		.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2,
	},
	.use_crc = false,
	.dma = {
		.irq = {
			// Note this is the stream ID that triggers interrupts (in this case RX)
			.flags = (DMA_IT_TCIF3 | DMA_IT_TEIF3 | DMA_IT_HTIF3),
			.init = {
				.NVIC_IRQChannel = DMA1_Stream3_IRQn,
				.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGH,
				.NVIC_IRQChannelSubPriority = 0,
				.NVIC_IRQChannelCmd = ENABLE,
			},
		},
		
		.rx = {
			.channel = DMA1_Stream3,
			.init = {
				.DMA_Channel            = DMA_Channel_0,
				.DMA_PeripheralBaseAddr = (uint32_t) & (SPI2->DR),
				.DMA_DIR                = DMA_DIR_PeripheralToMemory,
				.DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
				.DMA_MemoryInc          = DMA_MemoryInc_Enable,
				.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
				.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,
				.DMA_Mode               = DMA_Mode_Normal,
				.DMA_Priority           = DMA_Priority_Medium,
				//TODO: Enable FIFO
				.DMA_FIFOMode           = DMA_FIFOMode_Disable,
                .DMA_FIFOThreshold      = DMA_FIFOThreshold_Full,
                .DMA_MemoryBurst        = DMA_MemoryBurst_Single,
                .DMA_PeripheralBurst    = DMA_PeripheralBurst_Single,
			},
		},
		.tx = {
			.channel = DMA1_Stream4,
			.init = {
				.DMA_Channel            = DMA_Channel_0,
				.DMA_PeripheralBaseAddr = (uint32_t) & (SPI2->DR),
				.DMA_DIR                = DMA_DIR_MemoryToPeripheral,
				.DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
				.DMA_MemoryInc          = DMA_MemoryInc_Enable,
				.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
				.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,
				.DMA_Mode               = DMA_Mode_Normal,
				.DMA_Priority           = DMA_Priority_Medium,
				.DMA_FIFOMode           = DMA_FIFOMode_Disable,
                .DMA_FIFOThreshold      = DMA_FIFOThreshold_Full,
                .DMA_MemoryBurst        = DMA_MemoryBurst_Single,
                .DMA_PeripheralBurst    = DMA_PeripheralBurst_Single,
			},
		},
	},
	.sclk = {
		.gpio = GPIOB,
		.init = {
			.GPIO_Pin = GPIO_Pin_13,
			.GPIO_Speed = GPIO_Speed_100MHz,
			.GPIO_Mode = GPIO_Mode_AF,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_NOPULL
		},
	},
	.miso = {
		.gpio = GPIOB,
		.init = {
			.GPIO_Pin = GPIO_Pin_14,
			.GPIO_Speed = GPIO_Speed_50MHz,
			.GPIO_Mode = GPIO_Mode_AF,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_NOPULL
		},
	},
	.mosi = {
		.gpio = GPIOB,
		.init = {
			.GPIO_Pin = GPIO_Pin_15,
			.GPIO_Speed = GPIO_Speed_50MHz,
			.GPIO_Mode = GPIO_Mode_AF,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_NOPULL
		},
	},
	.slave_count = 1,
	.ssel = { {
		.gpio = GPIOB,
		.init = {
			.GPIO_Pin = GPIO_Pin_12,
			.GPIO_Speed = GPIO_Speed_50MHz,
			.GPIO_Mode  = GPIO_Mode_OUT,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_UP
		},
	} },
};

uint32_t pios_spi_gyro_id;
void PIOS_SPI_gyro_irq_handler(void)
{
	/* Call into the generic code to handle the IRQ for this specific device */
	PIOS_SPI_IRQ_Handler(pios_spi_gyro_id);
}


/* SPI3 Interface
 *      - Used for gyro communications
 */
void PIOS_SPI_flash_irq_handler(void);
void DMA1_Stream0_IRQHandler(void) __attribute__((alias("PIOS_SPI_flash_irq_handler")));
void DMA1_Stream2_IRQHandler(void) __attribute__((alias("PIOS_SPI_flash_irq_handler")));
static const struct pios_spi_cfg pios_spi_flash_cfg = {
	.regs = SPI3,
	.remap = GPIO_AF_SPI3,
	.init = {
		.SPI_Mode              = SPI_Mode_Master,
		.SPI_Direction         = SPI_Direction_2Lines_FullDuplex,
		.SPI_DataSize          = SPI_DataSize_8b,
		.SPI_NSS               = SPI_NSS_Soft,
		.SPI_FirstBit          = SPI_FirstBit_MSB,
		.SPI_CRCPolynomial     = 7,
		.SPI_CPOL              = SPI_CPOL_High,
		.SPI_CPHA              = SPI_CPHA_2Edge,
		.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2,
	},
	.use_crc = false,
	.dma = {
		.irq = {
			// Note this is the stream ID that triggers interrupts (in this case RX)
			.flags = (DMA_IT_TCIF3 | DMA_IT_TEIF3 | DMA_IT_HTIF3),
			.init = {
				.NVIC_IRQChannel = DMA1_Stream0_IRQn,
				.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGH,
				.NVIC_IRQChannelSubPriority = 0,
				.NVIC_IRQChannelCmd = ENABLE,
			},
		},
		
		.rx = {
			.channel = DMA1_Stream0,
			.init = {
				.DMA_Channel            = DMA_Channel_0,
				.DMA_PeripheralBaseAddr = (uint32_t) & (SPI3->DR),
				.DMA_DIR                = DMA_DIR_PeripheralToMemory,
				.DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
				.DMA_MemoryInc          = DMA_MemoryInc_Enable,
				.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
				.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,
				.DMA_Mode               = DMA_Mode_Normal,
				.DMA_Priority           = DMA_Priority_Medium,
				//TODO: Enable FIFO
				.DMA_FIFOMode           = DMA_FIFOMode_Disable,
                .DMA_FIFOThreshold      = DMA_FIFOThreshold_Full,
                .DMA_MemoryBurst        = DMA_MemoryBurst_Single,
                .DMA_PeripheralBurst    = DMA_PeripheralBurst_Single,
			},
		},
		.tx = {
			.channel = DMA1_Stream2,
			.init = {
				.DMA_Channel            = DMA_Channel_0,
				.DMA_PeripheralBaseAddr = (uint32_t) & (SPI3->DR),
				.DMA_DIR                = DMA_DIR_MemoryToPeripheral,
				.DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
				.DMA_MemoryInc          = DMA_MemoryInc_Enable,
				.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
				.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,
				.DMA_Mode               = DMA_Mode_Normal,
				.DMA_Priority           = DMA_Priority_Medium,
				.DMA_FIFOMode           = DMA_FIFOMode_Disable,
                .DMA_FIFOThreshold      = DMA_FIFOThreshold_Full,
                .DMA_MemoryBurst        = DMA_MemoryBurst_Single,
                .DMA_PeripheralBurst    = DMA_PeripheralBurst_Single,
			},
		},
	},
	.sclk = {
		.gpio = GPIOC,
		.init = {
			.GPIO_Pin = GPIO_Pin_10,
			.GPIO_Speed = GPIO_Speed_100MHz,
			.GPIO_Mode = GPIO_Mode_AF,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_NOPULL
		},
	},
	.miso = {
		.gpio = GPIOC,
		.init = {
			.GPIO_Pin = GPIO_Pin_11,
			.GPIO_Speed = GPIO_Speed_50MHz,
			.GPIO_Mode = GPIO_Mode_AF,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_NOPULL
		},
	},
	.mosi = {
		.gpio = GPIOC,
		.init = {
			.GPIO_Pin = GPIO_Pin_12,
			.GPIO_Speed = GPIO_Speed_50MHz,
			.GPIO_Mode = GPIO_Mode_AF,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_NOPULL
		},
	},
	.slave_count = 1,
	.ssel = { {
		.gpio = GPIOD,
		.init = {
			.GPIO_Pin = GPIO_Pin_2,
			.GPIO_Speed = GPIO_Speed_50MHz,
			.GPIO_Mode  = GPIO_Mode_OUT,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_UP
		},
	} },
};

uint32_t pios_spi_flash_id;
void PIOS_SPI_flash_irq_handler(void)
{
	/* Call into the generic code to handle the IRQ for this specific device */
	PIOS_SPI_IRQ_Handler(pios_spi_flash_id);
}
#endif /* PIOS_INCLUDE_SPI */



#include <pios_usart_priv.h>

#if defined(PIOS_INCLUDE_GPS)
/*
 * GPS USART
 */
static const struct pios_usart_cfg pios_usart_gps_cfg = {
	.regs = USART1,
	.remap = GPIO_AF_USART1,
	.init = {
		.USART_BaudRate = 57600,
		.USART_WordLength = USART_WordLength_8b,
		.USART_Parity = USART_Parity_No,
		.USART_StopBits = USART_StopBits_1,
		.USART_HardwareFlowControl =
		USART_HardwareFlowControl_None,
		.USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
	},
	.irq = {
		.init = {
			.NVIC_IRQChannel = USART1_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_MID,
			.NVIC_IRQChannelSubPriority = 0,
			.NVIC_IRQChannelCmd = ENABLE,
		},
	},
	.rx = {
		.gpio = GPIOA,
		.init = {
			.GPIO_Pin   = GPIO_Pin_10,
			.GPIO_Speed = GPIO_Speed_2MHz,
			.GPIO_Mode  = GPIO_Mode_AF,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd  = GPIO_PuPd_UP
		},
	},
	.tx = {
		.gpio = GPIOA,
		.init = {
			.GPIO_Pin   = GPIO_Pin_9,
			.GPIO_Speed = GPIO_Speed_2MHz,
			.GPIO_Mode  = GPIO_Mode_AF,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd  = GPIO_PuPd_UP
		},
	},
};

#define PIOS_COM_AUX_TX_BUF_LEN 150
static uint8_t pios_com_aux_tx_buffer[PIOS_COM_AUX_TX_BUF_LEN];
#define PIOS_COM_AUX_RX_BUF_LEN 10
static uint8_t pios_com_aux_rx_buffer[PIOS_COM_AUX_RX_BUF_LEN];

#endif /* PIOS_INCLUDE_GPS */

#ifdef PIOS_INCLUDE_COM_AUX
/*
 * AUX USART
 */
static const struct pios_usart_cfg pios_usart_aux_cfg = {
	.regs = UART4,
	.remap = GPIO_AF_UART4,
	.init = {
		.USART_BaudRate = 230400,
		.USART_WordLength = USART_WordLength_8b,
		.USART_Parity = USART_Parity_No,
		.USART_StopBits = USART_StopBits_1,
		.USART_HardwareFlowControl =
		USART_HardwareFlowControl_None,
		.USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
	},
	.irq = {
		.init = {
			.NVIC_IRQChannel = UART4_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGH,
			.NVIC_IRQChannelSubPriority = 0,
			.NVIC_IRQChannelCmd = ENABLE,
		},
	},
	.rx = {
		.gpio = GPIOC,
		.init = {
			.GPIO_Pin = GPIO_Pin_11,
			.GPIO_Speed = GPIO_Speed_2MHz,
			.GPIO_Mode  = GPIO_Mode_AF,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd  = GPIO_PuPd_UP
		},
	},
	.tx = {
		.gpio = GPIOC,
		.init = {
			.GPIO_Pin = GPIO_Pin_10,
			.GPIO_Speed = GPIO_Speed_2MHz,
			.GPIO_Mode  = GPIO_Mode_AF,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd  = GPIO_PuPd_UP
		},
	},
};

#define PIOS_COM_GPS_TX_BUF_LEN 10
static uint8_t pios_com_gps_tx_buffer[PIOS_COM_GPS_TX_BUF_LEN];
#define PIOS_COM_GPS_RX_BUF_LEN 192
static uint8_t pios_com_gps_rx_buffer[PIOS_COM_GPS_RX_BUF_LEN];

#endif /* PIOS_COM_AUX */

#ifdef PIOS_INCLUDE_COM_TELEM
/*
 * Telemetry on main USART
 */
static const struct pios_usart_cfg pios_usart_telem_main_cfg = {
	.regs = USART6,
	.regs = USART1,
	.remap = GPIO_AF_USART1,
	.init = {
		.USART_BaudRate = 57600,
		.USART_WordLength = USART_WordLength_8b,
		.USART_Parity = USART_Parity_No,
		.USART_StopBits = USART_StopBits_1,
		.USART_HardwareFlowControl =
		USART_HardwareFlowControl_None,
		.USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
	},
	.irq = {
		.init = {
			.NVIC_IRQChannel = USART1_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_MID,
			.NVIC_IRQChannelSubPriority = 0,
			.NVIC_IRQChannelCmd = ENABLE,
		},
	},
	.rx = {
		.gpio = GPIOA,
		.init = {
			.GPIO_Pin   = GPIO_Pin_10,
			.GPIO_Speed = GPIO_Speed_2MHz,
			.GPIO_Mode  = GPIO_Mode_AF,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd  = GPIO_PuPd_UP
		},
	},
	.tx = {
		.gpio = GPIOA,
		.init = {
			.GPIO_Pin   = GPIO_Pin_9,
			.GPIO_Speed = GPIO_Speed_2MHz,
			.GPIO_Mode  = GPIO_Mode_AF,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd  = GPIO_PuPd_UP
		},
	},
};

#define PIOS_COM_TELEM_RF_RX_BUF_LEN 512
#define PIOS_COM_TELEM_RF_TX_BUF_LEN 512

#endif /* PIOS_COM_TELEM */


#if defined(PIOS_INCLUDE_COM)

#include <pios_com_priv.h>

#endif /* PIOS_INCLUDE_COM */

#if defined(PIOS_INCLUDE_I2C)

#include <pios_i2c_priv.h>

/*
 * I2C Adapters
 */
void PIOS_I2C_mag_adapter_ev_irq_handler(void);
void PIOS_I2C_mag_adapter_er_irq_handler(void);
void I2C1_EV_IRQHandler()
__attribute__ ((alias("PIOS_I2C_mag_adapter_ev_irq_handler")));
void I2C1_ER_IRQHandler()
__attribute__ ((alias("PIOS_I2C_mag_adapter_er_irq_handler")));

static const struct pios_i2c_adapter_cfg pios_i2c_mag_adapter_cfg = {
	.regs = I2C1,
	.remap = GPIO_AF_I2C1,
	.init = {
		.I2C_Mode = I2C_Mode_I2C,
		.I2C_OwnAddress1 = 0,
		.I2C_Ack = I2C_Ack_Enable,
		.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit,
		.I2C_DutyCycle = I2C_DutyCycle_2,
		.I2C_ClockSpeed = 400000,	/* bits/s */
	},
	.transfer_timeout_ms = 50,
	.scl = {
		.gpio = GPIOB,
		.init = {
			.GPIO_Pin = GPIO_Pin_6,
            .GPIO_Mode  = GPIO_Mode_AF,
            .GPIO_Speed = GPIO_Speed_50MHz,
            .GPIO_OType = GPIO_OType_OD,
            .GPIO_PuPd  = GPIO_PuPd_NOPULL,
		},
	},
	.sda = {
		.gpio = GPIOB,
		.init = {
			.GPIO_Pin = GPIO_Pin_7,
            .GPIO_Mode  = GPIO_Mode_AF,
            .GPIO_Speed = GPIO_Speed_50MHz,
            .GPIO_OType = GPIO_OType_OD,
            .GPIO_PuPd  = GPIO_PuPd_NOPULL,
		},
	},
	.event = {
		.flags = 0,	/* FIXME: check this */
		.init = {
			.NVIC_IRQChannel = I2C1_EV_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGHEST,
			.NVIC_IRQChannelSubPriority = 0,
			.NVIC_IRQChannelCmd = ENABLE,
		},
	},
	.error = {
		.flags = 0,	/* FIXME: check this */
		.init = {
			.NVIC_IRQChannel = I2C1_ER_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGHEST,
			.NVIC_IRQChannelSubPriority = 0,
			.NVIC_IRQChannelCmd = ENABLE,
		},
	},
};

uint32_t pios_i2c_mag_adapter_id;
void PIOS_I2C_mag_adapter_ev_irq_handler(void)
{
	/* Call into the generic code to handle the IRQ for this specific device */
	PIOS_I2C_EV_IRQ_Handler(pios_i2c_mag_adapter_id);
}

void PIOS_I2C_mag_adapter_er_irq_handler(void)
{
	/* Call into the generic code to handle the IRQ for this specific device */
	PIOS_I2C_ER_IRQ_Handler(pios_i2c_mag_adapter_id);
}


void PIOS_I2C_flexiport_adapter_ev_irq_handler(void);
void PIOS_I2C_flexiport_adapter_er_irq_handler(void);
void I2C2_EV_IRQHandler() __attribute__ ((alias ("PIOS_I2C_flexiport_adapter_ev_irq_handler")));
void I2C2_ER_IRQHandler() __attribute__ ((alias ("PIOS_I2C_flexiport_adapter_er_irq_handler")));

static const struct pios_i2c_adapter_cfg pios_i2c_flexiport_adapter_cfg = {
	.regs = I2C2,
	.remap = GPIO_AF_I2C2,
	.init = {
		.I2C_Mode                = I2C_Mode_I2C,
		.I2C_OwnAddress1         = 0,
		.I2C_Ack                 = I2C_Ack_Enable,
		.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit,
		.I2C_DutyCycle           = I2C_DutyCycle_2,
		.I2C_ClockSpeed          = 400000,	/* bits/s */
	},
	.transfer_timeout_ms = 50,
	.scl = {
		.gpio = GPIOB,
		.init = {
			.GPIO_Pin   = GPIO_Pin_10,
            .GPIO_Mode  = GPIO_Mode_AF,
            .GPIO_Speed = GPIO_Speed_50MHz,
            .GPIO_OType = GPIO_OType_OD,
            .GPIO_PuPd  = GPIO_PuPd_NOPULL,
		},
	},
	.sda = {
		.gpio = GPIOB,
		.init = {
			.GPIO_Pin   = GPIO_Pin_11,
            .GPIO_Mode  = GPIO_Mode_AF,
            .GPIO_Speed = GPIO_Speed_50MHz,
            .GPIO_OType = GPIO_OType_OD,
            .GPIO_PuPd  = GPIO_PuPd_NOPULL,
		},
	},
	.event = {
		.flags   = 0,		/* FIXME: check this */
		.init = {
			.NVIC_IRQChannel                   = I2C2_EV_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGHEST,
			.NVIC_IRQChannelSubPriority        = 0,
			.NVIC_IRQChannelCmd                = ENABLE,
		},
	},
	.error = {
		.flags   = 0,		/* FIXME: check this */
		.init = {
			.NVIC_IRQChannel                   = I2C2_ER_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGHEST,
			.NVIC_IRQChannelSubPriority        = 0,
			.NVIC_IRQChannelCmd                = ENABLE,
		},
	},
};

uint32_t pios_i2c_flexiport_adapter_id;
void PIOS_I2C_flexiport_adapter_ev_irq_handler(void)
{
	/* Call into the generic code to handle the IRQ for this specific device */
	PIOS_I2C_EV_IRQ_Handler(pios_i2c_flexiport_adapter_id);
}

void PIOS_I2C_flexiport_adapter_er_irq_handler(void)
{
	/* Call into the generic code to handle the IRQ for this specific device */
	PIOS_I2C_ER_IRQ_Handler(pios_i2c_flexiport_adapter_id);
}


void PIOS_I2C_pressure_adapter_ev_irq_handler(void);
void PIOS_I2C_pressure_adapter_er_irq_handler(void);
void I2C3_EV_IRQHandler() __attribute__ ((alias ("PIOS_I2C_pressure_adapter_ev_irq_handler")));
void I2C3_ER_IRQHandler() __attribute__ ((alias ("PIOS_I2C_pressure_adapter_er_irq_handler")));

static const struct pios_i2c_adapter_cfg pios_i2c_pressure_adapter_cfg = {
	.regs = I2C3,
	.remap = GPIO_AF_I2C3,
	.init = {
		.I2C_Mode                = I2C_Mode_I2C,
		.I2C_OwnAddress1         = 0,
		.I2C_Ack                 = I2C_Ack_Enable,
		.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit,
		.I2C_DutyCycle           = I2C_DutyCycle_2,
		.I2C_ClockSpeed          = 40000,	/* bits/s */
	},
	.transfer_timeout_ms = 50,
	.scl = {
		.gpio = GPIOA,
		.init = {
			.GPIO_Pin   = GPIO_Pin_8,
            .GPIO_Mode  = GPIO_Mode_AF,
            .GPIO_Speed = GPIO_Speed_50MHz,
            .GPIO_OType = GPIO_OType_OD,
            .GPIO_PuPd  = GPIO_PuPd_NOPULL,
		},
	},
	.sda = {
		.gpio = GPIOC,
		.init = {
			.GPIO_Pin   = GPIO_Pin_9,
            .GPIO_Mode  = GPIO_Mode_AF,
            .GPIO_Speed = GPIO_Speed_50MHz,
            .GPIO_OType = GPIO_OType_OD,
            .GPIO_PuPd  = GPIO_PuPd_NOPULL,
		},
	},
	.event = {
		.flags   = 0,		/* FIXME: check this */
		.init = {
			.NVIC_IRQChannel                   = I2C3_EV_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGHEST,
			.NVIC_IRQChannelSubPriority        = 0,
			.NVIC_IRQChannelCmd                = ENABLE,
		},
	},
	.error = {
		.flags   = 0,		/* FIXME: check this */
		.init = {
			.NVIC_IRQChannel                   = I2C3_ER_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGHEST,
			.NVIC_IRQChannelSubPriority        = 0,
			.NVIC_IRQChannelCmd                = ENABLE,
		},
	},
};

uint32_t pios_i2c_pressure_adapter_id;
void PIOS_I2C_pressure_adapter_ev_irq_handler(void)
{
	/* Call into the generic code to handle the IRQ for this specific device */
	PIOS_I2C_EV_IRQ_Handler(pios_i2c_pressure_adapter_id);
}

void PIOS_I2C_pressure_adapter_er_irq_handler(void)
{
	/* Call into the generic code to handle the IRQ for this specific device */
	PIOS_I2C_ER_IRQ_Handler(pios_i2c_pressure_adapter_id);
}
#endif /* PIOS_INCLUDE_I2C */

#if defined(PIOS_INCLUDE_RCVR)
#include "pios_rcvr_priv.h"

/* One slot per selectable receiver group.
 *  eg. PWM, PPM, GCS, SPEKTRUM1, SPEKTRUM2, SBUS
 * NOTE: No slot in this map for NONE.
 */
uint32_t pios_rcvr_group_map[MANUALCONTROLSETTINGS_CHANNELGROUPS_NONE];
#endif

extern const struct pios_com_driver pios_usart_com_driver;

uint32_t pios_com_aux_id;
uint32_t pios_com_gps_id;
uint32_t pios_com_telem_usb_id;
uint32_t pios_com_telem_rf_id;


/**
 * Sensor configurations 
 */
#include "pios_hmc5883.h"
static const struct pios_hmc5883_cfg pios_hmc5883_cfg = {
	.drdy = {
		.gpio = GPIOB,
		.init = {
			.GPIO_Pin = GPIO_Pin_5,
			.GPIO_Speed = GPIO_Speed_100MHz,
			.GPIO_Mode = GPIO_Mode_IN,
			.GPIO_OType = GPIO_OType_OD,
			.GPIO_PuPd = GPIO_PuPd_NOPULL,
		},
	},
	.eoc_exti = {
		.pin_source = EXTI_PinSource5,
		.port_source = EXTI_PortSourceGPIOB,
		.init = {
			.EXTI_Line = EXTI_Line5, // matches above GPIO pin
			.EXTI_Mode = EXTI_Mode_Interrupt,
			.EXTI_Trigger = EXTI_Trigger_Rising,
			.EXTI_LineCmd = ENABLE,
		},
	},
	.eoc_irq = {
		.init = {
			.NVIC_IRQChannel = EXTI9_5_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_LOW,
			.NVIC_IRQChannelSubPriority = 0,
			.NVIC_IRQChannelCmd = ENABLE,
		},
	},
	.M_ODR = PIOS_HMC5883_ODR_75,
	.Meas_Conf = PIOS_HMC5883_MEASCONF_NORMAL,
	.Gain = PIOS_HMC5883_GAIN_1_9,
	.Mode = PIOS_HMC5883_MODE_CONTINUOUS,

};

#include "pios_bma180.h"
static const struct pios_bma180_cfg pios_bma180_cfg = {
	.drdy = {
		.gpio = GPIOC,
		.init = {
			.GPIO_Pin = GPIO_Pin_4,
			.GPIO_Speed = GPIO_Speed_100MHz,
			.GPIO_Mode = GPIO_Mode_IN,
			.GPIO_OType = GPIO_OType_OD,
			.GPIO_PuPd = GPIO_PuPd_NOPULL,
		},
	},
	.eoc_exti = {
		.pin_source = EXTI_PinSource4,
		.port_source = EXTI_PortSourceGPIOC,
		.init = {
			.EXTI_Line = EXTI_Line4, // matches above GPIO pin
			.EXTI_Mode = EXTI_Mode_Interrupt,
			.EXTI_Trigger = EXTI_Trigger_Rising,
			.EXTI_LineCmd = ENABLE,
		},
	},
	.eoc_irq = {
		.init = {
			.NVIC_IRQChannel = EXTI4_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_LOW,
			.NVIC_IRQChannelSubPriority = 0,
			.NVIC_IRQChannelCmd = ENABLE,
		},
	},
};

#include "pios_mpu6000.h"
static const struct pios_mpu6000_cfg pios_mpu6000_cfg = {
	.drdy = {
		.gpio = GPIOD,
		.init = {
			.GPIO_Pin = GPIO_Pin_8,
			.GPIO_Speed = GPIO_Speed_100MHz,
			.GPIO_Mode = GPIO_Mode_IN,
			.GPIO_OType = GPIO_OType_OD,
			.GPIO_PuPd = GPIO_PuPd_NOPULL,
		},
	},
	.eoc_exti = {
		.pin_source = EXTI_PinSource8,
		.port_source = EXTI_PortSourceGPIOD,
		.init = {
			.EXTI_Line = EXTI_Line8, // matches above GPIO pin
			.EXTI_Mode = EXTI_Mode_Interrupt,
			.EXTI_Trigger = EXTI_Trigger_Rising,
			.EXTI_LineCmd = ENABLE,
		},
	},
	.eoc_irq = {
		.init = {
			.NVIC_IRQChannel = EXTI9_5_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGH,
			.NVIC_IRQChannelSubPriority = 0,
			.NVIC_IRQChannelCmd = ENABLE,
		},
	},
	.Fifo_store = PIOS_MPU6000_FIFO_TEMP_OUT | PIOS_MPU6000_FIFO_GYRO_X_OUT | PIOS_MPU6000_FIFO_GYRO_Y_OUT | PIOS_MPU6000_FIFO_GYRO_Z_OUT,
	// Clock at 8 khz, downsampled by 8 for 1khz
	.Smpl_rate_div = 7, 
	.interrupt_cfg = PIOS_MPU6000_INT_CLR_ANYRD,
	.interrupt_en = PIOS_MPU6000_INTEN_DATA_RDY,
	.User_ctl = PIOS_MPU6000_USERCTL_FIFO_EN,
	.Pwr_mgmt_clk = PIOS_MPU6000_PWRMGMT_PLL_X_CLK,
	.gyro_range = PIOS_MPU6000_SCALE_500_DEG,
	.filter = PIOS_MPU6000_LOWPASS_256_HZ
};


/**
 * PIOS_Board_Init()
 * initializes all the core subsystems on this specific hardware
 * called from System/openpilot.c
 */
int32_t test_val;
uint8_t buf[4];
uint8_t rec[4];
struct pios_mpu6000_data test_gyro_data;
void PIOS_Board_Init(void) {
	/* Brings up System using CMSIS functions, enables the LEDs. */
	PIOS_SYS_Init();
	
	/* Delay system */
	PIOS_DELAY_Init();

#ifdef PIOS_DEBUG_ENABLE_DEBUG_PINS
	PIOS_DEBUG_Init(&pios_tim_servo_all_channels, NELEMENTS(pios_tim_servo_all_channels));
#endif	/* PIOS_DEBUG_ENABLE_DEBUG_PINS */
	
	
	/* Set up the SPI interface to the flash */
	if (PIOS_SPI_Init(&pios_spi_flash_id, &pios_spi_flash_cfg)) {
		PIOS_DEBUG_Assert(0);
	}
	PIOS_Flash_W25X_Init(pios_spi_flash_id);
	PIOS_FLASHFS_Init();
	
	/* Initialize UAVObject libraries */
	EventDispatcherInitialize();
	UAVObjInitialize();
	
	/* Initialize the alarms library */
	AlarmsInitialize();

	/* Initialize the task monitor library */
	TaskMonitorInitialize();

	/* IAP System Setup */
	PIOS_IAP_Init();
	
#if defined(PIOS_INCLUDE_COM)
#if defined(PIOS_INCLUDE_GPS)

	uint32_t pios_usart_gps_id;
	if (PIOS_USART_Init(&pios_usart_gps_id, &pios_usart_gps_cfg)) {
		PIOS_DEBUG_Assert(0);
	}
	if (PIOS_COM_Init(&pios_com_gps_id, &pios_usart_com_driver, pios_usart_gps_id,
					  pios_com_gps_rx_buffer, sizeof(pios_com_gps_rx_buffer),
					  pios_com_gps_tx_buffer, sizeof(pios_com_gps_tx_buffer))) {
		PIOS_DEBUG_Assert(0);
	}
#endif	/* PIOS_INCLUDE_GPS */
	
#if defined(PIOS_INCLUDE_COM_AUX)
	uint32_t pios_usart_aux_id;

    if (PIOS_USART_Init(&pios_usart_aux_id, &pios_usart_aux_cfg)) {
		PIOS_DEBUG_Assert(0);
	}
	if (PIOS_COM_Init(&pios_com_aux_id, &pios_usart_com_driver, pios_usart_aux_id,
					  pios_com_aux_rx_buffer, sizeof(pios_com_aux_rx_buffer),
					  pios_com_aux_tx_buffer, sizeof(pios_com_aux_tx_buffer))) {
		PIOS_DEBUG_Assert(0);
	}
#endif

#if defined(PIOS_INCLUDE_COM_TELEM)
{ /* Eventually add switch for this port function */
	uint32_t pios_usart_telem_rf_id;
	if (PIOS_USART_Init(&pios_usart_telem_rf_id, &pios_usart_telem_main_cfg)) {
		PIOS_Assert(0);
	}
	
	uint8_t * rx_buffer = (uint8_t *) pvPortMalloc(PIOS_COM_TELEM_RF_RX_BUF_LEN);
	uint8_t * tx_buffer = (uint8_t *) pvPortMalloc(PIOS_COM_TELEM_RF_TX_BUF_LEN);
	PIOS_Assert(rx_buffer);
	PIOS_Assert(tx_buffer);
	if (PIOS_COM_Init(&pios_com_telem_rf_id, &pios_usart_com_driver, pios_usart_telem_rf_id,
					  rx_buffer, PIOS_COM_TELEM_RF_RX_BUF_LEN,
					  tx_buffer, PIOS_COM_TELEM_RF_TX_BUF_LEN)) {
		PIOS_Assert(0);
	}
}
#endif	/* PIOS_INCLUDE_COM_AUX */

#endif	/* PIOS_INCLUDE_COM */
	
	if (PIOS_I2C_Init(&pios_i2c_mag_adapter_id, &pios_i2c_mag_adapter_cfg)) {
		PIOS_DEBUG_Assert(0);
	}

	if (PIOS_I2C_Init(&pios_i2c_pressure_adapter_id, &pios_i2c_pressure_adapter_cfg)) {
		PIOS_DEBUG_Assert(0);
	}
	/* Set up the SPI interface to the accelerometer*/
	if (PIOS_SPI_Init(&pios_spi_accel_id, &pios_spi_accel_cfg)) {
		PIOS_DEBUG_Assert(0);
	}

	/* Set up the SPI interface to the gyro */
	if (PIOS_SPI_Init(&pios_spi_gyro_id, &pios_spi_gyro_cfg)) {
		PIOS_DEBUG_Assert(0);
	}


	PIOS_MPU6000_Attach(pios_spi_gyro_id);
	PIOS_MPU6000_Init(&pios_mpu6000_cfg);

	PIOS_BMA180_Attach(pios_spi_accel_id);
	PIOS_BMA180_Init(&pios_bma180_cfg);

	PIOS_HMC5883_Init(&pios_hmc5883_cfg);

	

}

/**
 * @}
 * @}
 */

