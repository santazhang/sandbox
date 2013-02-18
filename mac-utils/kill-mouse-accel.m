#import <Foundation/Foundation.h>
#include <IOKit/hidsystem/event_status_driver.h>
#include <IOKit/hidsystem/IOHIDLib.h>
#include <IOKit/hidsystem/IOHIDParameter.h>

static int KillAccel(CFStringRef type)
{
    const int32_t accel = -0x10000;
    int success = 0;
    io_connect_t handle = NXOpenEventStatus();
    if (handle) {
        success = IOHIDSetParameter(handle, type, &accel, sizeof accel) == KERN_SUCCESS;
        NXCloseEventStatus(handle);
    }
    return success;
}

int main(int argc, const char * argv[])
{
    if (KillAccel(CFSTR(kIOHIDMouseAccelerationType))) {
        NSLog(@"killed mouse accel");
    }
    if (KillAccel(CFSTR(kIOHIDTrackpadAccelerationType))) {
        NSLog(@"killed trackpad accel");
    }
    return 0;
}
