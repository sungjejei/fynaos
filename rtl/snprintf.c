// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * snprintf() implementation.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#include <fynaos/types.h>
#include <fynaos/rtl.h>
#include <stdarg.h>

#define ZEROPAD   1
#define LEFTALIGN 2
#define PLUS      4
#define SPECIAL   8
#define SIGN      16
#define LOWERCASE 32
#define HEX       64
#define OCTA      128
#define BIN       256

#define LEN_HH      1
#define LEN_H       2
#define LEN_DEFAULT 3
#define LEN_L       4
#define LEN_LL      5

/*
 * This routine prints the number in the string.
 * Caller passes two versions of a number, signed and unsigned.
 * If the 'SIGN' flag is set, this routine will use snumber.
 * else, will use unumber.
 * 
 * If the buffer is not enough, this routine still returns
 * increased buffer address.
 * 
 * 'limit' specifies pointer to address following
 * last usable space of the buffer. So, the space limit points is unusable.
 * 
 * This routine returns increased buffer address.
 * It is NOT safe because it can be greater than limit.
 * This is because 'snprintf' routine must return lenght needed no matter what.
 */
static
char *print_number(uint64_t unumber, int64_t snumber,
                   unsigned int flags, unsigned int width,
                   char *buffer, const char *limit)
{
    char         tmp[66];
    char        *tmphead  = tmp;
    char        *tmptail  = tmp;
    unsigned int tmplen   = 0;
    uint64_t     base     = 0;
    const char  *digits   = flags & LOWERCASE ? "0123456789abcdef" : "0123456789ABCDEF";
    unsigned int pad      = 0;
    char         fill     = (flags & ZEROPAD) && !(flags & LEFTALIGN) ? '0' : ' ';
    boolean_t    negative = FALSE;

    if      (flags & HEX)  base = 16;
    else if (flags & OCTA) base = 8;
    else if (flags & BIN)  base = 2;
    else                   base = 10;

    /*
     * Convert snumber to unumber
     */

    if (flags & SIGN)
    {
        if (snumber < 0)
        {
            unumber = ~(uint64_t)snumber + 1;
            negative = TRUE;
        }
        else
        {
            unumber = (uint64_t)snumber;
        }
    }

    /*
     * Convert unumber to string in tmp buffer
     */

    if (unumber == 0)
    {
        *tmptail++ = '0';
    }
    else while (unumber)
    {
        *tmptail++ = digits[unumber % base];
        unumber /= base;
    }

    tmplen = (unsigned int)(tmptail - tmp);

    /*
     * Reverse tmp
     */

    tmptail--;

    while (tmphead < tmptail)
    {
        char a = *tmphead;
        *tmphead = *tmptail;
        *tmptail = a;

        tmphead++;
        tmptail--;
    }

    /*
     * Compute pad
     */

    unsigned int total = tmplen + (negative ? 1 : 0);

    pad = width > total
        ? width - total
        : 0;

    /*
     * Print
     */

    tmphead = tmp;

    if (buffer < limit && pad + total + 1 <= limit - buffer)
    {
        if (flags & LEFTALIGN)
        {
            if (negative)    *buffer++ = '-';
            while (tmplen--) *buffer++ = *tmphead++;
            while (pad--)    *buffer++ = fill;
        }
        else
        {
            if (fill == ' ')
            {
                while (pad--)    *buffer++ = fill;
                if (negative)    *buffer++ = '-';
                while (tmplen--) *buffer++ = *tmphead++;
            }
            else
            {
                if (negative)    *buffer++ = '-';
                while (pad--)    *buffer++ = fill;
                while (tmplen--) *buffer++ = *tmphead++;
            }

        }
    }
    else
    {
        buffer += pad + tmplen + (negative ? 1 : 0);
    }

    if (buffer < limit) *buffer = '\0';

    return buffer;
}

size_t vsnprintf(char *buffer, size_t length, const char *format, va_list args)
{
    char *limit = buffer + length;
    char *base  = buffer;

    while (*format)
    {
        if (*format == '%')
        {
            unsigned int flags = 0;
            unsigned int width = 0;
            int          len;

            format++; // skip '%'

            for (;; format++)
            {
                switch (*format)
                {
                case '0':
                    flags |= ZEROPAD;
                    continue;

                case '-':
                    flags |= LEFTALIGN;
                    continue;
                }

                break;
            }

            while (*format >= '0' && *format <= '9')
            {
                width = (width * 10) + (unsigned int)(*format - '0');
                format++;
            }

            if (*format == 'l')
            {
                if (*(format + 1) == 'l')
                {
                    len = LEN_LL;
                    format += 2;
                }
                else
                {
                    len = LEN_L;
                    format += 1;
                }
            }
            else if (*format == 'h')
            {
                if (*(format + 1) == 'h')
                {
                    len = LEN_HH;
                    format += 2;
                }
                else
                {
                    len = LEN_H;
                    format += 1;
                }
            }
            else
            {
                len = LEN_DEFAULT;
            }

            switch (*format)
            {
            case 's':
            {
                const char *str = va_arg(args, const char *);

                while (*str)
                {
                    if (buffer + 1 < limit)
                    {
                        *buffer = *str;
                    }
                    buffer++;
                    str++;
                }
                break;
            }

            case 'c':
                if (buffer + 1 < limit)
                {
                    *buffer++ = (char)va_arg(args, int);
                }
                else
                {
                    buffer++;
                }
                break;


            case 'd':
                flags |= SIGN;

                switch (len)
                {
                case LEN_HH:
                case LEN_H:
                case LEN_DEFAULT:
                    buffer = print_number(0, (int64_t)va_arg(args, int),
                                          flags, width,
                                          buffer, limit);
                    break;
                    
                case LEN_L:
                case LEN_LL:
                    buffer = print_number(0, (int64_t)va_arg(args, long long),
                                          flags, width,
                                          buffer, limit);
                    break;
                }
                break;

            case 'u':
                switch (len)
                {
                case LEN_HH:
                case LEN_H:
                case LEN_DEFAULT:
                    buffer = print_number((uint64_t)va_arg(args, int), 0,
                                          flags, width,
                                          buffer, limit);
                    break;
                    
                case LEN_L:
                case LEN_LL:
                    buffer = print_number((uint64_t)va_arg(args, long long), 0,
                                          flags, width,
                                          buffer, limit);
                    break;
                }
                break;

            case 'x':
                flags |= LOWERCASE;
                __fallthrough;
            case 'X':
                flags |= HEX;

                switch (len)
                {
                case LEN_HH:
                case LEN_H:
                case LEN_DEFAULT:
                    buffer = print_number((uint64_t)va_arg(args, int), 0,
                                          flags, width,
                                          buffer, limit);
                    break;
                    
                case LEN_L:
                case LEN_LL:
                    buffer = print_number((uint64_t)va_arg(args, long long), 0,
                                          flags, width,
                                          buffer, limit);
                    break;
                }
                break;

            case 'o':
                flags |= OCTA;

                switch (len)
                {
                case LEN_HH:
                case LEN_H:
                case LEN_DEFAULT:
                    buffer = print_number((uint64_t)va_arg(args, int), 0,
                                          flags, width,
                                          buffer, limit);
                    break;
                    
                case LEN_L:
                case LEN_LL:
                    buffer = print_number((uint64_t)va_arg(args, long long), 0,
                                          flags, width,
                                          buffer, limit);
                    break;
                }
                break;

            case 'b':
                flags |= BIN;

                switch (len)
                {
                case LEN_HH:
                case LEN_H:
                case LEN_DEFAULT:
                    buffer = print_number((uint64_t)va_arg(args, int), 0,
                                          flags, width,
                                          buffer, limit);
                    break;
                    
                case LEN_L:
                case LEN_LL:
                    buffer = print_number((uint64_t)va_arg(args, long long), 0,
                                          flags, width,
                                          buffer, limit);
                    break;
                }
                break;

            case 'p':
                flags |= HEX;

                if (buffer + 2 < limit)
                {
                    buffer[0] = '0';
                    buffer[1] = 'x';
                    buffer += 2;
                }

                buffer = print_number((uint64_t)va_arg(args, void*), 0,
                                    flags, width,
                                    buffer, limit);
                break;

            case '%':
                if (buffer + 1 < limit)
                {
                    *buffer = '%';
                }
                buffer++;
                break;

            default:
                if (buffer + 3 < limit)
                {
                    buffer[0] = '%';
                    buffer[1] = *format;
                }
                buffer += 2;
            }
        }
        else
        {
            if (buffer + 1 < limit) *buffer++ = *format;
        }
        format++;
    }
    
    if (buffer < limit)
    {
        *buffer = '\0';
    }
    else
    {
        *((char*)limit - 1) = '\0';
    }

    return (size_t)(buffer - base);
}

size_t snprintf(char *buffer, size_t len, const char *format, ...)
{
    size_t ret;
    va_list args;
    va_start(args, format);

    ret = vsnprintf(buffer, len, format, args);

    va_end(args);
    return ret;
}
