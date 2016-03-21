//
//  ns_debug.h
//  MulleObjC
//
//  Created by Nat! on 21/10/15.
//  Copyright © 2015 Mulle kybernetiK. All rights reserved.
//

#ifndef ns_debug__h__
#define ns_debug__h__

#include "ns_type.h"

extern BOOL   NSDebugEnabled;
extern BOOL   NSZombieEnabled;
extern BOOL   NSDeallocateZombies;
extern BOOL   NSHangOnUncaughtException;


extern void         *NSFrameAddress( NSUInteger frame);
extern void         *NSReturnAddress( NSUInteger frame);
extern NSUInteger   NSCountFrames( void);

#endif /* ns_debug_h */
