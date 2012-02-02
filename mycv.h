#ifndef MYCV_H
#define MYCV_H
typedef unsigned char uchar;
template<class T> class Image
{
  private:
  IplImage* imgp;
  public:
  Image(IplImage* img=0) {imgp=img;}
  ~Image(){imgp=0;}
  void operator=(IplImage* img) {imgp=img;}
  inline T* operator[](const int rowIndx) {
    return ((T *)(imgp->imageData + rowIndx*imgp->widthStep));}
}; 
 
typedef struct{
  unsigned char b,g,r;
} RgbPixel; 
 
typedef struct{
  float b,g,r;
} RgbPixelFloat; 
 
typedef Image<RgbPixel>       RgbImage;
typedef Image<RgbPixelFloat>  RgbImageFloat;
typedef Image<unsigned char>  BwImage;
typedef Image<float>          BwImageFloat;

uchar trimax(uchar x,uchar y,uchar z)
{
  uchar t=(x>y)?x:y;
  return ((t>z)?t:z);
}

uchar trimin(uchar x,uchar y,uchar z)
{
  uchar t=(x<y)?x:y;
  return ((t<z)?t:z);
}
#endif
