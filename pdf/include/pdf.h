#ifndef _PDF_H_
#  define _PDF_H_

#  include <string>
#  include <vector>

   using std::string;
   using std::vector;

   // ---------------------------------
   // Image-Related Types
   // ---------------------------------

   struct RGB
   {
      unsigned char mRed;
      unsigned char mGreen;
      unsigned char mBlue;

      RGB(
         unsigned char red,
         unsigned char green,
         unsigned char blue
      )  :
         mRed(red), mGreen(green), mBlue(blue)
      {
      }
   };

   typedef vector<RGB>      ImageRow;
   typedef vector<ImageRow> Image;

   struct ImageInfo
   {
      int    mObjectID;
      string mName;
      int    mWidth;
      int    mHeight;

      ImageInfo(
         int           objectID,
         const string &name,
         int           width,
         int           height
      )  :
         mObjectID(objectID), mName(name), mWidth(width), mHeight(height)
      {
      }
   };

   struct ImageEntry
   {
      Image     mImage;
      ImageInfo mInfo;

      ImageEntry(
         const Image     &image,
         const ImageInfo &info
      )  :
         mImage(image), mInfo(info)
      {
      }
   };

   // ---------------------------------
   // An x, y coordinate point
   // ---------------------------------

   struct XY
   {
      int mX;
      int mY;

      XY(int x, int y) : mX(x), mY(y) {}
   };

   // ---------------------------------
   // PDF
   // ---------------------------------

   class PDF
   {
      public:

       string encoding;
       
         enum Font
         {
            NONE,
            COURIER,
            COURIER_BOLD,
            COURIER_OBLIQUE,
            COURIER_BOLD_OBLIQUE,
            HELVETICA,
            HELVETICA_BOLD,
            HELVETICA_OBLIQUE,
            HELVETICA_BOLD_OBLIQUE,
            SYMBOL,
            TIMES,
            TIMES_BOLD,
            TIMES_ITALIC,
            TIMES_BOLD_ITALIC,
            ZAPF_DINGBATS
         };

         enum { DEFAULT_WIDTH = 612, DEFAULT_HEIGHT = 792 };

         PDF();
         PDF(int width, int height);

         int getWidth()  const;
         int getHeight() const;

       void setEncoding(string s) {
           encoding = s;
       }
       
         void newPage();

         string toString();
         bool   writeToFile(const string &fileName, string &errMsg);

         ImageInfo processImage(const Image &theImage);

         void showImage(
            const ImageInfo &info, int x, int y, double xScale, double yScale
         );

         void showImage(
            const ImageInfo &info, int x, int y, double scale
         );

         void showImage(
            const ImageInfo &info, int x, int y
         );

         static int stringWidth(
            Font currentFont, int currentSize, const string &text
         );

         int stringWidth(const string &text);

         void setFont(Font theFont, int size);
         void showTextXY(const string &text, int x, int y);
         void showTextHexaXY(const string &text, int x, int y);
         void rightJustifyTextXY(const string &text, int x, int y);

         vector<string> wrapText(
            const string &text, int maxWidth, bool rightJustify
         );

         void setLineWidth(int value);
         void drawLine(int x0, int y0, int x1, int y1);
         void drawLine(const vector<XY> &points);

         void drawRect(int x, int y, int width, int height);
         void fillRect(int x, int y, int width, int height);

         void drawPolygon(const vector<XY> &points);
         void fillPolygon(const vector<XY> &points);

         void drawEllipse(int xCenter, int yCenter, int xRadius, int yRadius);
         void fillEllipse(int xCenter, int yCenter, int xRadius, int yRadius);

         void drawCircle(int xCenter, int yCenter, int radius);
         void fillCircle(int xCenter, int yCenter, int radius);

         void setLineColor(
            unsigned char red, unsigned char green, unsigned char blue
         );

         void setFillColor(
            unsigned char red, unsigned char green, unsigned char blue
         );

         static const string FONTS[];
         static const int    N_FONTS;

      private:

         int mWidth;
         int mHeight;

         vector<string>     mPageContents;
         string             mCurrentPageContents;
         vector<ImageEntry> mEntries;

         Font mCurrentFont;
         int  mCurrentFontSize;
   };

#endif
