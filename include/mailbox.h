// mailbox.h -- Brad Slayter

#ifndef MAILBOX_H
#define MAILBOX_H

#include <stddef.h>
#include <stdint.h>

extern uint32_t readMailbox(uint32_t channel);
extern void writeMailbox(uint32_t channel, uint32_t data);

#endif