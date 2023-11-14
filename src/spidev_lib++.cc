 /*
 * Copyright (c) 2014 Philippe Van Hecke  <lemouchon@gmail.com >
 *
 * pyA20 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>

#include <linux/spi/spidev.h>

#include "spidev_lib++.h"


bool SPI::setBitPerWord(uint8_t p_bit) {

  /* Set bits per word*/
  errno = 0;
  if (ioctl(spifd_, SPI_IOC_WR_BITS_PER_WORD, &p_bit) < 0) {
    throw SPIError(strerror(errno), __FILE__, __LINE__);
  }
  if (ioctl(spifd_, SPI_IOC_RD_BITS_PER_WORD, &p_bit) < 0) {
    throw SPIError(strerror(errno), __FILE__, __LINE__);
  }

  spiconfig_.bits_per_word = p_bit;
  return true;
   
}
bool SPI::setSpeed(uint32_t p_speed) {
  /* Set SPI speed*/
  errno = 0;
  if (ioctl(spifd_, SPI_IOC_WR_MAX_SPEED_HZ, &p_speed) < 0) {
    throw SPIError(strerror(errno), __FILE__, __LINE__);
  }
  if (ioctl(spifd_, SPI_IOC_RD_MAX_SPEED_HZ, &p_speed) < 0) {
    throw SPIError(strerror(errno), __FILE__, __LINE__);
  }

  spiconfig_.speed = p_speed;
  return true;
}

bool SPI::setMode(uint8_t p_mode) {
  /* Set SPI_POL and SPI_PHA */
  errno = 0;
  if (ioctl(spifd_, SPI_IOC_WR_MODE, &p_mode) < 0) {
    throw SPIError(strerror(errno), __FILE__, __LINE__);
  }
  if (ioctl(spifd_, SPI_IOC_RD_MODE, &p_mode) < 0) {
    throw SPIError(strerror(errno), __FILE__, __LINE__);
  }

  spiconfig_.mode = p_mode;
  return true;
}

int SPI::xfer(uint8_t *buffer, uint32_t len) {
  struct spi_ioc_transfer spi_message[1];
  memset(spi_message, 0, sizeof(spi_message));
  
  spi_message[0].rx_buf = (unsigned long)buffer;
  spi_message[0].tx_buf = (unsigned long)buffer;
  spi_message[0].len = len;
  spi_message[0].delay_usecs = spiconfig_.delay;
  errno = 0;
  int ret = ioctl(spifd_, SPI_IOC_MESSAGE(1), spi_message);
  if (ret < 0) {
    throw SPIError(strerror(errno), __FILE__, __LINE__);
  }
  return ret;
}

int SPI::write(uint8_t *buffer, uint32_t len) {
  struct spi_ioc_transfer spi_message[1];
  memset(spi_message, 0, sizeof(spi_message));
  spi_message[0].tx_buf = (unsigned long)buffer;
  spi_message[0].len = len;
  spi_message[0].delay_usecs = spiconfig_.delay;
  errno = 0;
  int ret = ioctl(spifd_, SPI_IOC_MESSAGE(1), spi_message);
  if (ret < 0) {
    throw SPIError(strerror(errno), __FILE__, __LINE__);
  }
  return ret;
}

int SPI::read(uint8_t *buffer, uint32_t len) {
  struct spi_ioc_transfer spi_message[1];
  memset(spi_message, 0, sizeof(spi_message));
  
  spi_message[0].rx_buf = (unsigned long)buffer;
  spi_message[0].len = len;
  spi_message[0].delay_usecs = spiconfig_.delay;
  errno = 0;
  int ret = ioctl(spifd_, SPI_IOC_MESSAGE(1), spi_message);
  if (ret < 0) {
    throw SPIError(strerror(errno) , __FILE__, __LINE__);
  }
  return ret;
}

bool SPI::begin() {
  /* open spidev device */
  if (opened_ == true )
      return true;
  if (spidev_.empty() )
      return false;
  spifd_ = open(spidev_.c_str(), O_RDWR);

  if (spifd_ < 0) {
      return false;
  }
  /* Set SPI_POL and SPI_PHA */

  if (ioctl(spifd_, SPI_IOC_WR_MODE, &spiconfig_.mode) < 0) {
      close(spifd_);
      return false;
  }
  if (ioctl(spifd_, SPI_IOC_RD_MODE, &spiconfig_.mode) < 0) {
      close(spifd_);
      return false;
  }

  /* Set bits per word*/
  if (ioctl(spifd_, SPI_IOC_WR_BITS_PER_WORD, &spiconfig_.bits_per_word) < 0) {
      close(spifd_);
      return false;
  }
  if (ioctl(spifd_, SPI_IOC_RD_BITS_PER_WORD, &spiconfig_.bits_per_word) < 0) {
      close(spifd_);
      return false;
  }

  /* Set SPI speed*/
  if (ioctl(spifd_, SPI_IOC_WR_MAX_SPEED_HZ, &spiconfig_.speed) < 0) {
      close(spifd_);
      return false;
  }
  if (ioctl(spifd_, SPI_IOC_RD_MAX_SPEED_HZ, &spiconfig_.speed) < 0) {
      close(spifd_);
      return false;
  }

  opened_ = true;

  return true;  
}

SPI::SPI(const std::string& spidev, spi_config_t *p_spi_config) : spidev_(spidev) {
  if (p_spi_config){
  	memcpy(&spiconfig_, p_spi_config, sizeof(spi_config_t));
  }
  else {
    spiconfig_.mode = 0;
    spiconfig_.speed = 1000000;
    spiconfig_.bits_per_word = 8;
    spiconfig_.delay = 0;		
  }
}

SPI::~SPI() {
  if (opened_)
    close(spifd_);
}


bool SPI::setConfig(spi_config_t *p_spi_config) {
  if (p_spi_config != NULL) {
  	memcpy(&spiconfig_, p_spi_config, sizeof(spi_config_t));
    if (opened_){
      setMode(spiconfig_.mode);
      setBitPerWord(spiconfig_.bits_per_word);
      setSpeed(spiconfig_.speed);
    }
  }
  return false;
}



