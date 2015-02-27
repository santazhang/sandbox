//
//  check-addrbook.m
//
//  Created by Santa Zhang on 11/11/12.
//  Copyright (c) 2012 Santa Zhang. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AddressBook/AddressBook.h>

int main(int argc, const char * argv[])
{
    @autoreleasepool {

        ABAddressBook *addressBook = [ABAddressBook sharedAddressBook];

        NSError *error = NULL;
        NSRegularExpression *phoneRegex = [NSRegularExpression regularExpressionWithPattern:
            @"(^\\+86 [0-9]{3}-[0-9]{4}-[0-9]{4}$)|"
             "(^\\+86 \\([0-9]\\) [0-9]{2,3}-[0-9]{7,8}$)|"
             "(^\\+1 \\([0-9]{3}\\) [0-9]{3}-[0-9]{4}$)|"
             "(^\\+1 \\([0-9]{3}\\) [A-Z]{7}$)|"
             "(^12520 [0-9]{3}-[0-9]{4}-[0-9]{4}$)|"
             "(^\\+92 [0-9]{3} [0-9]{7}$)|"
             "(^\\+852 [0-9]{4} [0-9]{4}$)|(12520[0-9]{9,11})"
            options:NSRegularExpressionCaseInsensitive error:&error];

        for (ABPerson* p in [addressBook people]) {
            NSLog(@"Name: %@ %@", [p valueForProperty:kABFirstNameProperty], [p valueForProperty:kABLastNameProperty]);
            if ([p valueForProperty:kABPhoneProperty] != nil) {
                for (int i = 0; i < [[p valueForProperty:kABPhoneProperty] count]; i++) {
                    NSString* phone = [[p valueForProperty:kABPhoneProperty] valueAtIndex:i];
                    if ([phoneRegex firstMatchInString:phone options:0 range:NSMakeRange(0, [phone length])]) {
                        //NSLog(@"Phone: %@", phone);
                    } else {
                        NSLog(@"*** (Bad Format) Phone: %@", phone);
                    }
                }
            }

            if ([p valueForProperty:kABInstantMessageProperty] != nil) {
                for (int i = 0; i < [[p valueForProperty:kABInstantMessageProperty] count]; i++) {
                    NSLog(@"IM: %@", [[p valueForProperty:kABInstantMessageProperty] valueAtIndex:i]);
                }
            }
        }
    }
    return 0;
}
