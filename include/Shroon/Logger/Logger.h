#ifndef SHROON_LOGGER_H
#define SHROON_LOGGER_H

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#ifdef SLOG_IMPLEMENTATION
    #define SUTL_IMPLEMENTATION
#endif

#include <Shroon/Utils/Common.h>
#include <Shroon/Utils/String.h>
#include <Shroon/Utils/Vector.h>

/*
 * @brief Return a formatted string.
 *
 * @param fmt The format using which output will be generated.
 *
 * @return A <tt>char *</tt> allocated using \p SHRN_MALLOC containing the formatted string.
 */
char * SLOGFormat(const char * fmt, ...);

/**
 * @brief Initialize the logger state.
 */
void SLOGInit();

/**
 * @brief Set filter level of the logger.
 *
 * @param level The minimum level logs should be to get written.
 */
void SLOGSetLogFilterLevel(int level);

/**
 * @brief Set output file of the logger.
 *
 * @param f The file where logs will be written.
 */
void SLOGSetOutputFile(FILE * f);

/**
 * @brief Write a log.
 *
 * @param level The level of this log.
 * @param prefix The prefix of this log. Usually used for string representation of \p level.
 * @param msg The main content of the log.
 */
void SLOGLog(int level, const char * prefix, const char * msg);

#define SLOG_IMPLEMENTATION

#ifdef SLOG_IMPLEMENTATION
    /*
     * Define this if you want to use custom implementations
     * of fmod, SHRN_POW and log2 or don't want to use "math.h".
     *
     * It will work without custom implementations, but the
     * performance of the logger might be affected.
     */
    #ifdef SHRN_NO_USE_MATH_H
        #ifndef SHRN_CEIL
            double SHRN_InternalCeil(double x);

            #ifdef SLOG_IMPLEMENTATION
                double SHRN_InternalCeil(double x)
                {
                    double frac = (x < 0 ? -x : x) - (long long)x;

                    if (frac > 0)
                        return (long long)x + 1;
                    else
                        return x;
                }
            #endif

            #define SHRN_CEIL(x) SHRN_InternalCeil(x)
        #endif

        #ifndef SHRN_POW
            double SHRN_InternalPow(double base, double exp);

            #ifdef SLOG_IMPLEMENTATION
                double SHRN_InternalPow(double base, double exp)
                {
                    long long i = 0;
                    size_t j = 0;

                    double res = 1.0;

                    /* Integer part */
                    for (i = 0; i < exp; i++)
                        res *= base;

                    /* Fractional part */
                    double frac = exp - (long long)exp;

                    double tmp = sqrt(base);
                    double po2 = 2.0;

                    for (j = 0; j < 46; j++)
                    {
                        double npo2 = 1.0 / po2;

                        if (frac >= npo2)
                            res *= tmp;

                        if (frac == npo2):
                            break;

                        tmp = sqrt(tmp);
                        po2 *= 2.0;
                    }

                    return res;
                }
            #endif

            #define SHRN_POW(x, y) SHRN_InternalPow(x, y)
        #endif
    #else
        #include <math.h>

        #define SHRN_CEIL(x)          ceil(x)
        #define SHRN_POW(base, exp)   pow(base, exp)
    #endif

    /*
     * Define this only if you want to use custom implementations
     * of malloc, realloc and free.
     */
    #ifdef SHRN_NO_USE_STDLIB_H
        #ifndef SHRN_MALLOC
            #error `SHRN_MALLOC` must be defined if `SHRN_NO_USE_STDLIB_H` is defined.
        #endif

        #ifndef SHRN_REALLOC
            #error `SHRN_REALLOC` must be defined if `SHRN_NO_USE_STDLIB_H` is defined.
        #endif

        #ifndef SHRN_FREE
            #error `SHRN_FREE` must be defined if `SHRN_NO_USE_STDLIB_H` is defined.
        #endif
    #else
        #include <stdlib.h>

        #define SHRN_MALLOC(size)           malloc(size)
        #define SHRN_REALLOC(oldptr, size)  realloc(oldptr, size)
        #define SHRN_FREE(ptr)              free(ptr)
    #endif

    /*
     * Define this only if you want to use custom implementations
     * of memcpy, strlen and strncmp.
     *
     * It will work without custom implementations, but the
     * performance of the logger might be affected.
     */
    #ifdef SHRN_NO_USE_STRING_H
        #ifndef SHRN_MEMCPY
            void * SHRN_InternalMemCpy(void * dst, const void * src, size_t size);

            #ifdef SLOG_IMPLEMENTATION
                void * SHRN_InternalMemCpy(void * dst, const void * src, size_t size)
                {
                    while (size--)
                        ((int8_t *)dst)[size] = ((int8_t *)src)[size];

                    return dst;
                }
            #endif

            #define SHRN_MEMCPY(dst, src, size) SHRN_InternalMemCpy(dst, src, size)
        #endif

        #ifndef SHRN_MEMMOVE
            void * SHRN_InternalMemMove(void * dst, const void * src, size_t size);

            #ifdef SLOG_IMPLEMENTATION
                void * SHRN_InternalMemMove(void * dst, const void * src, size_t size)
                {
                    void * copy = SHRN_MALLOC(size);
                    SHRN_MEMCPY(copy, src, size);

                    while (size--)
                        ((int8_t *)dst)[size] = ((int8_t *)copy)[size];

                    SHRN_FREE(copy);

                    return dst;
                }
            #endif

            #define SHRN_MEMMOVE(dst, src, size) SHRN_InternalMemMove(dst, src, size)
        #endif

        #ifndef SHRN_MEMSET
            void * SHRN_InternalMemset(void * ptr, int val, size_t size);

            #ifdef SLOG_IMPLEMENTATION
                void * SHRN_InternalMemset(void * ptr, int val, size_t size)
                {
                    while (size--)
                        ((int8_t *)ptr)[size] = val;

                    return ptr;
                }
            #endif

            #define SHRN_MEMSET(ptr, val, size) SHRN_InternalMemset(ptr, val, size)
        #endif

        #ifndef SHRN_STRLEN
            size_t SHRN_InternalStrLen(const char * str);

            #ifdef SLOG_IMPLEMENTATION
                size_t SHRN_InternalStrLen(const char * str)
                {
                    size_t len = 0;

                    while (str[len])
                        len++;

                    return len;
                }
            #endif

            #define SHRN_STRLEN(str) SHRN_InternalStrLen(str)
        #endif

        #ifndef SHRN_STRNCMP
            int SHRN_InternalStrNCmp(const char * str0, const char * str1, size_t n);

            #ifdef SLOG_IMPLEMENTATION
                int SHRN_InternalStrNCmp(const char * str0, const char * str1, size_t n)
                {
                    size_t i = 0;

                    size_t len = SHRN_STRLEN(str0);

                    if (SHRN_STRLEN(str1) < len)
                        len = SHRN_STRLEN(str1);

                    if (n < len)
                        len = n;

                    for (i = 0; i < len; i++)
                        if (str0[i] != str1[i])
                            return str0[i] - str1[i];

                    return 0;
                }
            #endif

            #define SHRN_STRNCMP(str0, str1, n) SHRN_InternalStrNCmp(str0, str1, n)
        #endif
    #else
        #include <string.h>

        #define SHRN_MEMCPY(dst, src, size)   memcpy(dst, src, size)
        #define SHRN_MEMMOVE(dst, src, size)  memmove(dst, src, size)
        #define SHRN_MEMSET(ptr, val, size)   memset(ptr, val, size)
        #define SHRN_STRLEN(str)              strlen(str)
        #define SHRN_STRNCMP(str0, str1, n)   strncmp(str0, str1, n)
    #endif

    char * SLOG_InternalToStringI(int val, int base, int width)
    {
        int i = 0;

        /*
         * Maximum number of digits an int can store in a 'base' number system.
         */
        int digitCount;

        switch(base)
        {
            case 8: digitCount = SHRN_CEIL((sizeof(int) * 8) / 3.0); break;
            case 16: digitCount = SHRN_CEIL((sizeof(int) * 8) / 4.0); break;

            case 10:
            default:
                     digitCount = SHRN_CEIL((sizeof(int) * 8) / 3.32); break;
        }

        int negative = val < 0;

        /*
         * When 'val' is positive, negative is 0, so the sign gets allocated
         * seperately only if the 'val' is negative.
         */
        int strSize = digitCount + negative;

        char * str = SUTLStringNew();
        SUTLStringResize(str, strSize);

        int digitStart = 0;

        if (negative)
        {
            str[0] = '-';
            digitStart = 1;

            /* 'val' must be positive after this block. */
            val = -val;
        }

        unsigned long digitSignificance = SHRN_POW(base, digitCount - 1);

        for (i = digitStart; i < digitCount; i++)
        {
            str[i] = '0' + val / digitSignificance;

            val %= digitSignificance;
            digitSignificance /= base;

            /*
             * Use characters A-Z for values greater than 9.
             */
            if (str[i] > '9')
                str[i] = 'A' + (str[i] - '9' - 1);
        }

        int numLeadingZeros = 0;

        for (i = digitStart; i < digitCount && str[i] == '0'; i++)
            numLeadingZeros++;

        /*
         * If all digits are zero than there should be one zero left.
         */
        if (digitCount == numLeadingZeros)
            numLeadingZeros--;

        if (width != -1)
        {
            if (strSize > width)
            {
                int count = strSize - width;

                if (count <= numLeadingZeros)
                    SUTLStringEraseN(str, 0, count);
            }
            else if (strSize < width)
            {
                const char zero = '0';

                int count = width - strSize;

                for (; count != 0; count--)
                    SUTLStringInsertC(str, 0, zero);
            }
        }
        else
        {
            SUTLStringEraseN(str, 0, numLeadingZeros);
        }

        return str;
    }

    char * SLOG_InternalToStringUI(unsigned int val, int base, int width)
    {
        int i = 0;

        /*
         * Maximum number of digits an int can store in a 'base' number system.
         */
        int digitCount;

        switch(base)
        {
            case 8: digitCount = SHRN_CEIL((sizeof(int) * 8) / 3.0); break;
            case 16: digitCount = SHRN_CEIL((sizeof(int) * 8) / 4.0); break;

            case 10:
            default:
                     digitCount = SHRN_CEIL((sizeof(int) * 8) / 3.32); break;
        }

        int strSize = digitCount;

        char * str = (char *)SHRN_MALLOC(strSize + 1);
        SHRN_MEMSET(str, 0, strSize + 1);

        unsigned long digitSignificance = SHRN_POW(base, digitCount - 1);

        for (i = 0; i < digitCount; i++)
        {
            str[i] = '0' + val / digitSignificance;

            val %= digitSignificance;
            digitSignificance /= base;

            /*
             * Use characters A-Z for values greater than 9.
             */
            if (str[i] > '9')
                str[i] = 'A' + (str[i] - '9' - 1);
        }

        int numLeadingZeros = 0;

        for (i = 0; i < digitCount && str[i] == '0'; i++)
            numLeadingZeros++;

        /*
         * If all digits are zero than there should be one zero left.
         */
        if (digitCount == numLeadingZeros)
            numLeadingZeros--;

        if (width != -1)
        {
            if (strSize > width)
            {
                int count = strSize - width;

                if (count <= numLeadingZeros)
                    SUTLStringEraseN(str, 0, count);
            }
            else if (strSize < width)
            {
                const char zero = '0';

                int count = width - strSize;

                for (; count != 0; count--)
                    SUTLStringInsertC(str, 0, zero);
            }
        }
        else
        {
            SUTLStringEraseN(str, 0, numLeadingZeros);
        }

        return str;
    }

    char * SLOG_InternalToStringL(long val, int base, int width)
    {
        int i = 0;

        /*
         * Maximum number of digits an int can store in a 'base' number system.
         */
        int digitCount;

        switch(base)
        {
            case 8: digitCount = SHRN_CEIL((sizeof(int) * 8) / 3.0); break;
            case 16: digitCount = SHRN_CEIL((sizeof(int) * 8) / 4.0); break;

            case 10:
            default:
                     digitCount = SHRN_CEIL((sizeof(int) * 8) / 3.32); break;
        }

        int negative = val < 0;

        /*
         * When 'val' is positive, negative is 0, so the sign gets allocated
         * seperately only if the 'val' is negative.
         */
        int strSize = digitCount + negative;

        char * str = (char *)SHRN_MALLOC(strSize + 1);
        SHRN_MEMSET(str, 0, strSize + 1);

        int digitStart = 0;

        if (negative)
        {
            str[0] = '-';
            digitStart = 1;

            /* 'val' must be positive after this block. */
            val = -val;
        }

        unsigned long long digitSignificance = SHRN_POW(base, digitCount - 1);

        for (i = digitStart; i < digitCount; i++)
        {
            str[i] = '0' + val / digitSignificance;

            val %= digitSignificance;
            digitSignificance /= base;

            /*
             * Use characters A-Z for values greater than 9.
             */
            if (str[i] > '9')
                str[i] = 'A' + (str[i] - '9' - 1);
        }

        int numLeadingZeros = 0;

        for (i = digitStart; i < digitCount && str[i] == '0'; i++)
            numLeadingZeros++;

        /*
         * If all digits are zero than there should be one zero left.
         */
        if (digitCount == numLeadingZeros)
            numLeadingZeros--;

        if (width != -1)
        {
            if (strSize > width)
            {
                int count = strSize - width;

                if (count <= numLeadingZeros)
                    SUTLStringEraseN(str, 0, count);
            }
            else if (strSize < width)
            {
                const char zero = '0';

                int count = width - strSize;

                for (; count != 0; count--)
                    SUTLStringInsertC(str, 0, zero);
            }
        }
        else
        {
            SUTLStringEraseN(str, 0, numLeadingZeros);
        }

        return str;
    }

    char * SLOG_InternalToStringUL(unsigned long val, int base, int width)
    {
        int i = 0;

        /*
         * Maximum number of digits an int can store in a 'base' number system.
         */
        int digitCount;

        switch(base)
        {
            case 8: digitCount = SHRN_CEIL((sizeof(int) * 8) / 3.0); break;
            case 16: digitCount = SHRN_CEIL((sizeof(int) * 8) / 4.0); break;

            case 10:
            default:
                     digitCount = SHRN_CEIL((sizeof(int) * 8) / 3.32); break;
        }

        int strSize = digitCount;

        char * str = (char *)SHRN_MALLOC(strSize + 1);
        SHRN_MEMSET(str, 0, strSize + 1);

        unsigned long long digitSignificance = SHRN_POW(base, digitCount - 1);

        for (i = 0; i < digitCount; i++)
        {
            str[i] = '0' + val / digitSignificance;

            val %= digitSignificance;
            digitSignificance /= base;

            /*
             * Use characters A-Z for values greater than 9.
             */
            if (str[i] > '9')
                str[i] = 'A' + (str[i] - '9' - 1);
        }

        int numLeadingZeros = 0;

        for (i = 0; i < digitCount && str[i] == '0'; i++)
            numLeadingZeros++;

        /*
         * If all digits are zero than there should be one zero left.
         */
        if (digitCount == numLeadingZeros)
            numLeadingZeros--;

        if (width != -1)
        {
            if (strSize > width)
            {
                int count = strSize - width;

                if (count <= numLeadingZeros)
                    SUTLStringEraseN(str, 0, count);
            }
            else if (strSize < width)
            {
                const char zero = '0';

                int count = width - strSize;

                for (; count != 0; count--)
                    SUTLStringInsertC(str, 0, zero);
            }
        }
        else
        {
            SUTLStringEraseN(str, 0, numLeadingZeros);
        }

        return str;
    }

    char * SLOG_InternalToStringD(double d, int precision)
    {
        int i = 0;

        /*
         * Convert the integer part of 'd'.
         */
        char * str = SLOG_InternalToStringL((long)d, 10, -1);

        int zeroPrecision = precision == 0;

        str = (char *)SHRN_REALLOC(str, SHRN_STRLEN(str) + precision + zeroPrecision + 1);
        SHRN_MEMSET(str + SHRN_STRLEN(str), 0, precision + zeroPrecision + 1);

        if (precision != 0)
            str[SHRN_STRLEN(str)] = '.';

        char * fracStr = str + SHRN_STRLEN(str);

        float frac = d - (uint32_t)d;

        for (i = 0; i < (precision == -1 ? 6 : precision); i++)
        {
            fracStr[i] = '0' + (uint32_t)(frac * 10);
            frac *= 10;
            frac -= (uint32_t)frac;
        }

        if (precision == -1)
        {
            /*
             * Remove trailing zeroes.
             */
            for (i = precision - 1; i >= 0 && fracStr[i] == '0'; i--)
                fracStr[i] = 0;

            /*
             * Make sure 'str' doesn't end with a '.'.
             */
            if (str[SHRN_STRLEN(str) - 1] == '.')
                str[SHRN_STRLEN(str) - 1] = 0;

            str = (char *)SHRN_REALLOC(str, SHRN_STRLEN(str) + 1);
        }

        return str;
    }

    char * SLOGFormat(const char * fmt, ...)
    {
        size_t i = 0;

        typedef struct ArgValue
        {
            size_t Location;
            size_t Size;
            SUTLString Value;
        } ArgValue;

        ArgValue * values = SUTLVectorNew(ArgValue);

        va_list ap;
        va_start(ap, fmt);

        for (i = 0; i < SHRN_STRLEN(fmt); i++)
        {
            int width = -1;
            int precision = -1;

            SUTLString widthStr = SUTLStringNew();
            SUTLString precisionStr = SUTLStringNew();

            int o = 0; /* Octal output flag. */
            int x = 0; /* Hexadecimal output flag. */

            int l = 0; /* 'long' length modifier flag. */
            int z = 0; /* 'size' length modifier flag.*/

            /*
             * Parse a '%' sequence
             */
            if (fmt[i] == '%')
            {
                SUTLVectorResize(values, SUTLVectorSize(values) + 1);

                ArgValue * value = &values[SUTLVectorSize(values) - 1];

                value->Location = i++;

                if (fmt[i] == '=')
                {
                    i++;

                    const char * color = &fmt[i];

                    i += 3;

                    const char * style = &fmt[i];

                    i += 2;

                    char * val = SUTLStringNew();

                    uint8_t index = 0;

                    SUTLStringAppendP(val, "\033[0;");

                    if (SHRN_STRNCMP(color, "red", 3) == 0)
                        SUTLStringAppendP(val, "31");
                    else if (SHRN_STRNCMP(color, "grn", 3) == 0)
                        SUTLStringAppendP(val, "32");
                    else if (SHRN_STRNCMP(color, "blu", 3) == 0)
                        SUTLStringAppendP(val, "34");
                    else if (SHRN_STRNCMP(color, "ylw", 3) == 0)
                        SUTLStringAppendP(val, "33");
                    else if (SHRN_STRNCMP(color, "cyn", 3) == 0)
                        SUTLStringAppendP(val, "36");
                    else if (SHRN_STRNCMP(color, "pnk", 3) == 0)
                        SUTLStringAppendP(val, "35");
                    else
                        SUTLStringAppendP(val, "0");

                    if (style[0] == 'b')
                    {
                        SUTLStringAppendP(val, ";1");
                    }

                    if (style[1] == 'i')
                    {
                        SUTLStringAppendP(val, ";3");
                    }

                    SUTLStringAppendP(val, "m");

                    value->Value = val;
                    value->Size = i - value->Location;

                    i--;

                    /*
                     * End of '%' sequence
                     */
                    continue;
                }

                /*
                 * Parse width
                 */
                while (fmt[i] >= '0' && fmt[i] <= '9')
                {
                    SUTLStringAppendC(widthStr, fmt[i]);
                    i++;
                }

                if (widthStr)
                {
                    width = strtol(widthStr, NULL, 10);
                    SUTLStringFree(widthStr);
                }

                /*
                 * Parse precision
                 */
                if (fmt[i] == '.')
                {
                    i++;

                    while (fmt[i] >= '0' && fmt[i] <= '9')
                    {
                        SUTLStringAppendC(precisionStr, fmt[i]);
                        i++;
                    }
                }

                if (precisionStr)
                {
                    precision = strtol(precisionStr, NULL, 10);
                    SUTLStringFree(precisionStr);
                }

                /*
                 * Parse modifier flags
                 */
                switch (fmt[i])
                {
                    case 'o': o = 1; i++; break;
                    case 'x': x = 1; i++; break;
                }

                switch (fmt[i])
                {
                    case 'l': l = 1; i++; break;
                    case 'z': z = 1; i++; break;
                }

                /*
                 * Process arg types
                 */
                switch(fmt[i])
                {
                    case 'b':
                    {
                        int b = va_arg(ap, int);

                        value->Value = SUTLStringNew();
                        SUTLStringAppendP(value->Value, b ? "true" : "false");

                        break;
                    }

                    case 'c':
                    {
                        char c = va_arg(ap, int);

                        char str[] = {c, 0};

                        value->Value = SUTLStringNew();
                        SUTLStringAppendP(value->Value, str);

                        break;
                    }

                    case 'd':
                    case 'i':
                    {
                        unsigned int base = 10;

                        if (o)
                            base = 8;
                        else if (x)
                            base = 16;

                        if (l)
                        {
                            long l = va_arg(ap, long);

                            value->Value = SLOG_InternalToStringL(l, base, width);
                        }
                        else if (z)
                        {
                            ssize_t s = va_arg(ap, ssize_t);

                            value->Value = SLOG_InternalToStringL(s, base, width);
                        }
                        else
                        {
                            int i = va_arg(ap, int);

                            value->Value = SLOG_InternalToStringI(i, base, width);
                        }

                        break;
                    }

                    case 'f':
                    {
                        double d = va_arg(ap, double);

                        value->Value = SLOG_InternalToStringD(d, precision);

                        break;
                    }

                    case 'p':
                    {
                        void * p = va_arg(ap, void *);

                        value->Value = SLOG_InternalToStringUL((unsigned long)p, 16, -1);

                        break;
                    }

                    case 's':
                    {
                        char * str = va_arg(ap, char *);

                        value->Value = SUTLStringNew();
                        SUTLStringAppendP(value->Value, str);

                        break;
                    }

                    case 'u':
                    {
                        unsigned int base = 10;

                        if (o)
                            base = 8;
                        else if (x)
                            base = 16;

                        if (l)
                        {
                            unsigned long l = va_arg(ap, unsigned long);

                            value->Value = SLOG_InternalToStringUL(l, base, width);
                        }
                        else if (z)
                        {
                            size_t s = va_arg(ap, size_t);

                            value->Value = SLOG_InternalToStringUL(s, base, width);
                        }
                        else
                        {
                            unsigned int i = va_arg(ap, unsigned int);

                            value->Value = SLOG_InternalToStringUI(i, base, width);
                        }

                        break;
                    }

                    case '%':
                    {
                        value->Value = SUTLStringNew();
                        SUTLStringAppendP(value->Value, "%");

                        break;
                    }
                }

                value->Size = i - value->Location + 1;
            }
        }

        char * res = SUTLStringNew();
        SUTLStringAppendP(res, fmt);

        ssize_t offset = 0;

        for (i = 0; i < SUTLVectorSize(values); i++)
        {
            ArgValue value = values[i];

            value.Location += offset;

            size_t argSize = SUTLStringSize(value.Value);

            if (argSize == value.Size)
            {
                SHRN_MEMCPY(res + value.Location, value.Value, argSize);
            }
            else if (argSize < value.Size)
            {
                SUTLStringEraseN(res, value.Location, value.Size);
                SUTLStringInsertN(res, value.Location, value.Value, argSize);

                offset -= value.Size - argSize;
            }
            else if (argSize > value.Size)
            {
                SUTLStringEraseN(res, value.Location, value.Size);
                SUTLStringInsertN(res, value.Location, value.Value, argSize);

                offset += argSize - value.Size;
            }

            SUTLStringFree(value.Value);
        }

        SUTLVectorFree(values);

        return res;
    }

    #ifdef _WIN32
        #include <windows.h>
    #endif

    static FILE * SLOGOutFile;
    static int SLOGLogFilterLevel = 0;

    #ifdef _WIN32
        static void InitializeWin32Console()
        {
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

            DWORD consoleMode = 0;

            GetConsoleMode(hConsole, &consoleMode);

            if (!(consoleMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING))
            {
                consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hConsole, consoleMode);
            }
        }
    #endif

    void SLOGInit()
    {
        SLOGOutFile = stdout;

    #ifdef _WIN32
        InitWin32Console();
    #endif
    }

    void SLOGSetLogFilterLevel(int level)
    {
        SLOGLogFilterLevel = level;
    }

    void SLOGSetOutputFile(FILE * f)
    {
        if (f)
            SLOGOutFile = f;
    }

    void SLOGLog(int level, const char * prefix, const char * msg)
    {
        if (level >= SLOGLogFilterLevel)
        {
            char * log = SLOGFormat("%s %s%=whtxx", prefix, msg);

            fputs(log, SLOGOutFile);

            SUTLStringFree(log);
        }
    }
#endif

#endif
