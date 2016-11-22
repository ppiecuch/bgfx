#import <Cocoa/Cocoa.h>
#import <assert.h>

void *get_nswindow_from_nsview(void *nsview)
{
    assert(nsview);
    assert([(NSView*)nsview isKindOfClass:[NSView class]]);

    return (void*)[(NSView*)nsview window];
}
