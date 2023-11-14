#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <spidev_lib++.h>
#include <memory>

static void hex_dump(const uint8_t *src, size_t length, size_t line_size, char *prefix)
{
	int i = 0;
	const uint8_t *address = src;
	const uint8_t *line = address;
	unsigned char c;

	printf("%s | ", prefix);
	while (length-- > 0) {
		printf("%02X ", *address++);
		if (!(++i % line_size) || (length == 0 && i % line_size)) {
			if (length == 0) {
				while (i++ % line_size)
					printf("__ ");
			}
			printf(" | ");  /* right close */
			while (line < address) {
				c = *line++;
				printf("%c", (c < 33 || c == 255) ? 0x2E : c);
			}
			printf("\n");
			if (length > 0)
				printf("%s | ", prefix);
		}
	}
}


int  main() {

  constexpr uint32_t bufsize = 200;
  uint8_t buffer[bufsize];
  memset(buffer,0,bufsize);

  spi_config_t spi_config{SPI_MODE_3, 8, 1000000, 0};
  std::unique_ptr<SPI> mySPI = std::make_unique<SPI>("/dev/spidev1.1", &spi_config);

  if (1) {// mySPI->begin()) {
    //mySPI->xfer(tx_buffer,strlen((char*)tx_buffer),rx_buffer,strlen((char*)tx_buffer));
    mySPI->read(buffer, bufsize);
    hex_dump(buffer, bufsize, 32, "RX");
      //printf("rx_buffer=%s\n",(char *)rx_buffer);
  }
  return 0;
}
