#pragma once

#ifdef WINDOWS
#define WIN_LNX(win, linux) win
#elif defined(LINUX) || defined(ANDROID)
#ifndef LINUX
#define LINUX
#endif
#define WIN_LNX(win, linux) linux
#endif
