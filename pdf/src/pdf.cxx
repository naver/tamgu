/************************************************************
 * Headers
 ************************************************************/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cctype>
#include <cmath>

#include "metrics.h"
#include "pdf.h"

/************************************************************
 * Using Declarations
 ************************************************************/

using std::ios;
using std::hex;
using std::setw;
using std::cout;
using std::endl;
using std::setfill;
using std::ofstream;
using std::ifstream;
using std::uppercase;
using std::ostringstream;

/************************************************************
 * Static Member Variables
 ************************************************************/

const string PDF::FONTS[] =
{
   "Courier",
   "Courier-Bold",
   "Courier-Oblique",
   "Courier-BoldOblique",
   "Helvetica",
   "Helvetica-Bold",
   "Helvetica-Oblique",
   "Helvetica-BoldOblique",
   "Symbol",
   "Times-Roman",
   "Times-Bold",
   "Times-Italic",
   "Times-BoldItalic",
   "ZapfDingbats"
};

const int PDF::N_FONTS = sizeof(PDF::FONTS) / sizeof(PDF::FONTS[0]);

/************************************************************
 * Constants
 ************************************************************/

static const string OP_BEGIN_TEXT    = "BT";
static const string OP_CONCAT_MATRIX = "cm";
static const string OP_DO            = "Do";
static const string OP_END_TEXT      = "ET";
static const string OP_FILL          = "f";
static const string OP_LINETO        = "l";
static const string OP_MOVETO        = "m";
static const string OP_SAVE          = "q";
static const string OP_RESTORE       = "Q";
static const string OP_LINE_COLOR    = "RG";
static const string OP_FILL_COLOR    = "rg";
static const string OP_STROKE        = "S";
static const string OP_SET_FONT      = "Tf";
static const string OP_TEXT_SHOW     = "Tj";
static const string OP_TEXT_MATRIX   = "Tm";
static const string OP_RECTANGLE     = "re";
static const string OP_WIDTH         = "w";

/************************************************************
 * Local Utility Functions
 ************************************************************/

static string handlePolygon(
   const vector<XY> &points, const string &theOperator
)
{
   if(points.size() <= 2)
      return("");

   ostringstream out;

   int n = points.size();

   for(int i = 0; i < n; i ++)
   {
      out << points[i].mX << " " <<
             points[i].mY << " " <<
             (i == 0 ? OP_MOVETO : OP_LINETO) << "\n";
   }

   // Close the polygon if it isn't already closed

   if(points[0].mX != points[n - 1].mX ||
      points[0].mY != points[n - 1].mY)
   {
      out << points[0].mX << " " <<
             points[0].mY << " " << OP_LINETO << "\n";
   }

   out << theOperator << "\n";

   return(out.str());
}

static string handleRectangle(
   int x, int y, int width, int height, const string &theOperator
)
{
   ostringstream out;

   out << x            << " " << y           << " " <<
          width        << " " << height      << " " <<
          OP_RECTANGLE << " " << theOperator << "\n";

   return(out.str());
}

static string handleColor(
   unsigned char    red,
   unsigned char    green,
   unsigned char    blue,
   const    string &theOperator
)
{
   ostringstream out;

   out <<
      (red   / 255.0) << " " <<
      (green / 255.0) << " " <<
      (blue  / 255.0) << " " << theOperator << "\n";

   return(out.str());
}

static vector<XY> strokeEllipse(
   int xCenter, int yCenter, int xRadius, int yRadius
)
{
   // ----------------
   //
   // Formula:
   //
   //     2     2
   //    X     Y
   //    -  +  -  = 1
   //     2     2
   //    A     B
   //
   // ----------------

   double a2 = xRadius * xRadius;
   double b2 = yRadius * yRadius;

   vector<XY> points;

   for(int i = xRadius; i >= 0; i --)
   {
      double x = i;
      double y = sqrt(b2 * (1 - x*x / a2));

      points.push_back(
         XY(
            (int)(xCenter + x + 0.5),
            (int)(yCenter + y + 0.5)
         )
      );
   }

   for(int i = points.size() - 2; i >= 0; i --)
   {
      points.push_back(
         XY(
            xCenter - (points[i].mX - xCenter),
            points[i].mY
         )
      );
   }

   for(int i = points.size() - 2; i >= 0; i --)
   {
      points.push_back(
         XY(
            points[i].mX,
            yCenter - (points[i].mY - yCenter)
         )
      );
   }

   return(points);
}

/************************************************************
 * Member Functions
 ************************************************************/

PDF::PDF() :
    encoding("WinAnsiEncoding"),
    mWidth(DEFAULT_WIDTH),
    mHeight(DEFAULT_HEIGHT),
    mCurrentFont(NONE),
    mCurrentFontSize(0)
{
}

PDF::PDF(int width, int height) :
    encoding("WinAnsiEncoding"),
    mWidth(width),
    mHeight(height),
    mCurrentFont(NONE),
    mCurrentFontSize(0)
{
}

int PDF::getWidth() const
{
   return(mWidth);
}

int PDF::getHeight() const
{
   return(mHeight);
}

static int getImageObjectID(int index)
{
   // Images are stored just after our fonts

   return(PDF::N_FONTS + index + 1);
}

string PDF::toString()
{
   vector<int>   offsets;
   ostringstream out;

   // ---------------------------------------------
   // Firstly, the string to identify
   // this as a pdf file
   // ---------------------------------------------

   out << "%PDF-1.1" << "\n\n";

   // ---------------------------------------------
   // Now, our standard fonts
   // ---------------------------------------------

   for(int i = 0; i < N_FONTS; i ++)
   { 
      offsets.push_back(out.str().size());

      out << (1 + i) << " 0 obj"               << "\n" <<
             "<<"                              << "\n" <<
             "   /Type     /Font"              << "\n" <<
             "   /Subtype  /Type1"             << "\n" <<
             "   /BaseFont /" << PDF::FONTS[i] << "\n" <<
             "   /Encoding /" << encoding   << "\n" <<
             ">>"                              << "\n" <<
             "endobj"                          << "\n\n";
   }

   // Store our images (if we have any)

   int theID    = getImageObjectID(0);
   int nEntries = mEntries.size();

   for(int i = 0; i < nEntries; i ++)
   {
      offsets.push_back(out.str().size());

      theID       = mEntries[i].mInfo.mObjectID;
      int height  = mEntries[i].mImage.size();
      int width   = (height > 0 ? mEntries[i].mImage[0].size() : 0);

      // We don't count the final newline in our /Length value

      int length  = (3 * 2 * width + 1) * height - 1;

      out << theID << " 0 obj" << "\n" <<
             "<<" << "\n" <<
             "   /Type              /XObject"        << "\n" <<
             "   /Subtype           /Image"          << "\n" <<
             "   /Width             " << width       << "\n" <<
             "   /Height            " << height      << "\n" <<
             "   /BitsPerComponent  8"               << "\n" <<
             "   /ColorSpace        /DeviceRGB"      << "\n" <<
             "   /Filter            /ASCIIHexDecode" << "\n" <<
             "   /Length            " << length      << "\n" <<
             ">>" << "\n";

      out << "stream" << "\n";

      for(int y = 0; y < height; y ++)
      {
         for(int x = 0; x < width; x ++)
         {
            int r = mEntries[i].mImage[y][x].mRed;
            int g = mEntries[i].mImage[y][x].mGreen;
            int b = mEntries[i].mImage[y][x].mBlue;

            ostringstream tmp;

            tmp << hex << uppercase << setfill('0') <<
                  setw(2) << r << 
                  setw(2) << g <<
                  setw(2) << b;

            out << tmp.str();
         }

         out << "\n";
      }

      out << "endstream" << "\n" <<
             "endobj"    << "\n\n";
   }

   if(nEntries)
      theID++;

   // Now, set up an obj for our common resources,
   // first our fonts and then any images

   offsets.push_back(out.str().size());

   int idResources = theID;

   out << idResources << " 0 obj" << "\n" <<
          "<<"                    << "\n" <<
          "   /Font <<"           << "\n";

   for(int i = 0; i < N_FONTS; i ++)
      out << "      /F" << (i + 1) << " " << (i + 1) << " 0 R" << "\n";

   out << "   >>"  << "\n";

   if(nEntries)
   {
      out << "   /XObject <<" << "\n";

      for(int i = 0; i < nEntries; i ++)
      {
         out << "      " <<
            mEntries[i].mInfo.mName << " " <<
            mEntries[i].mInfo.mObjectID << " 0 R" << "\n";
      }

      out << "   >>"  << "\n";
   }

   out << ">>"     <<   "\n" <<
          "endobj" << "\n\n";

   // Now, our top-level object

   offsets.push_back(out.str().size());

   int idCatalog = (idResources + 1);
   int idPages   = (idCatalog   + 1);

   out << idCatalog << " 0 obj"             << "\n" <<
          "<<"                              << "\n" <<
          "   /Type /Catalog"               << "\n" <<
          "   /Pages " << idPages << " 0 R" << "\n" <<
          ">>"                              << "\n" <<
          "endobj"                          << "\n\n";

   // Prepare our pages

   vector<string> pages;

   for(int i = 0, n = mPageContents.size(); i < n; i ++)
      pages.push_back(mPageContents[i]);

   // Append the current page but only if it
   // is not empty

   if(mCurrentPageContents != "")
      pages.push_back(mCurrentPageContents);

   // Now our pages object

   offsets.push_back(out.str().size());

   out << idPages << " 0 obj" << "\n" <<
          "<<"                << "\n" <<
          "   /Type /Pages"   << "\n" <<
          "   /MediaBox ";

   out << "[ 0 0 " << mWidth << " " << mHeight << " ]" << "\n";

   out << "   /Count " << pages.size() << "\n" <<
          "   /Kids [";

   for(int i = 0, n = pages.size(); i < n; i ++)
      out << " " << (1 + idPages + i) << " 0 R";

   out <<
      " ]"     << "\n" <<
      ">>"     << "\n" <<
      "endobj" << "\n\n";

   // Now our pages

   for(int i = 0, n = pages.size(); i < n; i ++)
   {
      offsets.push_back(out.str().size());

      int idPage = (1 + idPages + i);

      out << idPage << " 0 obj"                        << "\n" <<
             "<<"                                      << "\n" <<
             "   /Type /Page"                          << "\n" <<
             "   /Parent " << idPages << " 0 R"        << "\n" <<
             "   /Contents " << (idPage + n) << " 0 R" << "\n" <<
             "   /Resources " << idResources << " 0 R" << "\n" <<
             ">>"                                      << "\n" <<
             "endobj"                                  << "\n\n";
   }

   // Now our page contents

   int idPageContent = idPages + pages.size() + 1;

   for(int i = 0, n = pages.size(); i < n; i ++)
   {
      string theContents = pages[i];

      theContents = OP_BEGIN_TEXT + "\n" + theContents + OP_END_TEXT;
      int size    = theContents.size();

      offsets.push_back(out.str().size());

      out << idPageContent << " 0 obj" << "\n" <<
             "<<"                      << "\n" <<
             "   /Length " << size     << "\n" <<
             ">>"                      << "\n" <<
             "stream"                  << "\n" <<
             theContents               << "\n" <<
             "endstream"               << "\n" <<
             "endobj"                  << "\n\n";

      idPageContent++;
   }

   int xrefOffset = out.str().size();
   int theSize    = (offsets.size() + 1);

   string append = " ";

   out << "xref" << "\n" <<
          "0 " << theSize << "\n" <<
          "0000000000 65535 f" << append << "\n";

   for(int i = 0, n = offsets.size(); i < n; i ++)
   {
      ostringstream tmp;

      tmp << offsets[i];

      for(int j = 0, nn = 10 - tmp.str().size(); j < nn; j ++)
         out << "0";

      out << offsets[i] << " 00000 n" << append << "\n";
   }

   out << "trailer" << "\n" <<
          "<<"      << "\n" <<
          "   /Size " << theSize << "\n" <<
          "   /Root " << idCatalog << " 0 R" << "\n" <<
          ">>" << "\n" <<
          "startxref" << "\n" <<
          xrefOffset << "\n"
          "%%EOF\n";

   return(out.str());
}

bool PDF::writeToFile(const string &fileName, string &errMsg)
{
   ofstream theFile(fileName.c_str(), ios::out | ios::binary);

   if(!theFile)
   {
      errMsg = "Could not open file: [" + fileName + "]";
      return(false);
   }

   string s = toString();

   theFile.write(s.c_str(), s.size());

   theFile.close();

   return(true);
}

ImageInfo PDF::processImage(const Image &theImage)
{
   int id = getImageObjectID(mEntries.size());

   ostringstream out;

   out << "/Img" << id;

   int height = theImage.size();
   int width  = (height == 0 ? 0 : theImage[0].size());

   ImageInfo theInfo(id, out.str(), width, height);

   mEntries.push_back(ImageEntry(theImage, theInfo));

   return(theInfo);
}

void PDF::newPage()
{
   mPageContents.push_back(mCurrentPageContents);

   mCurrentPageContents = "";
   mCurrentFont         = NONE;
   mCurrentFontSize     = 0;
}

void PDF::showImage(
   const ImageInfo &info, int x, int y, double xScale, double yScale
)
{
   int width  = (int)(info.mWidth  * xScale + 0.5);
   int height = (int)(info.mHeight * yScale + 0.5);

   ostringstream out;

   out <<
      OP_SAVE    << " " <<
      width      << " " << 0     << " " << 0 << " " << height << " " <<
      x          << " " << y     << " " << OP_CONCAT_MATRIX   << " " <<
      info.mName << " " << OP_DO << " " << OP_RESTORE         << "\n";

   mCurrentPageContents += out.str();
}

void PDF::showImage(const ImageInfo &info, int x, int y, double scale)
{
   showImage(info, x, y, scale, scale);
}

void PDF::showImage(const ImageInfo &info, int x, int y)
{
   showImage(info, x, y, 1.0, 1.0);
}

int PDF::stringWidth(
   Font currentFont, int currentSize, const string &text
)
{
   int width    = 0;
   int *metrics = Metrics::get(currentFont);

   if(metrics)
   {
      double dWidth = 0.0;

      for(int i = 0, n = text.size(); i < n; i ++)
      {
         double value = metrics[text[i]] / 1000.0;
         dWidth      += currentSize * value;
      }

      width = (int)(dWidth + 0.5);
   }

   return(width);
}

int PDF::stringWidth(const string &text)
{
   int width = 0;

   if(mCurrentFont != NONE)
   {
      width = stringWidth(mCurrentFont, mCurrentFontSize, text);
   }

   return(width);
}

void PDF::setFont(Font theFont, int size)
{
   ostringstream out;

   out << "/F" << (int)theFont << " " << size << " " << OP_SET_FONT << "\n";

   mCurrentPageContents += out.str();

   mCurrentFont     = theFont;
   mCurrentFontSize = size;
}

void PDF::showTextXY(const string &text, int x, int y)
{
   string theText;

   for(int i = 0, n = text.size(); i < n; i ++)
   {
      if(text[i] == '(' || text[i] == ')')
         theText += "\\";

      theText += text[i];
   }

   ostringstream out;

   out << "1 0 0 1" << " " << x << " " << y << " " << OP_TEXT_MATRIX << "\n";
   out << "(" << theText << ")" << " " << OP_TEXT_SHOW << "\n";

   mCurrentPageContents += out.str();
}

void PDF::showTextHexaXY(const string &text, int x, int y)
{
   string theText;

   for(int i = 0, n = text.size(); i < n; i ++)
   {
      if(text[i] == '(' || text[i] == ')')
         theText += "\\";

      theText += text[i];
   }

   ostringstream out;

   out << "1 0 0 1" << " " << x << " " << y << " " << OP_TEXT_MATRIX << "\n";
   out << "<" << theText << ">" << " " << OP_TEXT_SHOW << "\n";

   mCurrentPageContents += out.str();
}


void PDF::rightJustifyTextXY(const string &text, int x, int y)
{
   if(mCurrentFont != NONE)
      x -= stringWidth(text);

   showTextXY(text, x, y);
}

vector<string> PDF::wrapText(
   const string &text, int maxWidth, bool rightJustify
)
{
   // First, break 'text' up into separate words

   vector<string> words;
   string         currentWord;

   for(int i = 0, n = text.size(); i < n; i ++)
   {
      char c = text[i];

      if(currentWord != "")
      {
         if(isspace(c))
         {
            words.push_back(currentWord);
            currentWord = "";
         }
         else
         {
            currentWord += c;
         }
      }
      else
      {
         if(!isspace(c))
         {
            currentWord += c;
         }
      }
   }

   if(currentWord != "")
      words.push_back(currentWord);

#if 0
   cout << "Initial: [" << text << "]" << endl;

   for(int i = 0; i < (int)words.size(); i ++)
      cout << "[" << words[i] << "]" << endl;

#endif

   vector<string> result;
   string         currentLine;

   for(int i = 0, n = words.size(); i < n; i ++)
   {
      string s = currentLine;

      if(currentLine != "")
      {
         switch(currentLine[currentLine.size() - 1])
         {
            case '.': case '?': case '!': s += " "; break;
         }

         s += " ";
      }

      s += words[i];
      
      if(stringWidth(s) > maxWidth)
      {
         result.push_back(currentLine);
         currentLine = words[i];
      }
      else
         currentLine = s;
   }

   result.push_back(currentLine);

   // If so requested, right justify each line

   if(rightJustify)
   {
      string aSpace  = " ";
      int spaceWidth = stringWidth(aSpace);

      // Right justify each line by interspersing additional spaces
      // between the words until adding an additional space would make
      // the line too long.

      for(int i = 0, n = result.size(); i < n; i ++)
      {
         string line = result[i];

         if(stringWidth(line) + spaceWidth <= maxWidth)
         {
            // Store off our indices, the locations in 'line' of the
            // first space after each word:
            //
            //   here  is    a string    of some  words
            //       ^   ^    ^      ^     ^    ^

            vector<int> indices;

            for(int j = 0, nl = line.size(); j < nl; j ++)
            {
               if(j > 0 && !isspace(line[j - 1]) && isspace(line[j]))
                  indices.push_back(j);
            }

            int nIndices = indices.size();

            // We only add spaces between words so unless
            // there are 2+ words there's nothing to do

            if(nIndices > 0)
            {
               // We first insert a space at indices[0], then
               // at indices[1], etc.  The variable 'theIndex'
               // indices which offset in 'indices' we're using

               int theIndex = 0;

               while(stringWidth(line) + spaceWidth <= maxWidth)
               {
                  line = line.substr(0, indices[theIndex]) +
                         aSpace +
                         line.substr(indices[theIndex]);

                  // Because we added a space to the string after
                  // indices[theIndex], we need to add one to all
                  // our offsets after 'theIndex' to reflect the
                  // addition of the space

                  for(int j = theIndex + 1; j < nIndices; j ++)
                     indices[j]++;

                  // If we've hit our last two words, roll back
                  // around to the first two words to add the
                  // next space

                  if(++theIndex == nIndices)
                     theIndex = 0;
               }
            }

            // Update the entry in 'result'

            result[i] = line;
         }
      }
   }

   return(result);
}

void PDF::setLineWidth(int value)
{
   ostringstream out;

   out << value << " " << OP_WIDTH << "\n";

   mCurrentPageContents += out.str();
}

void PDF::drawLine(int x0, int y0, int x1, int y1)
{
   vector<XY> points;

   points.push_back(XY(x0, y0));
   points.push_back(XY(x1, y1));

   drawLine(points);
}

void PDF::drawLine(const vector<XY> &points)
{
   ostringstream out;

   for(int i = 0, n = points.size(); i < n; i ++)
   {
      out << points[i].mX << " " <<
             points[i].mY << " " <<
             (i == 0 ? OP_MOVETO : OP_LINETO) << "\n";
   }

   out << OP_STROKE << "\n";

   mCurrentPageContents += out.str();
}

void PDF::drawRect(int x, int y, int width, int height)
{
   mCurrentPageContents += handleRectangle(x, y, width, height, OP_STROKE);
}

void PDF::fillRect(int x, int y, int width, int height)
{
   mCurrentPageContents += handleRectangle(x, y, width, height, OP_FILL);
}

void PDF::drawPolygon(const vector<XY> &points)
{
   mCurrentPageContents += handlePolygon(points, OP_STROKE);
}

void PDF::fillPolygon(const vector<XY> &points)
{
   mCurrentPageContents += handlePolygon(points, OP_FILL);
}

void PDF::drawEllipse(
   int xCenter, int yCenter, int xRadius, int yRadius
)
{
   mCurrentPageContents += handlePolygon(
      strokeEllipse(xCenter, yCenter, xRadius, yRadius), OP_STROKE
   );
}

void PDF::fillEllipse(
   int xCenter, int yCenter, int xRadius, int yRadius
)
{
   mCurrentPageContents += handlePolygon(
      strokeEllipse(xCenter, yCenter, xRadius, yRadius), OP_FILL
   );
}

void PDF::drawCircle(int xCenter, int yCenter, int radius)
{
   drawEllipse(xCenter, yCenter, radius, radius);
}

void PDF::fillCircle(int xCenter, int yCenter, int radius)
{
   fillEllipse(xCenter, yCenter, radius, radius);
}

void PDF::setLineColor(
   unsigned char red, unsigned char green, unsigned char blue
)
{
   mCurrentPageContents += handleColor(red, green, blue, OP_LINE_COLOR);
}

void PDF::setFillColor(
   unsigned char red, unsigned char green, unsigned char blue
)
{
   mCurrentPageContents += handleColor(red, green, blue, OP_FILL_COLOR);
}
