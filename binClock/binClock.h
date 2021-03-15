#ifndef BINCLOCK_H
# define BINCLOCK_H

struct Time {
    uint8_t hours
          , minutes
          , seconds;
};

uint8_t get_time(uint8_t);
void tick(void);

#endif
