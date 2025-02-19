/* SDSLib 2.0 -- A C dynamic strings library
 *
 * Copyright (c) 2006-2015, Salvatore Sanfilippo <antirez at gmail dot com>
 * Copyright (c) 2015, Oran Agra
 * Copyright (c) 2015, Redis Labs, Inc
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"

#ifndef __SDS_H
#define __SDS_H

#define SDS_MAX_PREALLOC (1024 * 1024)
extern const char* SDS_NOINIT;

#include "titus/export.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

typedef char* sds;

/* Note: sdshdr5 is never used, we just access the flags byte directly.
 * However is here to document the layout of type 5 SDS strings. */
struct __attribute__((__packed__)) sdshdr5 {
    unsigned char flags; /* 3 lsb of type, and 5 msb of string length */
    char buf[];
};
struct __attribute__((__packed__)) sdshdr8 {
    uint8_t len;         /* used */
    uint8_t alloc;       /* excluding the header and null terminator */
    unsigned char flags; /* 3 lsb of type, 5 unused bits */
    char buf[];
};
struct __attribute__((__packed__)) sdshdr16 {
    uint16_t len;        /* used */
    uint16_t alloc;      /* excluding the header and null terminator */
    unsigned char flags; /* 3 lsb of type, 5 unused bits */
    char buf[];
};
struct __attribute__((__packed__)) sdshdr32 {
    uint32_t len;        /* used */
    uint32_t alloc;      /* excluding the header and null terminator */
    unsigned char flags; /* 3 lsb of type, 5 unused bits */
    char buf[];
};
struct __attribute__((__packed__)) sdshdr64 {
    uint64_t len;        /* used */
    uint64_t alloc;      /* excluding the header and null terminator */
    unsigned char flags; /* 3 lsb of type, 5 unused bits */
    char buf[];
};

#define SDS_TYPE_5 0
#define SDS_TYPE_8 1
#define SDS_TYPE_16 2
#define SDS_TYPE_32 3
#define SDS_TYPE_64 4
#define SDS_TYPE_MASK 7
#define SDS_TYPE_BITS 3
#define SDS_HDR_VAR(T, s) struct sdshdr##T* sh = (void*)((s) - (sizeof(struct sdshdr##T)));
#define SDS_HDR(T, s) ((struct sdshdr##T*)((s) - (sizeof(struct sdshdr##T))))
#define SDS_TYPE_5_LEN(f) ((f) >> SDS_TYPE_BITS)

static inline size_t sdslen(const sds s) {
    unsigned char flags = s[-1];
    switch(flags & SDS_TYPE_MASK) {
    case SDS_TYPE_5:
        return SDS_TYPE_5_LEN(flags);
    case SDS_TYPE_8:
        return SDS_HDR(8, s)->len;
    case SDS_TYPE_16:
        return SDS_HDR(16, s)->len;
    case SDS_TYPE_32:
        return SDS_HDR(32, s)->len;
    case SDS_TYPE_64:
        return SDS_HDR(64, s)->len;
    }
    return 0;
}

static inline size_t sdsavail(const sds s) {
    unsigned char flags = s[-1];
    switch(flags & SDS_TYPE_MASK) {
    case SDS_TYPE_5: {
        return 0;
    }
    case SDS_TYPE_8: {
        SDS_HDR_VAR(8, s);
        return sh->alloc - sh->len;
    }
    case SDS_TYPE_16: {
        SDS_HDR_VAR(16, s);
        return sh->alloc - sh->len;
    }
    case SDS_TYPE_32: {
        SDS_HDR_VAR(32, s);
        return sh->alloc - sh->len;
    }
    case SDS_TYPE_64: {
        SDS_HDR_VAR(64, s);
        return sh->alloc - sh->len;
    }
    }
    return 0;
}

static inline void sdssetlen(sds s, size_t newlen) {
    unsigned char flags = s[-1];
    switch(flags & SDS_TYPE_MASK) {
    case SDS_TYPE_5: {
        unsigned char* fp = ((unsigned char*)s) - 1;
        *fp               = SDS_TYPE_5 | (newlen << SDS_TYPE_BITS);
    } break;
    case SDS_TYPE_8:
        SDS_HDR(8, s)->len = newlen;
        break;
    case SDS_TYPE_16:
        SDS_HDR(16, s)->len = newlen;
        break;
    case SDS_TYPE_32:
        SDS_HDR(32, s)->len = newlen;
        break;
    case SDS_TYPE_64:
        SDS_HDR(64, s)->len = newlen;
        break;
    }
}

static inline void sdsinclen(sds s, size_t inc) {
    unsigned char flags = s[-1];
    switch(flags & SDS_TYPE_MASK) {
    case SDS_TYPE_5: {
        unsigned char* fp    = ((unsigned char*)s) - 1;
        unsigned char newlen = SDS_TYPE_5_LEN(flags) + inc;
        *fp                  = SDS_TYPE_5 | (newlen << SDS_TYPE_BITS);
    } break;
    case SDS_TYPE_8:
        SDS_HDR(8, s)->len += inc;
        break;
    case SDS_TYPE_16:
        SDS_HDR(16, s)->len += inc;
        break;
    case SDS_TYPE_32:
        SDS_HDR(32, s)->len += inc;
        break;
    case SDS_TYPE_64:
        SDS_HDR(64, s)->len += inc;
        break;
    }
}

/* sdsalloc() = sdsavail() + sdslen() */
static inline size_t sdsalloc(const sds s) {
    unsigned char flags = s[-1];
    switch(flags & SDS_TYPE_MASK) {
    case SDS_TYPE_5:
        return SDS_TYPE_5_LEN(flags);
    case SDS_TYPE_8:
        return SDS_HDR(8, s)->alloc;
    case SDS_TYPE_16:
        return SDS_HDR(16, s)->alloc;
    case SDS_TYPE_32:
        return SDS_HDR(32, s)->alloc;
    case SDS_TYPE_64:
        return SDS_HDR(64, s)->alloc;
    }
    return 0;
}

static inline void sdssetalloc(sds s, size_t newlen) {
    unsigned char flags = s[-1];
    switch(flags & SDS_TYPE_MASK) {
    case SDS_TYPE_5:
        /* Nothing to do, this type has no total allocation info. */
        break;
    case SDS_TYPE_8:
        SDS_HDR(8, s)->alloc = newlen;
        break;
    case SDS_TYPE_16:
        SDS_HDR(16, s)->alloc = newlen;
        break;
    case SDS_TYPE_32:
        SDS_HDR(32, s)->alloc = newlen;
        break;
    case SDS_TYPE_64:
        SDS_HDR(64, s)->alloc = newlen;
        break;
    }
}

TITUS_EXPORT sds sdsnewlen(const void* init, size_t initlen);
TITUS_EXPORT sds sdsnew(const char* init);
TITUS_EXPORT sds sdsempty(void);
TITUS_EXPORT sds sdsdup(const sds s);
TITUS_EXPORT void sdsfree(sds s);
TITUS_EXPORT sds sdsgrowzero(sds s, size_t len);
TITUS_EXPORT sds sdscatlen(sds s, const void* t, size_t len);
TITUS_EXPORT sds sdscat(sds s, const char* t);
TITUS_EXPORT sds sdscatsds(sds s, const sds t);
TITUS_EXPORT sds sdscpylen(sds s, const char* t, size_t len);
TITUS_EXPORT sds sdscpy(sds s, const char* t);

TITUS_EXPORT sds sdscatvprintf(sds s, const char* fmt, va_list ap);
#ifdef __GNUC__
TITUS_EXPORT sds sdscatprintf(sds s, const char* fmt, ...) __attribute__((format(printf, 2, 3)));
#else
TITUS_EXPORT sds sdscatprintf(sds s, const char* fmt, ...);
#endif

TITUS_EXPORT sds sdscatfmt(sds s, char const* fmt, ...);
TITUS_EXPORT sds sdstrim(sds s, const char* cset);
TITUS_EXPORT void sdsrange(sds s, ptrdiff_t start, ptrdiff_t end);
TITUS_EXPORT void sdsupdatelen(sds s);
TITUS_EXPORT void sdsclear(sds s);
TITUS_EXPORT int sdscmp(const sds s1, const sds s2);
TITUS_EXPORT sds* sdssplitlen(const char* s, ptrdiff_t len, const char* sep, int seplen, int* count);
TITUS_EXPORT void sdsfreesplitres(sds* tokens, int count);
TITUS_EXPORT void sdstolower(sds s);
TITUS_EXPORT void sdstoupper(sds s);
TITUS_EXPORT sds sdsfromlonglong(long long value);
TITUS_EXPORT sds sdscatrepr(sds s, const char* p, size_t len);
TITUS_EXPORT sds* sdssplitargs(const char* line, int* argc);
TITUS_EXPORT sds sdsmapchars(sds s, const char* from, const char* to, size_t setlen);
TITUS_EXPORT sds sdsjoin(char** argv, int argc, char* sep);
TITUS_EXPORT sds sdsjoinsds(sds* argv, int argc, const char* sep, size_t seplen);

/* Low level functions exposed to the user API */
TITUS_EXPORT sds sdsMakeRoomFor(sds s, size_t addlen);
TITUS_EXPORT void sdsIncrLen(sds s, ptrdiff_t incr);
TITUS_EXPORT sds sdsRemoveFreeSpace(sds s);
TITUS_EXPORT size_t sdsAllocSize(sds s);
TITUS_EXPORT void* sdsAllocPtr(sds s);

/* Export the allocator used by SDS to the program using SDS.
 * Sometimes the program SDS is linked to, may use a different set of
 * allocators, but may want to allocate or free things that SDS will
 * respectively free or allocate. */
void* sds_malloc(size_t size);
void* sds_realloc(void* ptr, size_t size);
void sds_free(void* ptr);

#ifdef REDIS_TEST
int sdsTest(int argc, char* argv[]);
#endif

#endif

#pragma clang diagnostic pop
