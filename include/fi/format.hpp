#ifndef FI_FORMAT_HPP_
#define FI_FORMAT_HPP_

#include <FreeImage.h>

namespace fi
{
enum class format
{
  unknown = FIF_UNKNOWN,
  bmp     = FIF_BMP    ,
  ico     = FIF_ICO    ,
  jpeg    = FIF_JPEG   ,
  jng     = FIF_JNG    ,
  koala   = FIF_KOALA  ,
  lbm     = FIF_LBM    ,
  iff     = FIF_IFF    ,
  mng     = FIF_MNG    ,
  pbm     = FIF_PBM    ,
  raw_pbm = FIF_PBMRAW ,
  pcd     = FIF_PCD    ,
  pcx     = FIF_PCX    ,
  pgm     = FIF_PGM    ,
  raw_pgm = FIF_PGMRAW ,
  png     = FIF_PNG    ,
  ppm     = FIF_PPM    ,
  raw_ppm = FIF_PPMRAW ,
  ras     = FIF_RAS    ,
  targa   = FIF_TARGA  ,
  tiff    = FIF_TIFF   ,
  wbmp    = FIF_WBMP   ,
  psd     = FIF_PSD    ,
  cut     = FIF_CUT    ,
  xbm     = FIF_XBM    ,
  xpm     = FIF_XPM    ,
  dds     = FIF_DDS    ,
  gif     = FIF_GIF    ,
  hdr     = FIF_HDR    ,
  faxg3   = FIF_FAXG3  ,
  sgi     = FIF_SGI    ,
  exr     = FIF_EXR    ,
  j2k     = FIF_J2K    ,
  jp2     = FIF_JP2    ,
  pfm     = FIF_PFM    ,
  pict    = FIF_PICT   ,
  raw     = FIF_RAW    ,
  webp    = FIF_WEBP   ,
  jxr     = FIF_JXR
};
}

#endif
