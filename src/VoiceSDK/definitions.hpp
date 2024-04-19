#ifndef __H_VOICE_SDK_DEFINITIONS_HPP
#define __H_VOICE_SDK_DEFINITIONS_HPP __TIMESTAMP__

#if defined(__cpp_if_constexpr) //  __cplusplus >= 201703L 
#define IFCONSTEXPR if constexpr
#else
#define IFCONSTEXPR if
#endif

#ifndef VoiceSDK_EXPORTS
# if defined _WIN32 || defined WINCE || defined __CYGWIN__
#   define VoiceSDK_EXPORTS __declspec(dllexport)
# elif defined __GNUC__ && __GNUC__ >= 4
#   define VoiceSDK_EXPORTS __attribute__ ((visibility ("default")))
# endif
#endif

#endif