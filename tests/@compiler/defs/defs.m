#include <mulle_objc/mulle_objc.h>

@interface Bar
{
   int  x;
}

@end


@implementation Bar

- (void) takeBarDefs:(struct { @defs( Bar); }*) defs
{
   self->x = defs->x;
}

@end


@interface Foo : Bar
{
   int y;
}

@property int   z;

@end


@implementation Foo

+ (id) new
{
   return( mulle_objc_class_alloc_instance( self, NULL));
}


- (void) takeFooDefs:(struct { @defs( Foo); } *) defs
{
   [super takeBarDefs:(struct { @defs( Bar); } *) defs];

   self->y = defs->y;
   [self setZ:defs->_z];
}

@end


main()
{
   Foo  *foo;

   foo = [Foo new];
   [foo takeFooDefs:&( struct { @defs( Foo); }){ .x = 1, .y = 2, ._z = 3 }];
}
