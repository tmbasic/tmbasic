/*------------------------------------------------------------*/
/* filename -       edits.cpp                                 */
/*                                                            */
/* function(s)                                                */
/*                  TEditor member functions                  */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TEditor
#define Uses_TText
#include <tvision/tv.h>

#include <ctype.h>

char TEditor::bufChar( uint P )
{
    return buffer[bufPtr(P)];
}

uint TEditor::bufPtr( uint P )
{
    return P < curPtr ? P : P + gapLen;
}

void TEditor::formatLine( TScreenCell *DrawBuf,
                          uint P,
                          int Width,
                          ushort Colors
                        )
{
    const struct { uchar color; uint end; } ranges[] =
    {
        // The attributes for normal text are in the lower byte of 'Colors'.
        // The attributes for text selection are in the upper byte.
        { uchar(Colors), selStart },
        { uchar(Colors >> 8), selEnd },
        { uchar(Colors), bufLen }
    };

    uchar Color;
    TSpan<TScreenCell> Cells(DrawBuf, Width);
    int X = 0;
    for (int r = 0; r < 3; ++r)
    {
        Color = ranges[r].color;
        while (P < ranges[r].end && X < Width)
        {
            TStringView chars = bufChars(P);
            uchar Char = chars[0];
            if (Char == '\r' || Char == '\n')
                goto fill;
            if (Char == '\t') {
                do {
                    ::setCell(Cells[X++], ' ', Color);
                } while (X%8 != 0 && X < Width);
                ++P;
            } else
                formatCell(Cells.subspan(X), (uint&) X, chars, P, Color);
        }
    }
fill:
    while (X < Width)
        ::setCell(Cells[X++], ' ', Color);
}

uint TEditor::lineEnd( uint P )
{
    for (uint i = P; i < bufLen; ++i)
    {
        char c = bufChar(i);
        if (c == '\r' || c == '\n')
            return i;
    }
    return bufLen;
}

uint TEditor::lineStart( uint P )
{
    uint i = P;
    while (i--)
    {
        char c = bufChar(i);
        if (c == '\r')
        {
            if ( i + 1 != curPtr && i + 1 != bufLen &&
                 bufChar(i + 1) == '\n' )
                return i + 2;
            return i + 1;
        }
        else if (c == '\n')
            return i + 1;
    }
    return 0;
}

uint TEditor::nextChar( uint P )
{
    if (P + 1 < bufLen)
    {
        if (bufChar(P) == '\r' && bufChar(P + 1) == '\n')
            return P + 2;
        if (encSingleByte)
            return P + 1;
        else
            return P + TText::next(bufChars(P));
    }
    return bufLen;
}

uint TEditor::prevChar( uint P )
{
    if (P > 1)
    {
        if (bufChar(P - 2) == '\r' && bufChar(P - 1) == '\n')
            return P - 2;
        if (encSingleByte)
            return P - 1;
        else
        {
            TStringView t = bufPrevChars(P);
            return P - TText::prev(t, t.size());
        }
    }
    return 0;
}

extern "C" {

int countLines( const char *buf, uint count )
{
    int lines = 0;
    for (uint i = 0; i < count; ++i)
        if (buf[i] == '\r')
        {
            ++lines;
            if (i+1 < count && buf[i+1] == '\n')
                ++i;
        }
        else if (buf[i] == '\n')
            ++lines;
    return lines;
}

uint scan( const char *block, uint size, const char *str )
{
    uint i = 0, len = strlen(str);
    if (len > 0)
        while (i < size)
        {
            uint j = i, k = 0;
            while (j < size && block[j++] == str[k])
                if (++k == len)
                    return i;
            ++i;
        }
    return sfSearchFailed;
}

uint iScan( const char *block, uint size, const char *str )
{
    uint i = 0, len = strlen(str);
    if (len > 0)
        while (i < size)
        {
            uint j = i, k = 0;
            while (j < size && toupper(block[j++]) == toupper(str[k]))
                if (++k == len)
                    return i;
            ++i;
        }
    return sfSearchFailed;
}

}
