#ifndef __EMSCRIPTEN__

#include <stdio.h>
#include <string.h>

#if defined (_WIN32) || defined (_WIN64)
#define TRAY_WINAPI 1
#elif defined (__linux__) || defined (linux) || defined (__linux)
#define TRAY_APPINDICATOR 1
#elif defined (__APPLE__) || defined (__MACH__)
#define TRAY_APPKIT 1
#endif

#include "tray.h"
#include "windowBehaviour.h"

#if TRAY_APPINDICATOR
#define TRAY_ICON1 "mail-message-new"
#define TRAY_ICON2 "mail-message-new"
#elif TRAY_APPKIT
#define TRAY_ICON1 "icon.png"
#define TRAY_ICON2 "icon.png"
#elif TRAY_WINAPI
#define TRAY_ICON1 "icon.ico"
#define TRAY_ICON2 "icon.ico"
#endif

// C header here

static void toggle_cb(struct tray_menu *item);
static void show_cb(struct tray_menu *item);
static void quit_cb(struct tray_menu *item);
static void submenu_cb(struct tray_menu *item);

// Test tray init
struct tray tray = {
    .icon = TRAY_ICON1,
#if TRAY_WINAPI
    .tooltip = (char *)"Tray",
    .tray_icon_left_button_handle = show_window,
#endif
    .menu =
        (struct tray_menu[]) {
            {.text = "Show", .cb = show_cb},
            {.text = "Checked", .checked = 1, .checkbox = 1, .cb = toggle_cb},
            {.text = "Disabled", .disabled = 1},
            {.text = "-"},
            {.text = "SubMenu",
             .submenu =
                 (struct tray_menu[]) {
                     {.text = "FIRST", .checked = 1, .checkbox = 1, .cb = submenu_cb},
                     {.text = "SECOND",
                      .submenu =
                          (struct tray_menu[]) {
                              {.text = "THIRD",
                               .submenu =
                                   (struct tray_menu[]) {
                                       {.text = "7", .cb = submenu_cb},
                                       {.text = "-"},
                                       {.text = "8", .cb = submenu_cb},
                                       {.text = (const char *)NULL}}},
                              {.text = "FOUR",
                               .submenu =
                                   (struct tray_menu[]) {
                                       {.text = "5", .cb = submenu_cb},
                                       {.text = "6", .cb = submenu_cb},
                                       {.text = (const char *)NULL}}},
                              {.text = (const char *)NULL}}},
                     {.text = (const char *)NULL}}},
            {.text = "-"},
            {.text = "Quit", .cb = quit_cb},
            {.text = (const char *)NULL}},
};

static void toggle_cb(struct tray_menu *item) {
  printf("toggle cb\n");
  item->checked = !item->checked;
  tray_update(&tray);
}

static void show_cb(struct tray_menu *item) {
  (void)item;
  printf("show cb\n");
  show_window();
  if (strcmp(tray.icon, TRAY_ICON1) == 0) {
    tray.icon = TRAY_ICON2;
  } else {
    tray.icon = TRAY_ICON1;
  }
  tray_update(&tray);
}

static void quit_cb(struct tray_menu *item) {
  (void)item;
  printf("quit cb\n");
  tray_exit();
}

static void submenu_cb(struct tray_menu *item) {
  (void)item;
  printf("submenu: clicked on %s\n", item->text);
  tray_update(&tray);
}

#endif
