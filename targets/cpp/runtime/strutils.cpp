// Copyright (c) 2016 IBM Corporation.
#include "strutils.h"
#include "term.h"
#include <cstring>

// Interpret one Unicode relaxed UTF-8 character starting at s into codepoint c.
// NOTE: leaves s on last character in sequence (this leaves s untouched for 7-bit characters)
static unsigned int onecodepoint(unsigned char **sp)
{
    unsigned int c = **sp;
    if (c <= 0x7F) // .7 bit: U+0000     U+007F     1     0xxxxxxx
    {}
    else if ((c & 0xC0) == 0x80) // continuation character out of place -- assume normal
    {}
    else if ((c & 0xE0) == 0xC0) // 11 bit: U+0080 U+07FF    2 110xxxxx 10xxxxxx
    {
        if ((*((*sp)+1) & 0xC0) == 0x80) // if not continued assume roque 8-bit!
        {
            c = (c & 0x1F) << 6;
            c |= *(++(*sp)) & 0x3F;
        }
    }
    else if ((c & 0xF0) == 0xE0) // 16 bit: U+0800 U+FFFF 3 1110xxxx 10xxxxxx 10xxxxxx
    {
        if ((*((*sp)+1) & 0xC0) == 0x80) // if not continued assume roque 8-bit!
        {
            c = (c & 0x1F) << 12;
            c |= (*(++(*sp)) & 0x3F) << 6;
            if ((*((*sp)+1) & 0xC0) == 0x80) c |= *(++(*sp)) & 0x3F;
        }
    }
    else if ((c & 0xF8) == 0xF0) // 21 bit: U+10000 U+1FFFFF 4     11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    {
        if ((*((*sp)+1) & 0xC0) == 0x80) // if not continued assume roque 8-bit!
        {
            c = (c & 0x1F)<<18;
            c |= (*(++(*sp)) & 0x3F) << 12;
            if ((*((*sp)+1) & 0xC0) == 0x80) c |= (*(++(*sp)) & 0x3F) << 6;
            if ((*((*sp)+1) & 0xC0) == 0x80) c |= *(++(*sp)) & 0x3F;
        }
    }
    else if ((c & 0xFC) == 0xF8) // 26 bit: U+200000 U+3FFFFFF 5 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
    {
        if ((*((*sp)+1) & 0xC0) == 0x80) // if not continued assume roque 8-bit!
        {
            c = (c & 0x1F)<<24;
            c |= (*(++(*sp)) & 0x3F) << 18;
            if ((*((*sp)+1) & 0xC0) == 0x80) c |= (*(++(*sp)) & 0x3F) << 12;
            if ((*((*sp)+1) & 0xC0) == 0x80) c |= (*(++(*sp)) & 0x3F) << 6;
            if ((*((*sp)+1) & 0xC0) == 0x80) c |= *(++(*sp)) & 0x3F;
        }
    }
    else if ((c & 0xFE) == 0xFC) // 31 bit: U+4000000 U+7FFFFFFF 6 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
    {
        if ((*((*sp)+1) & 0xC0) == 0x80) // if not continued assume roque 8-bit!
        {
            c = (c & 0x1F)<<30;
            c |= (*(++(*sp)) & 0x3F) << 24;
            if ((*((*sp)+1) & 0xC0) == 0x80) c |= (*(++(*sp)) & 0x3F) << 18;
            if ((*((*sp)+1) & 0xC0) == 0x80) c |= (*(++(*sp)) & 0x3F) << 12;
            if ((*((*sp)+1) & 0xC0) == 0x80) c |= (*(++(*sp)) & 0x3F) << 6;
            if ((*((*sp)+1) & 0xC0) == 0x80) c |= *(++(*sp)) & 0x3F;
        }
    }
    return c;
}


// Convert UTF-8 chars to external escaped string form.
// All characters in *sourcep are converted into characters starting at *targetp,
// not going beyond endtarget. Updates *sourcep and *targetp.
static const char HexDigits[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
static void escape(char **sourcep, char **targetp, char *endsource, char *endtarget)
{
    unsigned char *s = (unsigned char *)*sourcep;
    unsigned char *t = (unsigned char *)*targetp;
    for (; *s && s < (unsigned char *)endsource && t < (unsigned char *)endtarget-5; ++s)
    {
        unsigned int c = *s;
        switch (c)
        {
        case '\"' : *(t++) = '\\'; *(t++) = '\"'; break;
        case '\\' : *(t++) = '\\'; *(t++) = '\\'; break;
        case '\n' : *(t++) = '\\'; *(t++) = 'n'; break;
        case '\r' : *(t++) = '\\'; *(t++) = 'r'; break;
        case '\f' : *(t++) = '\\'; *(t++) = 'f'; break;
        case '\a' : *(t++) = '\\'; *(t++) = 'a'; break;
        case '\t' : *(t++) = '\\'; *(t++) = 't'; break;

        default :
            c = onecodepoint(&s);
            // Ready to externalize
            if (c >= ' ' && c <= '~')
            {
                *(t++) = c;
            }
            else if (c <= 0x7F) // non-printable 7 bit characters printed as octal.
            {
                *(t++) = '\\'; *(t++) = '0' + ((c>>6)&0x7); *(t++) = '0' + ((c>>3)&0x7); *(t++) = '0' + (c&0x7);
            }
            else if (c <= 0xFFFF) // most Unicode escapes
            {
                *(t++) = '\\'; *(t++) = 'u';
                *(t++) = HexDigits[(c>>12)&0xF]; *(t++) = HexDigits[(c>>8)&0xF]; *(t++) = HexDigits[(c>>4)&0xF]; *(t++) = HexDigits[c&0xF];
            }
            else // extreme Unicode
            {
                *(t++) = '\\'; *(t++) = 'U';
                *(t++) = HexDigits[(c>>28)&0xF]; *(t++) = HexDigits[(c>>24)&0xF]; *(t++) = HexDigits[(c>>20)&0xF]; *(t++) = HexDigits[(c>>16)&0xF];
                *(t++) = HexDigits[(c>>12)&0xF]; *(t++) = HexDigits[(c>>8)&0xF]; *(t++) = HexDigits[(c>>4)&0xF]; *(t++) = HexDigits[c&0xF];
            }
        }
    }
    *sourcep = (char*)s;
    *targetp = (char*)t;
}

/**
 *  Convert UTF-8 chars to external escaped string form.
 */
std::string& makeEscaped(tosca::Context& context, const char *src)
{
    size_t src_length = strlen(src);
    size_t tmp_length = src_length*10+3; // enough space even if all are quotes!
    char *tmp = (char *) alloca(tmp_length+1);
    char *s = (char*)src;
    char *t = tmp;
    *(t++) = '"';
    escape(&s, &t, ((char*)src)+src_length, tmp+tmp_length-2);
    *(t++) = '"';
    *(t++) = '\0';
    return *(new std::string(tmp));
}

/**
 * Mangle the given name to be a valid Java/C/C++ identifier
 */
std::string& makeMangle(tosca::Context& context, const std::string& src)
{
    std::string::size_type length = src.size();
    std::string& mangled = *(new std::string(src));
    for (std::string::size_type i = 0; i < length; ++i)
    {
        char c = src[i];
        mangled[i] = (c == '/' ? '_' : c);
    }
    return mangled;
}