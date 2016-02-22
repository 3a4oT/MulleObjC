/*
 *  MulleFoundation - A tiny Foundation replacement
 *
 *  NSAllocation+ObjectiveC.h is a part of MulleFoundation
 *
 *  Copyright (C) 2011 Nat!, Mulle kybernetiK.
 *  All rights reserved.
 *
 *  Coded by Nat!
 *
 *  $Id$
 *
 */
#ifndef NSALLOCATION_OBJECTIVE_C_H___
#define NSALLOCATION_OBJECTIVE_C_H___

#import "ns_type.h"
#import "ns_allocation.h"
#import "ns_zone.h"


__attribute__((returns_nonnull))
static inline id    _NSAllocateObject( Class cls, NSUInteger extra, NSZone *zone)
{
   struct _mulle_objc_runtime        *runtime;
   struct _mulle_objc_class          *infra;
   struct _mulle_objc_objectheader   *header;
   struct _mulle_objc_objectheader   *object;
   // self is the meta class
   
   assert( ! _mulle_objc_class_is_metaclass( cls));
   
   runtime = _mulle_objc_class_get_runtime( cls);
   
   header  = runtime->allocator.calloc( 1, cls->instance_and_header_size + extra);
   if( ! header)
      _mulle_objc_runtime_raise_fail_errno_exception( runtime);

   _mulle_objc_objectheader_set_isa( header, cls);
   return( (id) _mulle_objc_objectheader_get_object( header));
}


__attribute__((returns_nonnull))
static inline id    _NSAllocateNonZeroedObject( Class isa, NSUInteger extra, NSZone *zone)
{
   // future for now its still just zeroed
   return( (id) _NSAllocateObject( isa, extra, zone));
}


static inline void  _NSFinalizeObject( id obj)
{
   extern int   _NSObjectZeroProperty( struct _mulle_objc_property *, struct _mulle_objc_class *, void *);
   struct _mulle_objc_class   *cls;
   
   // walk through properties and release them
   cls = _mulle_objc_object_get_isa( obj);
   _mulle_objc_class_walk_properties( cls, _mulle_objc_class_get_inheritance( cls), _NSObjectZeroProperty, obj);
}

// this does not zero properties
static inline void   _NSDeallocateObject( id obj)
{
   struct _mulle_objc_runtime        *runtime;
   struct _mulle_objc_class          *cls;
   struct _mulle_objc_objectheader   *header;
   struct _mulle_objc_objectheader   *object;
   
   cls     = _mulle_objc_object_get_isa( obj);
   runtime = _mulle_objc_class_get_runtime( cls);
   header  = _mulle_objc_object_get_objectheader( obj);
   runtime->allocator.free( header);
}

id     NSAllocateObject( Class meta, NSUInteger extra, NSZone *zone);
void   NSFinalizeObject( id ob);
void   NSDeallocateObject( id obj);  


// implement the convenience stuff
// we don't go through a configuration, because we later have atomicity
// which is "harmless" as its faster than a function call
//
static inline void   NSIncrementExtraRefCount( id obj)
{
   _mulle_objc_object_increment_retaincount( obj);
}


static inline BOOL  NSDecrementExtraRefCountWasZero( id obj)
{
   return( (BOOL) (obj ? _mulle_objc_object_decrement_retaincount_was_zero( obj) : 0));
}


static inline NSUInteger   NSExtraRefCount( id obj)
{
   return( (NSUInteger) mulle_objc_object_get_retaincount( obj));
}


static inline BOOL    NSShouldRetainWithZone( id p, NSZone *zone)
{
   return( YES);
}

#endif

