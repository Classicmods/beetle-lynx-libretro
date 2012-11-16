#include "mednafen/mednafen-types.h"
#include "mednafen/mednafen.h"
#include "mednafen/md5.h"
#include "mednafen/git.h"
#include "mednafen/general.h"
#include "mednafen/mednafen-driver.h"

#if defined(__CELLOS_LV2__)
#include <sys/timer.h>
#include <ppu_intrinsics.h>
#endif

#ifndef _WIN32
#include <unistd.h>
#endif

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

//#define LIBRETRO_DEBUG

// Stubs

void MDFND_Sleep(unsigned int time)
{
#if defined(__CELLOS_LV2__)
   sys_timer_usleep(time * 1000);
#elif defined(_WIN32)
   Sleep(time);
#else
   usleep(time * 1000);
#endif
}

extern std::string retro_base_directory;
extern std::string retro_base_name;

#ifdef _WIN32
static void sanitize_path(std::string &path)
{
   size_t size = path.size();
   for (size_t i = 0; i < size; i++)
      if (path[i] == '/')
         path[i] = '\\';
}
#endif

// Use a simpler approach to make sure that things go right for libretro.
std::string MDFN_MakeFName(MakeFName_Type type, int id1, const char *cd1)
{
   std::string ret;
   switch (type)
   {
      case MDFNMKF_SAV:
         ret = retro_base_directory + std::string(PSS) + retro_base_name +
            std::string(".") + md5_context::asciistr(MDFNGameInfo->MD5, 0) + std::string(".") +
            std::string(cd1);
         break;
      case MDFNMKF_FIRMWARE:
         ret = std::string(cd1);
         break;
      default:
         break;
   }

#ifdef _WIN32
   sanitize_path(ret); // Because Windows path handling is mongoloid.
#endif
   return ret;
}

void MDFND_DispMessage(unsigned char *str)
{
#ifdef LIBRETRO_DEBUG
   if(str != NULL)
      fprintf(stderr, "DISPMSG: %s\n", str);
#endif
}

void MDFND_Message(const char *str)
{
#ifdef LIBRETRO_DEBUG
   if(str != NULL)
      fprintf(stderr, "MSG: %s\n", str);
#endif
}

void MDFND_MidSync(const EmulateSpecStruct *)
{}

void MDFND_PrintError(const char* err)
{
#ifdef LIBRETRO_DEBUG
   if(err != NULL)
      fprintf(stderr, "ERR: %s\n", err);
#endif
}

uint32 MDFND_GetTime()
{
   static bool first = true;
   static uint32_t start_ms;

#if defined(__CELLOS_LV2__)
   uint64_t time = __mftb();
   uint32_t ms = (time - start_ms) / 1000;

   if (first)
   {
      start_ms = ms;
      first = false;
   }
#elif defined(_WIN32)
   DWORD ms = timeGetTime();
   if (first)
   {
      start_ms = ms;
      first = false;
   }
#else
   struct timeval val;
   gettimeofday(&val, NULL);
   uint32_t ms = val.tv_sec * 1000 + val.tv_usec / 1000;

   if (first)
   {
      start_ms = ms;
      first = false;
   }
#endif

   return ms - start_ms;
}

