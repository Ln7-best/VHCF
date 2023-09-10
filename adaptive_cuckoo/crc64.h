#pragma once
#ifndef _CRC_CRC64_H
#define _CRC_CRC64_H

#include <stdint.h>
extern "C" {

uint64_t crc64(const void *s, int l);
}
#endif