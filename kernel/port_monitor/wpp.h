#pragma once

#define WPP_CONTROL_GUIDS                                          \
    WPP_DEFINE_CONTROL_GUID(                                       \
        myDriverTraceGuid, (A2CA4540,4094,4388,8A06,D7A2B132FADF), \
        WPP_DEFINE_BIT(MAIN)                                       \
        )

#define WPP_LEVEL_FLAGS_LOGGER(lvl,flags) \
           WPP_LEVEL_LOGGER(flags)

#define WPP_LEVEL_FLAGS_ENABLED(lvl, flags) \
           (WPP_LEVEL_ENABLED(flags) && WPP_CONTROL(WPP_BIT_ ## flags).Level >= lvl)

// begin_wpp config
// FUNC info{LEVEL=TRACE_LEVEL_INFORMATION}(FLAGS,MSG, ...);
// end_wpp
