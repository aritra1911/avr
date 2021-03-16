#ifndef _SYNC_H
# define _SYNC_H

ISR(TWI_vect);
void initI2CSlave(const uint8_t);
void connectI2CSlave(void);
void disconnectI2CSlave(void);

#endif  /* _SYNC_H */
