#ifndef FI_METADATA_TYPE_HPP_
#define FI_METADATA_TYPE_HPP_

#include <FreeImage.h>

namespace fi
{
enum class metadata_type
{
  none      = FIDT_NOTYPE    ,
  undefined = FIDT_UNDEFINED ,
  uint8     = FIDT_BYTE      ,
  uint16    = FIDT_SHORT     ,
  uint32    = FIDT_LONG      ,
  uint64    = FIDT_LONG8     ,
  int8      = FIDT_SBYTE     ,
  int16     = FIDT_SSHORT    ,
  int32     = FIDT_SLONG     ,
  int64     = FIDT_SLONG8    ,
  float_    = FIDT_FLOAT     ,
  double_   = FIDT_DOUBLE    ,
  urational = FIDT_RATIONAL  ,
  rational  = FIDT_SRATIONAL ,
  ifd32     = FIDT_IFD       ,
  ifd64     = FIDT_IFD8      ,
  ascii     = FIDT_ASCII     ,
  palette   = FIDT_PALETTE
};
}

#endif
