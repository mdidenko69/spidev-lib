/*
 *
 * Copyright (c) 2014 Philippe Van Hecke <lemouchon@gmail.com>
 *
 * you can redistribute it and/or modify
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

#ifndef _SPI_LIB_HPP
#define _SPI_LIB_HPP

#include <stdint.h>
#include <stdexcept>
#include <string>
#include <sstream>
#include <linux/spi/spidev.h>

typedef struct {
  uint8_t mode{SPI_MODE_0};
  uint8_t bits_per_word{8};
  uint32_t speed{5000000};
  uint16_t delay{0};
} spi_config_t;

class SPIError : public std::runtime_error {
public:
  SPIError(const std::string& msg, const char *file, int line) : std::runtime_error(msg) {
    std::ostringstream o;
    o << file << ":" << line << ": " << msg;
    msg_ = o.str();
  }
  const char *what() const noexcept override {
    return msg_.c_str();
  }
private:
  std::string msg_;
};

class SPI {
public:

  SPI(const std::string& spidev, spi_config_t *p_spi_config = nullptr);
  ~SPI();

  bool begin();
  int read(uint8_t *buffer, uint32_t len);
  int write(uint8_t *buffer, uint32_t len);
  int xfer(uint8_t *buffer, uint32_t len);
  bool setSpeed(uint32_t p_speed);
  bool setMode(uint8_t p_mode);
  bool setBitPerWord(uint8_t p_bit);
	bool setConfig(spi_config_t *p_spi_config);

private:
  std::string spidev_ {nullptr};
  int spifd_ {};
  spi_config_t spiconfig_;
  bool opened_ {false};

};

#endif





