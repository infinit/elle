#ifndef _JSON_SPIRIT_PLATFORM_H_
#define _JSON_SPIRIT_PLATFORM_H_

#define JSON_SPIRIT_PLATFORM_WINDOWS 0
#define JSON_SPIRIT_PLATFORM_LINUX   1
#define JSON_SPIRIT_PLATFORM_MAC     2

#if defined(__WIN32__) || defined(_WIN32)
#  define JSON_SPIRIT_PLATFORM JSON_SPIRIT_PLATFORM_WINDOWS
#elif defined(__APPLE_CC__) || defined(__APPLE__)
#  define JSON_SPIRIT_PLATFORM JSON_SPIRIT_PLATFORM_MAC
#  ifndef __MACOSX__
#    define __MACOSX__
#  endif
#else
#  define JSON_SPIRIT_PLATFORM JSON_SPIRIT_PLATFORM_LINUX
#endif

#ifdef JSON_SPIRIT_DEBUG_BUILD
#define JSON_SPIRIT_DEBUG 1
#else
#define JSON_SPIRIT_DEBUG 0
#endif

#ifndef JSON_SPIRIT_EXPORT
# if JSON_SPIRIT_PLATFORM == JSON_SPIRIT_PLATFORM_WINDOWS
#   if defined(JSON_SPIRIT_STATIC_LINKED)
#     define JSON_SPIRIT_EXPORT
#   else
#     if defined(JSON_SPIRIT_LIBRARY_BUILD)
#       define JSON_SPIRIT_EXPORT __declspec(dllexport)
#       define JSON_SPIRIT_EXPORT_TEMPLATE
#     else
#       define JSON_SPIRIT_EXPORT __declspec(dllimport)
#       define JSON_SPIRIT_EXPORT_TEMPLATE extern
#     endif
#   endif
#   define JSON_SPIRIT_PLUGIN_EXPORT __declspec(dllexport)
# else
#   if defined(__GNUC__) && __GNUC__ >= 4
#     define JSON_SPIRIT_EXPORT __attribute__ ((visibility("default")))
#     define JSON_SPIRIT_PLUGIN_EXPORT __attribute__ ((visibility("default")))
#   else
#     define JSON_SPIRIT_EXPORT
#     define JSON_SPIRIT_PLUGIN_EXPORT
#   endif
#   define JSON_SPIRIT_EXPORT_TEMPLATE
# endif
#endif


#ifndef JSON_SPIRIT_FUNCTION_EXPORT
# if JSON_SPIRIT_PLATFORM == JSON_SPIRIT_PLATFORM_WINDOWS
#   if defined(JSON_SPIRIT_STATIC_LINKED)
#     define JSON_SPIRIT_FUNCTION_EXPORT
#   else
#     if defined(JSON_SPIRIT_LIBRARY_BUILD)
#       define JSON_SPIRIT_FUNCTION_EXPORT __declspec(dllexport)
#     else
#       define JSON_SPIRIT_FUNCTION_EXPORT __declspec(dllimport)
#     endif
#   endif
# else
#   if defined(__GNUC__) && __GNUC__ >= 4
#     define JSON_SPIRIT_FUNCTION_EXPORT __attribute__ ((visibility("default")))
#   else
#     define JSON_SPIRIT_FUNCTION_EXPORT
#   endif
# endif
#endif



#ifndef JSON_SPIRIT_EXPORT_C
# define JSON_SPIRIT_EXPORT_C extern "C" JSON_SPIRIT_EXPORT
#endif

#ifndef JSON_SPIRIT_PLUGIN_EXPORT_C
# define JSON_SPIRIT_PLUGIN_EXPORT_C extern "C" JSON_SPIRIT_PLUGIN_EXPORT
#endif

#endif //_JSON_SPIRIT_PLATFORM_H_
