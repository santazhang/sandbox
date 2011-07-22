#import <Cocoa/Cocoa.h>
#import "MouseAccelKillerAppDelegate.h"

int main(int argc, char *argv[])
{
//    return NSApplicationMain(argc,  (const char **) argv);
	MouseAccelKillerAppDelegate * delegate = [[MouseAccelKillerAppDelegate alloc] init];

	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

	NSApplication * application = [NSApplication sharedApplication];
	[application setDelegate:delegate];
	[NSApp run];

	[pool drain];

	[delegate release];

	return 0;
}
