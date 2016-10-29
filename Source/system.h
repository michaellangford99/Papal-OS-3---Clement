/*
This is the main system header
*/

#ifndef SYSTEM_H
#define SYSTEM_H

/* Check if the compiler thinks we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This needs to be compiled with a ix86-elf compiler"
#endif

#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif

#if !defined(Clement)
#define Clement
#endif

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "math.h"
#include "string.h"
#include "error.h"
#include "debug.h"
#include "ascii.h"
#include "cpu/cpu.h"
#include "tools/tools.h"
#include "multiboot.h"
#include "vga.h"
#include "mem/memory.h"
#include "mem/paging.h"
#include "console.h"
#include "keyboard.h"
#include "clock.h"
#include "ata.h"
#include "fs/file_system.h"
#endif
