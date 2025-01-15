#include "ad5680SPI.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <QThread>
#include <QDebug>
// Hardware functions. --------------------------------------------------------

// ----------------------------------------------------------------------------
/*
    Writes a command.
*/
// ----------------------------------------------------------------------------
AD5680SPI::AD5680SPI(char *spichannel)
{
    ad5680_init(spichannel);
}
// Hardware functions. --------------------------------------------------------

// ----------------------------------------------------------------------------
/*
    Writes a command.
*/
// ----------------------------------------------------------------------------
void AD5680SPI::ad5680_write_command( uint8_t command )
{
    uint8_t spi[1];
    spi[0] = command;
    send_byte_data(spi, 1);
}

// ----------------------------------------------------------------------------
/*
    Writes a data byte.
*/
// ----------------------------------------------------------------------------
void AD5680SPI::ad5680_write_data( uint8_t data )
{
    uint8_t spi[1];
    spi[0] = data;
    send_byte_data(spi, 1);
}

void AD5680SPI::ad5680_write_data( uint8_t *data , uint16_t len)
{
    send_byte_data(data, len);
}
// ----------------------------------------------------------------------------
/*
    Writes a data stream.
*/
// ----------------------------------------------------------------------------
void AD5680SPI::ad5680_write_stream( uint8_t *buf, uint16_t count )
{
    send_byte_data(buf, count);
}

// ----------------------------------------------------------------------------
void AD5680SPI::ad5680_init( char *spichannel)
{
    static bool init = false;       // 1st Call to init.

    spi_init(spichannel);
    init = true;

}

void AD5680SPI::spi_init(char* spidev)
{
    fd_spi = open(spidev, O_RDWR);
    if (fd_spi < 0)
        qDebug() << "can't open device";
    ret_spi = ioctl(fd_spi, SPI_IOC_WR_MODE, &mode_spi);
    if (ret_spi == -1)
        qDebug() << ("can't set spi mode");
    ret_spi = ioctl(fd_spi, SPI_IOC_RD_MODE, &mode_spi);
    if (ret_spi == -1)
        qDebug() << ("can't get spi mode");
    ret_spi = ioctl(fd_spi, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret_spi == -1)
        qDebug() << ("can't set bits per word");
    ret_spi = ioctl(fd_spi, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret_spi == -1)
        qDebug() << ("can't get bits per word");
    ret_spi = ioctl(fd_spi, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret_spi == -1)
        qDebug() << ("can't set max speed hz");
    ret_spi = ioctl(fd_spi, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret_spi == -1)
        qDebug() << ("can't get max speed hz");

    printf("spi mode: 0x%x\n", mode_spi);
    printf("bits per word: %d\n", bits);
    printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

}

void AD5680SPI::clear_rx(){
    for (int i = 0; i < 10; i++){
        rx[i]  = 0;
    }
}
int AD5680SPI::send_byte_data(uint8_t *txByteData, uint16_t len)
{
    uint8_t *rxByteData;
    struct spi_ioc_transfer tr = {
        tr.tx_buf = (unsigned long)txByteData,
        tr.rx_buf = (unsigned long)rxByteData,
        tr.len = len,
        tr.delay_usecs = delay,
        tr.speed_hz = speed,
        tr.bits_per_word = bits,
    };
    if (mode_spi & SPI_TX_QUAD)
        tr.tx_nbits = 4;
    else if (mode_spi & SPI_TX_DUAL)
        tr.tx_nbits = 2;
    if (mode_spi & SPI_RX_QUAD)
        tr.rx_nbits = 4;
    else if (mode_spi & SPI_RX_DUAL)
        tr.rx_nbits = 2;
    if (!(mode_spi & SPI_LOOP)) {
        if (mode_spi & (SPI_TX_QUAD | SPI_TX_DUAL))
            tr.rx_buf = 0;
        else if (mode_spi & (SPI_RX_QUAD | SPI_RX_DUAL))
            tr.tx_buf = 0;
    }
    ret_spi = ioctl(fd_spi, SPI_IOC_MESSAGE(1), &tr);
    if (ret_spi < 0)
        qDebug() << "can't send_byte_data" << ret_spi;
    return ret_spi;
}
int AD5680SPI::setVout(uint32_t data)
{
    uint32_t dataShift = (data & 0x0003FFFF) << 2;
    uint8_t buf[3];
    buf[0] = (dataShift) >> 0;
    buf[1] = (dataShift) >> 8;
    buf[2] = (dataShift) >> 16;
    qDebug() << buf[2] << buf[1] << buf[0];
    send_byte_data(buf, 3);
}
