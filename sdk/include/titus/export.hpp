
#ifndef TITUS_EXPORT_H
#define TITUS_EXPORT_H

#ifdef TITUS_STATIC_DEFINE
#  define TITUS_EXPORT
#  define TITUS_NO_EXPORT
#else
#  ifndef TITUS_EXPORT
#    ifdef titus_sdk_EXPORTS
        /* We are building this library */
#      define TITUS_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define TITUS_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef TITUS_NO_EXPORT
#    define TITUS_NO_EXPORT 
#  endif
#endif

#ifndef TITUS_DEPRECATED
#  define TITUS_DEPRECATED __declspec(deprecated)
#endif

#ifndef TITUS_DEPRECATED_EXPORT
#  define TITUS_DEPRECATED_EXPORT TITUS_EXPORT TITUS_DEPRECATED
#endif

#ifndef TITUS_DEPRECATED_NO_EXPORT
#  define TITUS_DEPRECATED_NO_EXPORT TITUS_NO_EXPORT TITUS_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef TITUS_NO_DEPRECATED
#    define TITUS_NO_DEPRECATED
#  endif
#endif

#endif /* TITUS_EXPORT_H */
