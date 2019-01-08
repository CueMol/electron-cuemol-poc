#import <AppKit/AppKit.h>
#import <OpenGL/gl.h>
#import <OpenGL/glext.h>
#import <OpenGL/glu.h>

#include <stdio.h>
#include "my_extension.hpp"

NSView *g_parView;
NSView *g_view;

void printWndInfo(unsigned char *buffer, size_t size)
{
  NSView *pView = *(NSView **)buffer;
  printf("ParentView.subviews count: %d\n", pView.subviews.count);
}

void createNatWinImpl(unsigned char *buffer, size_t size)
{
  NSView *pView = *(NSView **)buffer;

  int width = 100, height = 100;
  NSRect rect = NSMakeRect(0,0,width,height);
  NSButton *view = [NSButton alloc];
  [view initWithFrame: rect];
  [pView addSubview: view];
  //[view setParentView: pView];

  g_parView = pView;
  g_view = view;
}

void setWindowPos(int x, int y, int width, int height)
{
  NSView *parView = g_parView;
  NSView *view = g_view;

  int ph = [parView frame].size.height;
  printf("XPCNativeWidgetCocoa> parent Height= %d\n", ph);
  int y2 =  ph - (y+height);
  printf("XPCNativeWidgetCocoa> flipped y: %d\n", y2);

  NSRect rect = NSMakeRect(x,y2,width,height);
  [view setFrame: rect];
}
