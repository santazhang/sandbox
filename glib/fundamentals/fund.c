#include <stdio.h>

#include <glib.h>

int main()
{
  printf("This is GLib fundamentals!\n");
  printf("GLib version %d.%d.%d\n", GLIB_MAJOR_VERSION, GLIB_MINOR_VERSION,
    GLIB_MICRO_VERSION);
  printf("GLib check match '%s'\n", glib_check_version(2, 20, 1));
  return 0;
}
