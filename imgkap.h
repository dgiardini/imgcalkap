#ifndef _IMG_KAP_INC__
#define _IMG_KAP_INC__
#ifdef __cplusplus
extern "C" {
#endif
#include <FreeImage.h>
typedef union
{
    RGBQUAD  q;
    uint32_t p;
} Color32;

#define METTERS     0
#define FATHOMS     1

#define NORMAL     0
#define OLDKAP     1

#define COLOR_NONE  1
#define COLOR_IMG   2
#define COLOR_MAP   3
#define COLOR_KAP   4

#define FIF_KAP 1024
#define FIF_NO1 1025
#define FIF_TXT 1026
#define FIF_KML 1027
int imgtokap(int typein,char *filein, double lat0, double lon0, double lat1, double lon1, int optkap,int color,char *title, int units, char *sd,char *fileout);
int imgheadertokap(int typein,char *filein,int typeheader,int optkap,int color,char *title,char *fileheader,char *fileout);
int kaptoimg(int typein,char *filein,int typeheader,char *fileheader,int typeout,char *fileout,char *optionpal);

int findfiletype(char *file);
int writeimgkap(FILE *out,FIBITMAP **bitmap,int optkap,int colors,Color32 *pal,uint16_t widthin,uint16_t heightin,uint16_t widthout,uint16_t heightout);

int bsb_uncompress_row(int typein, FILE *in, uint8_t *buf_out, uint16_t bits_in,uint16_t bits_out,uint16_t width);
int bsb_compress_row(const uint8_t *buf_in, uint8_t *buf_out, uint16_t bits_out, uint16_t line, uint16_t widthin,uint16_t widthout);


#ifdef __cplusplus
}
#endif

#endif
