#ifndef I2C_H
#define I2C_H

#define I2C_START()             TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN)
#define I2C_WAIT_TRASMISSION()  while(!(TWCR & 0x80))
#define I2C_WRITE(data)         TWDR = (data); TWCR = (1 << TWINT) | (1 << TWEN)
#define I2C_STOP()              TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);

#endif