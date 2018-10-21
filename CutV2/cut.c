#include "cut.h"


/*
 * determines the average space between caracters of a line
 * param :
 *      image: image where informations will be read
 *      rect: rectangle around the line
 */
int GetLineThresold(Image image, Rect line)
{
    // go to first col with black pixels
    int keep = 1;
    int x = line.topLeft.x;
    for (; x < line.downRight.x && keep; ++x)
    {
        for (int y = line.topLeft.y; y < line.downRight.y; ++y)
        {
            int pos = y * image.w + x;
            if (image.data[pos] == 1)
            {
                keep = 0;
            }
        }
    }

    int active = 0;
    unsigned long colCount = 0;
    unsigned long bufferCount = 0;
    unsigned long spaceCount = 0;

    /*
    for debugging purposes
    Rect rect;
    rect.topLeft.y = line.topLeft.y;
    rect.downRight.y = line.downRight.y;
    */

    // first line with black piexls,
    // count every space and length till no black pixels no more
    for (; x < line.downRight.x; ++x)
    {
        int y = line.topLeft.y;
        for (; y < line.downRight.y; ++y)
        {
            int pos = y * image.w + x;
            if (image.data[pos] == 1)
            {
                if (active == 1)
                {
                    spaceCount++;
                    colCount += bufferCount;
                    bufferCount = 0;
                    /*
                     for debugging purposes
                    rect.downRight.x = x - 1;
                    DrawRect(rect, image, 3, 3);
                    */
                }
                active = 0;
                break;
            }

        }
        if  (y == line.downRight.y)
        {
            if (active == 0)
            {
                active = 1;
                // for debugging purposes
                // rect.topLeft.x = x;
            }
            bufferCount++;
        }
    }
    return (spaceCount != 0) ? colCount / spaceCount : 0;
}

/*
 * Applies caracter cut of the image in the line specified bu rect
 * AND calculates linethresold to estimate average space and detect spaces
 * Also writes in FILE f the position of detected caracters and spaces too
 * param :
 *      image: image where informations will be read
 *      rect: rectangle around the line
 *      result: image where graphical result will be saved
 *      f: file in which OCR result will be written
 */
void CutChar2(Image image, Rect line, Image result, FILE *f)
{
    int thresold = GetLineThresold(image, line);
    // activation function (linear)
    thresold = 1.5 * thresold;

    int xl = 0, xr = 0;

    int active = 0;
    Rect charPos;
    charPos.topLeft.y = line.topLeft.y;
    charPos.downRight.y = line.downRight.y;
    for (int x = line.topLeft.x; x < line.downRight.x; ++x)
    {
        int y = line.topLeft.y;
        for (; y < line.downRight.y; ++y)
        {
            int pos = y * image.w + x;
            if (image.data[pos] == 1 )
            {
                if  (active == 0)
                {
                    charPos.topLeft.x = ( x == 0 )? 0 : x-1;
                    active = 1;
                    // compated computed space to thresold
                    xr = x;
                    if (xr - xl > thresold)
                    {
                        Rect rect;
                        rect.topLeft.x = xl;
                        rect.downRight.x = xr-1;
                        rect.topLeft.y = line.topLeft.y;
                        rect.downRight.y = line.downRight.y;
                        DrawRect_hor(rect, image, 4);
                        fputc('_', f);
                    }

                }
                break;
            }
        }
        if  (y == line.downRight.y)
        {
            if (active == 1)
            {
                active = 0;
                charPos.downRight.x = x;
                fputc('C', f);
                DrawRect_ver(charPos, result, 3);
                xl = x;
            }
        }
    }
}

/*
 * Sends a rect of the image without white borders (no need to save them)
 * param :
 *      image: image where informations will be read
 */
Rect CutBorder(Image image)
{
    Cord topleft;
    topleft.x = 0;
    topleft.y = 0;

    Cord downRight;
    downRight.x = image.w;
    downRight.y = image.h;

    Rect rect;
    rect.topLeft = topleft;
    rect.downRight = downRight;

    // left border
    unsigned keep = 1;
    for (size_t x = 0; x < image.w && keep; ++x)
    {
        for (size_t y = 0; y < image.h && keep; ++y)
        {
            int pos = y * image.w + x;
            if (image.data[pos] == 1)
            {
                rect.topLeft.x = x;
                keep = 0;
            }
        }
    }
    // right border
    keep = 1;
    for (size_t x = image.w-1; x > 0 && keep; --x)
    {
        for (size_t y = 0; y < image.h && keep; ++y)
        {
            int pos = y * image.w + x;
            if (image.data[pos] == 1)
            {
                rect.downRight.x = x + 1;
                keep = 0;
            }
        }
    }
    // upper border
    keep = 1;
    for (size_t y = 0; y < image.h && keep; ++y)
    {
        for (size_t x = 0; x < image.w && keep; ++x)
        {
            int pos = y * image.w + x;
            if (image.data[pos] == 1)
            {
                rect.topLeft.y = y;
                keep = 0;
            }
        }
    }
    // down border
    keep = 1;
    for (size_t y = image.h-1; y > 0 && keep; --y)
    {
        for (size_t x = 0; x < image.w && keep; ++x)
        {
            int pos = y * image.w + x;
            if (image.data[pos] == 1)
            {
                rect.downRight.y = y + 1;
                keep = 0;
            }
        }
    }
    DrawRect(rect, image, 4, 4);
    return rect;
}

/*
 * Sends back a copy of a given Image struct
 * param :
 *      image: image where informations will be read
 */
Image CopyImage(Image image)
{
    Image result;
    result.w = image.w;
    result.h = image.h;
    unsigned char data[ result.w * result.h ];
    result.data = data;

    int max = result.w * result.h;
    for (int k = 0; k < max; ++k)
    {
        data[k] = image.data[k];
    }
    return result;
}

/*
 * For a given image in a rect zone, detects lines
 * It also applies caracter detection and eventually space detection
 * param :
 *      image: image where informations will be read
 *      rect: rectangle around the block
 *      result: image where graphical result will be saved
 *      f: file in which OCR result will be written
 */
void cutLine(Image image, Rect rect, Image result, FILE *f)
{
    /*
    Image result;
    result.w = image.w;
    result.h = image.h;
    result.data = image.data;
    */
    int active = 0;
    Rect inrect;
    inrect.topLeft.x = rect.topLeft.x;
    inrect.downRight.x = rect.downRight.x;
    for (int y = rect.topLeft.y; y < rect.downRight.y; ++y)
    {
        int x = rect.topLeft.x;
        for (; x < rect.downRight.x; ++x)
        {
            int pos = y * image.w + x;
            if (image.data[pos] == 1)
            {
                if (active == 0)
                {
                    inrect.topLeft.y = ( y == 0 )? 0 : y-1;
                    active = 1;
                }
                break;
            }
        }
        if (x == rect.downRight.x && active == 1)
        {
            active = 0;
            inrect.downRight.y = y;
            DrawRect_hor(inrect, result, 2);
            CutChar(image, inrect, result, f);
            //CutChar2(image, inrect, result, f);
            fputc('\n', f);
        }
    }
}

/*
 * Applies caracter cut of the image in the line specified bu rect
 * Also writes in FILE f the position of detected caracters and spaces too
 * param :
 *      image: image where informations will be read
 *      rect: rectangle around the line
 *      result: image where graphical result will be saved
 *      f: file in which OCR result will be written
 */
void CutChar(Image image, Rect line, Image result, FILE *f)
{
    int active = 0;
    Rect charPos;
    charPos.topLeft.y = line.topLeft.y;
    charPos.downRight.y = line.downRight.y;
    for (int x = line.topLeft.x; x < line.downRight.x; ++x)
    {
        int y = line.topLeft.y;
        for (; y < line.downRight.y; ++y)
        {
            int pos = y * image.w + x;
            if (image.data[pos] == 1 )
            {
                if  (active == 0)
                {
                    charPos.topLeft.x = ( x == 0 )? 0 : x-1;
                    active = 1;
                }
                break;
            }
        }
        if  (y == line.downRight.y && active == 1)
        {
            active = 0;
            charPos.downRight.x = x;
            fputc('C', f);
            DrawRect_ver(charPos, result, 3);
        }
    }
}

/*
 * Draws the borders of the rect in given image
 * param :
 *      rect: rectangle to draw
 *      image: image where grapgical result will be saved
 *      hor_val: horizontal value to apply
 *      ver_val: vertical value to apply
 * Take note : corners will be defined by hor_val
 */
void DrawRect(Rect rect, Image image, int hor_val, int ver_val)
{
    DrawRect_ver(rect, image, ver_val);
    DrawRect_hor(rect, image, hor_val);
}

/*
 * Draws the horizontal borders of the rect in given image
 * param :
 *      rect: rectangle to draw
 *      image: image where grapgical result will be saved
 *      val: value to apply
 */
void DrawRect_hor(Rect rect, Image image, int val)
{
    int ypos = rect.topLeft.y * image.w;
    for (int x = rect.topLeft.x; x <= rect.downRight.x; ++x)
    {
        int pos = ypos + x;
        image.data[pos] = val;
    }
    ypos = rect.downRight.y * image.w;
    for (int x = rect.topLeft.x; x <= rect.downRight.x; ++x)
    {
        int pos = ypos + x;
        image.data[pos] = val;
    }
}

/*
 * Draws the vertical borders of the rect in given image
 * param :
 *      rect: rectangle to draw
 *      image: image where grapgical result will be saved
 *      val: value to apply
 */
void DrawRect_ver(Rect rect, Image image, int val)
{
    int xpos = rect.topLeft.x;
    for (int y = rect.topLeft.y; y <= rect.downRight.y; ++y)
    {
        int pos = xpos + y * image.w;
        image.data[pos] = val;
    }
    xpos = rect.downRight.x;
    for (int y = rect.topLeft.y; y <= rect.downRight.y; ++y)
    {
        int pos = xpos + y * image.w;
        image.data[pos] = val;
    }
}

/*
 * Calling function applying the image segmentation to a given image struct
 * param :
 *      image: image where informations will be read
 *      newImage: bool to know if image shoould be modified or newly created
 */
Image Parse_Image(Image image, int newImage)
{
    Image result = image;
    if (newImage)
    {
        result = CopyImage(image);
    }
    Cord left;
    left.x = 0;
    left.y = 0;
    Cord right;
    right.x = image.w;
    right.y = image.h;
    Rect border;
    border.topLeft = left;
    border.downRight = right;

    // border = CutBorder(image);

    FILE *file = fopen("output.txt", "w+");

    if (file == NULL)
    {
        printf("Unexpected error when opening file !");
    }

    cutLine(image, border, result, file);

    fclose(file);

    return result;
}