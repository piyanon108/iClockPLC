#ifndef AD5680SPI_H
#define AD5680SPI_H

#include <QObject>
#include <string>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <QThread>
#include <linux_spi.h>


#define DAC_RANGE (0x3ffff)
#define DAC_MIDPOINT (DAC_RANGE >> 1)
#define AD5680_SPI_MODE        (3)
#define AD5680_SPI_FREQUENCY   (100000)
#define AD5680_SPI_CHANNEL    0 // Channel.
#define AD5680_SPI_BAUD 5000000 // Baud rate (5 MHz).
#define AD5680_SPI_FLAGS   0x03 // Mode flags for pigpio.

class AD5680SPI
{
public:
    AD5680SPI(char *spichannel);

    // ----------------------------------------------------------------------------
    /*
        Writes a command.
    */
    // ----------------------------------------------------------------------------
    void ad5680_write_command( uint8_t command );

    // ----------------------------------------------------------------------------
    /*
        Writes a data byte.
    */
    // ----------------------------------------------------------------------------
    void ad5680_write_data( uint8_t data );
    void ad5680_write_data(uint8_t *data , uint16_t len);

    // ----------------------------------------------------------------------------
    /*
        Writes a data stream.
    */
    // ----------------------------------------------------------------------------
    void ad5680_write_stream(uint8_t *buf, uint16_t count );

    void ad5680_init(char *spichannel);
    int setVout(uint32_t data);
private:
    void iniHW();
    void delay_mSec(int mSec);
    void spi_init(char *spidev);
    void clear_rx();
    int send_byte_data(uint8_t *txByteData, uint16_t len);
    int mode_spi = AD5680_SPI_MODE;
    int ret_spi=0;
    int fd_spi;
    uint8_t bits = 8;
    uint32_t speed = AD5680_SPI_FREQUENCY;
    uint16_t delay = 10;
    uint8_t tx[2048] = {0, };
    uint8_t rx[2048] = {0, };
    char *spiDev;
    bool m_IsEventThreadRunning;
};

#endif // AD5680SPI_H
