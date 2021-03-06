#ifndef RDS_H
#define RDS_H

#include <inttypes.h>

#define RDS_FLAG_INIT       50

char *rdsGetText(void);
void rdsSetBlocks(uint8_t *rdsBlock);
void rdsDisable();
uint8_t rdsGetFlag(void);

#endif // RDS_H
