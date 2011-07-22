#import "MouseAccelKiller.h"

#include <IOKit/hidsystem/IOHIDLib.h>
#include <IOKit/hidsystem/IOHIDParameter.h>

static int KillAccel(CFStringRef type)
{
	const int32_t accel = -0x10000;
	int success = 0;
	io_connect_t handle = NXOpenEventStatus();

	if(handle) {
		success = IOHIDSetParameter(handle, type, &accel, sizeof accel) == KERN_SUCCESS;
		NXCloseEventStatus(handle);
	}
	return success;
}

@implementation MouseAccelKiller

- (IBAction) killAllSilently:(id) sender
{
	[self killMouseAccel:self];
	[self killTrackpadAccel:self];
}

- (IBAction)killMouseAccel:(id)sender
{
	if(!KillAccel(CFSTR(kIOHIDMouseAccelerationType))) {
		NSRunAlertPanel(@"Failed", @"", @"OK", nil, nil);
	} else {
//		NSRunAlertPanel(@"", @"hello", @"OK", nil, nil);
	}
}

- (IBAction)killTrackpadAccel:(id)sender
{
	if(!KillAccel(CFSTR(kIOHIDTrackpadAccelerationType))) {
		NSRunAlertPanel(@"Failed", @"", @"OK", nil, nil);
	} else {
//		NSRunAlertPanel(@"Success", @"hello", @"OK", nil, nil);
	}

}

@end
