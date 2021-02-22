/*
* Copyright (c) 2020-2021 Nordic Semiconductor, ASA
*
* SPDX-License-Identifier: Apache-2.0
*
* nsc v1.4.1 1/14/21
*
* nRF SPI Loopback Test : Connect MISO to MOSI for loopback
*   Master (SPIM) in foreground 
*   Slave (SPIS) in a Zephyr Thread
*
*/

#include <zephyr.h>
#include <sys/printk.h>
#include <drivers/spi.h>
#include <string.h>

#define SPIS_STACK_SIZE 500
#define SPIS_PRIORITY 5

#define SYCN_TEST true
#ifdef  SYCN_TEST
        static uint8_t tx_message_cnt = 1;
        #define TST_IDX 1
#endif

extern void spis_thread(void *, void *, void *);

K_THREAD_DEFINE(spis_tid, SPIS_STACK_SIZE, spis_thread, NULL, NULL, NULL,
 		SPIS_PRIORITY, 0, 0);

#define SPI_BUFFER_SIZE 32
static uint8_t spim_tx_buffer[SPI_BUFFER_SIZE] = "[*]MASTER OUT TEST STRING 32BYTE";
static uint8_t spim_rx_buffer[SPI_BUFFER_SIZE];
                                                  
static uint8_t spis_tx_buffer[SPI_BUFFER_SIZE] = "<*>slave out default message buf";
static uint8_t spis_rx_buffer[SPI_BUFFER_SIZE];

//////// Master Setup

#define MY_SPIM DT_NODELABEL(spi2)

const struct device * spim_dev;

 struct spi_cs_control spim_cs_one = { .gpio_pin = DT_GPIO_PIN(MY_SPIM, cs_gpios),
                        .gpio_dt_flags = GPIO_ACTIVE_LOW,
                        .gpio_dt_flags = GPIO_ACTIVE_LOW,
                        .delay = 0,
  };

static const struct   spi_config spim_cfg = { 
                        .operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB |
                              SPI_MODE_CPOL | SPI_MODE_CPHA,
                        .frequency = 4000000,
                        .slave = 0,
                        .cs = &spim_cs_one,	
};

const struct spi_buf  spim_tx_buf = {
                        .buf = spim_tx_buffer,
  	            	      .len = sizeof(spim_tx_buffer)
};

const struct spi_buf_set  spim_tx = {
                            .buffers = &spim_tx_buf,
		                        .count = 1
};

const struct spi_buf spim_rx_buf = {
                    	.buf = spim_rx_buffer,
                    	.len = sizeof(spim_rx_buffer),
};

const struct spi_buf_set spim_rx = {
                        .buffers = &spim_rx_buf,
                        .count = 1
}; 

//////// Slave Setup

#define MY_SPIS DT_NODELABEL(spi1)

const struct device * spis_dev;

static const struct spi_config spis_cfg = { .operation =  SPI_WORD_SET(8) | SPI_TRANSFER_MSB |
                                                          SPI_OP_MODE_SLAVE | SPI_MODE_CPOL | SPI_MODE_CPHA,
					                                  .frequency = 4000000,
					                                  .slave = 1,
};

const struct spi_buf spis_tx_buf = { .buf = spis_tx_buffer,
            									 	     .len = sizeof(spis_tx_buffer)
};

const struct spi_buf_set spis_tx = {.buffers = &spis_tx_buf,
				                            .count = 1
};

const struct spi_buf spis_rx_buf = 	{ .buf = spis_rx_buffer,
					                            .len = sizeof(spis_rx_buffer),
};

const struct spi_buf_set spis_rx =	{ .buffers = &spis_rx_buf,
					                            .count = 1 
};

//////// Functions alphabetical

void spi_test_send(void){
  int err;
  uint8_t incr;
     
  incr = 0;

  if(spis_dev != NULL) {

#ifdef SYCN_TEST
    spim_tx_buffer[TST_IDX] = '0'+ (tx_message_cnt % 10);
    printk("\nSYCN_TEST %d\n", tx_message_cnt++);
#endif

    err = spi_transceive(spim_dev, &spim_cfg, &spim_tx, &spim_rx);

    if (err == 0) {
      printk("SPIM RX: ");
           
      while(incr < SPI_BUFFER_SIZE) {
        printk("%c", spim_rx_buffer[incr]);
        incr++;
      };  

    printk("\n");
  } //(err == 0)
  else
    printk("SPIM error: %d\n", err);
  } //(spis_dev != NULL) 

#ifdef SYCN_TEST     
  else 
	 printk("SPIS has not been initialized.\n");     
#endif           
} //spi_test_send ()

//////// 

static void spim_init(void){

  spim_cs_one.gpio_dev = device_get_binding(DT_GPIO_LABEL(MY_SPIM, cs_gpios));
   
  if (spim_cs_one.gpio_dev != NULL){
    spim_dev = device_get_binding(DT_LABEL(MY_SPIM)); // recommended syntax

    if (spim_dev != NULL){
      printk("\nSPIM Device: %s\n", DT_PROP(MY_SPIM,label));
      printk("SPIM CSN PIN %d, MISO PIN %d, MOSI PIN %d, CLK PIN %d\n",
                    DT_GPIO_PIN(MY_SPIM, cs_gpios),
                    DT_PROP(MY_SPIM, miso_pin),
                    DT_PROP(MY_SPIM, mosi_pin),
                    DT_PROP(MY_SPIM, sck_pin)); 
    } //(spim_dev != NULL)
    else 
      printk("\nSPIM Device: %s\n", DT_PROP(MY_SPIM,label));

  } //(spim_cs_one.gpio_dev != NULL)
  else
    printk("Could not get GPIO LABEL %s device\n", DT_GPIO_LABEL(MY_SPIM, cs_gpios));
    
}

//////// 

static void spis_init(void){
	
  spis_dev = device_get_binding(DT_LABEL(MY_SPIS));

  if (spis_dev != NULL) {
    printk("\nSPIS device name: %s\n", DT_LABEL(MY_SPIS));
    printk("SPIS CSN %d , MISO %d, MOSI %d, CLK %d\n\n",
    DT_PROP(MY_SPIS, csn_pin),
    DT_PROP(MY_SPIS, miso_pin),
    DT_PROP(MY_SPIS, mosi_pin),
    DT_PROP(MY_SPIS, sck_pin));
  } 
  else 
    printk("Could not get %s device\n", DT_PROP(MY_SPIS,label));

} //spis_init ()

//////// 

void spis_test_transceive(void){
	int cnt;
  uint8_t incr = 0;

  cnt = spi_transceive(spis_dev, &spis_cfg, &spis_tx, &spis_rx);

#ifdef SYCN_TEST
  spis_tx_buffer[TST_IDX] = spis_rx_buffer[TST_IDX];
#endif

	if (cnt > 0) {
    printk("SPIS RX: ");
    while (incr < SPI_BUFFER_SIZE) {        
      printk("%c", spis_rx_buffer[incr]);
      incr++;
    };

  printk("\n\n");  
  } // (cnt > 0)
  else 
	  printk("SPIS error %i\n", cnt);

} // spi_test_transceive()

////////

extern void spis_thread(void *unused1, void *unused2, void *unused3){

  spis_init();

  while (1) {
	 spis_test_transceive();
	}
}

///////////////////////////////////////////////////////////////////////

void main(void){

 	printk("\n\n\nSPI Loop Back Example\n");
 	printk("SPIS handling will be performed in an independent thread\n");
 	printk("SPIM task in main thread\n\n");

  spim_init();

  do{ 
		spi_test_send();
        		k_msleep(1000);
    	} while (1);
 }
