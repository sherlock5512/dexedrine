#define _DEFAULT_SOURCE
#include <bsd/libutil.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <systemd/sd-bus.h>
#include <unistd.h>
#include <linux/limits.h>

void handleSigs(int sig);

#define _cleanup_(f) __attribute__((cleanup(f)))
