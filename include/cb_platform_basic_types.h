/**
 \copyright Copyright (C) 2020 u-blox 

u-blox reserves all rights in this deliverable (documentation, software, etc.,
hereafter “Deliverable”). 

u-blox grants you the right to use, copy, modify and distribute the
Deliverable provided hereunder for any purpose without fee.

THIS DELIVERABLE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
WARRANTY. IN PARTICULAR, NEITHER THE AUTHOR NOR U-BLOX MAKES ANY
REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY OF THIS
DELIVERABLE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.

In case you provide us a feedback or make a contribution in the form of a
further development of the Deliverable (“Contribution”), u-blox will have the
same rights as granted to you, namely to use, copy, modify and distribute the
Contribution provided to us for any purpose without fee.

Component:  u-connectXpress Host Library
\file
*/


#ifndef _CB_PLATFORM_BASIC_TYPES_H_
#define _CB_PLATFORM_BASIC_TYPES_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/*! 
 *  @{
 * \libfile Internal defines.
 * \libdefs
*/

/*===========================================================================
 * TYPES
 *=========================================================================*/

/*===========================================================================
 * COMMON SYSTEM DEFINES
 *=========================================================================*/


typedef int8_t                  cb_int8;
typedef int16_t                 cb_int16;
typedef int32_t                 cb_int32;
typedef int64_t                 cb_int64;

typedef uint8_t                 cb_uint8;
typedef uint16_t                cb_uint16;
typedef uint32_t                cb_uint32;
typedef uint64_t                cb_uint64;

typedef bool                    cb_boolean;
typedef char                    cb_char;
typedef int                     cb_int;

/**
 * Used when declaring an empty array that does not take up space in a struct.
 * Example: struct { cb_uint8 payload[cb_EMPTY_ARRAY]; }
 * In some compilers this is empty i.e. payload[]. While in some it requires a zero.
 * I.e. payload[0];
 * Use this define to get it working for your system.
 */
#define cb_EMPTY_ARRAY

/*===========================================================================
 * DEFINES
 *=========================================================================*/

/**
 * Used in function definitions to declare an input parameter unused to avoid warnings.
 */
#define cb_UNUSED(x) UNUSED_ ## x __attribute__((unused))


/**
 * Define cb_ASSERT to the wanted assert handler.
 */

#define cb_ASSERT(exp)              do { if (!(exp)) { \
                                        printf("ASSERT %s:%d: %s\n", __FILE__, __LINE__, #exp); \
                                        while(1); \
                                    } } while(0)



/**@{*/
/**
 * Packed struct defines.
 * - cb_PACKED_STRUCT_ATTR_PRE is used before the typedef'ed struct declaration.
 * - cb_PACKED_STRUCT_ATTR_INLINE_PRE is after the typedef but before the struct declaration.
 * - cb_PACKED_STRUCT_ATTR_INLINE_POST is used after the struct declaration but before the typedef'ed name.
 * - cb_PACKED_STRUCT_ATTR_POST is used after the entire struct declaration.
 * 
 * example:
 *      cb_PACKED_STRUCT_ATTR_PRE
 *      typedef cb_PACKED_STRUCT_ATTR_INLINE_PRE struct myPackedStruct {
 *          int a;
 *          int b;
 *      } cb_PACKED_STRUCT_ATTR_INLINE_POST myPackedStruct
 *      cb_PACKED_STRUCT_ATTR_POST
 *
 */
/*
#define cb_PACKED_STRUCT_ATTR_PRE
#define cb_PACKED_STRUCT_ATTR_INLINE_PRE
*/
#define cb_PACKED_STRUCT_ATTR_INLINE_POST       __attribute__ ((__packed__))
/*
#define cb_PACKED_STRUCT_ATTR_POST
*/
/**@}*/


/*! @} */

#endif /* _CB_PLATFORM_BASIC_TYPES_H_ */

