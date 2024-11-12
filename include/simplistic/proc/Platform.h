#pragma once

#ifdef WINDOWS
#define WIN_LNX(win, linux) win
#elif defined(LINUX) || defined(ANDROID)
#define WIN_LNX(win, linux) linux
#endif
