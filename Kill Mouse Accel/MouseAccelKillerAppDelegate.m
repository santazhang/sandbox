//
//  MouseAccelKillerAppDelegate.m
//  Kill Mouse Accel
//
//  Created by kreeble on 3/26/11.
//  Copyright 2011 kreeble. All rights reserved.
//

#import "MouseAccelKillerAppDelegate.h"
#import "MouseAccelKiller.h"

@implementation MouseAccelKillerAppDelegate

- (void) applicationDidFinishLaunching:(NSNotification *) aNotification {
	NSLog(@"Running scripts..");
	MouseAccelKiller *a = [[MouseAccelKiller alloc] init];
    [a killMouseAccel:self];
	//[a killAllSilently:self];

	// Debug
	// NSRunAlertPanel(@"Done", @"", @"OK", nil, nil);
	
	NSLog(@"Done.");
	
	[NSApp terminate:nil];
}

@end
