/**
 * generated by HARMLESS : 'Hardware ARchitecture Modeling Language
 *                          for Embedded Software Simulation'
 * model : e200z4
 *
 */

#ifndef __e200z4_COMP_TYPEDEF_H__
#define __e200z4_COMP_TYPEDEF_H__

typedef struct {
	u8 lock;
	u32 tag;
	u8 valid;
} gadl_type_ICacheE200Z4_cacheLine;

typedef struct {
	gadl_type_ICacheE200Z4_cacheLine lines[64];
} gadl_type_ICacheE200Z4_cacheWay;

typedef struct {
	u32 tag;
	u8 valid;
} gadl_type_fetcher_cacheLine;

#endif