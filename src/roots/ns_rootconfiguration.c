//
//  ns_rootconfiguration.c
//  MulleObjC
//
//  Created by Nat! on 15/10/15.
//  Copyright © 2015 Mulle kybernetiK. All rights reserved.
//

// this is the only file that has an __attribute__ constructor
// this links in all the roots stuff into the runtime

#include "ns_objc_include.h"

#include "ns_rootconfiguration.h"
#include "_ns_exception.h"


# pragma mark -
# pragma mark Exceptions

static void  perror_abort( char *s)
{
   perror( s);
   abort();
}


static void  init_ns_exceptionhandlertable ( struct _ns_exceptionhandlertable *table)
{
   unsigned int   i;
   
   for( i = 0; i <= MulleObjCExceptionHandlerTableSize; i++)
      table->handlers[ i] = (i == MulleObjCExceptionErrnoHandlerIndex) ? (void *) perror_abort : (void *) abort;
}


void   _ns_rootconfiguration_add_root( struct _ns_rootconfiguration *config, void *obj)
{
   assert( mulle_set_get( config->roots, obj) == NULL);
   
   mulle_objc_object_retain( obj);
   mulle_set_put( config->roots, obj);
}


void   _ns_rootconfiguration_release_roots( struct _ns_rootconfiguration *config)
{
   struct mulle_setenumerator     rover;
   void                           *obj;
   
   /* remove all root objects: need to have an enclosing
    * autoreleasepool here
    */
   rover = mulle_set_enumerate( config->roots);
   while( obj = mulle_setenumerator_next( &rover))
      mulle_objc_object_release( obj);
   mulle_setenumerator_done( &rover);
}

# pragma mark -
# pragma mark AutoreleasePool

extern void   MulleObjCAutoreleasePoolConfigurationUnsetThread( void);

static void   runtime_dies( struct _mulle_objc_runtime *runtime, void *data)
{
   struct _ns_rootconfiguration   *config;
   
   _mulle_objc_runtime_get_foundationspace( runtime, (void **) &config, NULL);
   
   mulle_set_free( config->roots);
   if( data != config)
      free( data);
}


static void  *describe_object( struct mulle_container_keycallback *callback, void *p)
{
   // we have no strings yet, someone should patch mulle_allocator_objc
   return( NULL);
}


static struct mulle_container_keycallback   default_root_object_callback =
{
   (void *) mulle_container_callback_pointer_hash,
   (void *) mulle_container_callback_pointer_is_equal,
   (void *) mulle_container_callback_self,
   (void *) mulle_container_callback_nop,
   describe_object,
   NULL,
   NULL,
   NULL
};


static void   nop( struct _mulle_objc_runtime *runtime, void *friend,  struct mulle_objc_loadversion *info)
{
}


/*
 * This function sets up a Foundation on a per thread
 * basis.
 */
extern struct _mulle_objc_method    NSObject_msgForward_method;
extern void                         MulleObjCDeterminePageSize( void);

struct _ns_rootconfiguration  *__ns_root_setup( struct _mulle_objc_runtime *runtime,
                                                struct _ns_root_setupconfig *config)
{
   size_t                               size;
   struct _mulle_objc_foundation        us;
   struct _ns_rootconfiguration         *roots;
   struct mulle_container_keycallback   root_object_callback;

   MulleObjCDeterminePageSize();

   __mulle_objc_runtime_setup( runtime, config->allocator_p);
   
   runtime->classdefaults.inheritance    = MULLE_OBJC_CLASS_DONT_INHERIT_PROTOCOL_CATEGORIES;
   runtime->classdefaults.forwardmethod  = config->forward;
   runtime->exceptions.uncaughtexception = (void *) config->uncaughtexception;
   
   _mulle_objc_runtime_get_foundationspace( runtime, (void **) &roots, &size);
   if( size < sizeof( struct _ns_rootconfiguration))
   {
      roots = calloc( 1, sizeof( struct _ns_rootconfiguration));
      if( ! roots)
         mulle_objc_raise_fail_errno_exception();
   }

   us.runtimefriend.destructor    = runtime_dies;
   us.runtimefriend.data          = roots;
   us.runtimefriend.versionassert = config->versionassert ? config->versionassert : nop;
   
   roots->runtime = runtime;
   
   /* the callback is copied anyway, but the allocator needs to be stored
      in the config. It's OK to have a different allocator for Foundation
      then for the runtime. The roots->allocator is used to create instances.
    */
   
   roots->allocator               = *config->allocator_p;
   root_object_callback           = default_root_object_callback;
   root_object_callback.allocator = &roots->allocator;
   
   roots->roots = mulle_set_create( 32, &root_object_callback);
   _mulle_objc_runtime_set_foundation( runtime, &us);
   
   return( roots);
}


void   _ns_root_setup( struct _mulle_objc_runtime *runtime,
                       struct _ns_root_setupconfig *config)
{
   struct _ns_rootconfiguration   *roots;
   
   roots = __ns_root_setup( runtime, config);
   
   init_ns_exceptionhandlertable ( &roots->exceptions);

   //
   // this initializes the autorelease pool for this thread
   // it registers this thread with the runtime
   // .. we can not do this yet, no classes are loaded
   //
   // this will be done in [NSThread load]
   //
   // thread = [NSThread new];
   // [thread instantiateRuntimeThread];
}


//
// your chance to take over, if you somehow make it to the front of the
// __load chain. (You can with dylibs, but is very hard with .a)
//
void   (*ns_root_setup)( struct _mulle_objc_runtime *runtime,
                         struct _ns_root_setupconfig *config) = _ns_root_setup;

mulle_thread_tss_t   __ns_rootconfigurationKey;

//
// we assume worst case, that some other class is loading in faster than
// this is the "patch" in function, that is not defined by the objc
// runtime
//

