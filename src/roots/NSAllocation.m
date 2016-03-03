//
//  NSAllocation.c
//  MulleObjC
//
//  Created by Nat! on 07.02.16.
//  Copyright © 2016 Mulle kybernetiK. All rights reserved.
//

#include "NSAllocation.h"

int   MulleObjCObjectZeroProperty( struct _mulle_objc_property *property, struct _mulle_objc_class *cls, void *self);

int   MulleObjCObjectZeroProperty( struct _mulle_objc_property *property, struct _mulle_objc_class *cls, void *self)
{
   char   *signature;
   
   signature = _mulle_objc_property_get_signature( property);
   switch( *signature)
   {
   case _C_COPY_ID   :
   case _C_RETAIN_ID :
      if( property->setter)
         mulle_objc_object_call_no_fastmethod( self, property->setter, nil);
   }
   return( 0);
}


id   NSAllocateObject( Class meta, NSUInteger extra, NSZone *zone)
{
   if( meta)
      return( _MulleObjCAllocateObject( meta, 0, NULL));
   abort(); // TODO: raise exception
   return( nil);
}


void   NSFinalizeObject( id self)
{
   if( self)
      _MulleObjCFinalizeObject( self);
}


void   NSDeallocateObject( id self)
{
   if( self)
      _MulleObjCDeallocateObject( self);
}
