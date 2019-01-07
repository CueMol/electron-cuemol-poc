
#include <nan.h>
#include <stdio.h>
#include <windows.h>
#include <GL/gl.h>
#pragma comment(lib, "opengl32.lib")

using namespace Nan;
using namespace v8;

static HWND g_hWnd;
static HDC g_hDC;
static HGLRC g_hGLRC;

void printWndInfo(HWND hWnd)
{
  char sbuf[256];
  int nlen;
  
  nlen = GetWindowText(hWnd, sbuf, sizeof(sbuf));
  sbuf[nlen] = 0;
  printf("title; <%s>\n", sbuf);
  nlen = GetClassName(hWnd, sbuf, sizeof(sbuf));
  sbuf[nlen] = 0;
  printf("class; <%s>\n", sbuf);

  RECT rc;
  GetWindowRect(hWnd, &rc);
  printf("WndRect (%d,%d)-(%d,%d)\n", rc.left, rc.top, rc.right, rc.bottom);
  fflush(stdout);
}

void resetExStyles(HWND hWnd)
{
  LONG lStyle;
  /*
  lStyle  = GetWindowLong( hWnd, GWL_STYLE );
  lStyle &= ~WS_VISIBLE;
  lStyle  = SetWindowLong( hWnd, GWL_STYLE, lStyle );
  */

  lStyle  = GetWindowLong( hWnd, GWL_EXSTYLE );
  lStyle &= ~(WS_EX_TRANSPARENT|WS_EX_LAYERED);
  lStyle &= ~(0x00200000L);
  printf("style: %X\n", lStyle);
  fflush(stdout);
  lStyle  = SetWindowLong( hWnd, GWL_EXSTYLE, lStyle );
}

void setClipChildren(HWND hWnd)
{
  LONG lStyle;
  lStyle  = GetWindowLong( hWnd, GWL_STYLE );
  //  lStyle |= (WS_CLIPSIBLINGS|WS_CLIPCHILDREN);
  // lStyle |= (WS_CLIPSIBLINGS);
  lStyle |= (WS_CLIPCHILDREN);
  lStyle  = SetWindowLong( hWnd, GWL_STYLE, lStyle );
}

void setClipSiblings(HWND hWnd)
{
  LONG lStyle;
  lStyle  = GetWindowLong( hWnd, GWL_STYLE );
  lStyle |= (WS_CLIPSIBLINGS);
  lStyle  = SetWindowLong( hWnd, GWL_STYLE, lStyle );
}

BOOL CALLBACK EnumChildProc(HWND hWnd , LPARAM lp)
{
  printf("chinld wnd: %X\n", hWnd);
  printWndInfo(hWnd);

  // resetExStyles(hWnd);

  return TRUE;
}

// entry stub
static
LRESULT CALLBACK sHandleWin32Event(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  /*
  // get our plugin instance object and ask it for the version string
#ifdef _WIN64
  LONG_PTR ldata = GetWindowLongPtr(hWnd, GWLP_USERDATA);
#else
  LONG ldata = GetWindowLong(hWnd, GWL_USERDATA);
#endif
  XPCNativeWidgetWin *ppn = reinterpret_cast<XPCNativeWidgetWin *>(ldata);
  
  if (ppn)
    return ppn->handleEvent(hWnd, msg, wParam, lParam);
  */

  switch (msg){
  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc=BeginPaint(hWnd,&ps);
    //TextOut(hdc, 0, 0, "Hello, Windows!", 15); 
    //Rectangle(hdc, 0, 0, 10, 10);
    RECT rc;
    GetClientRect(hWnd, &rc);
    int w = rc.right-rc.left;
    int h = rc.bottom-rc.top;
    int dist = 200;

    wglMakeCurrent(g_hDC, g_hGLRC);

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho(0,w, h,0, dist+1.0, -dist-1.0); //-slabnear+1.0, -slabnear-1.0);
    glOrtho(-(GLdouble)w/200.0, (GLdouble)w/200.0,
    	    -(GLdouble)h/200.0, (GLdouble)h/200.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0.0, 0.0, 0, 1);
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPointSize(10.0);
    glLineWidth(1.0);

    glDisable(GL_CULL_FACE);
    glBegin(GL_TRIANGLES);
    //glBegin(GL_POINTS);
    //glBegin(GL_LINE_LOOP);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(-0.50f, -0.50f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2f(-0.50f, 0.50f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2f(0.50f, -0.50f);
    glEnd();

    SwapBuffers(g_hDC);
    wglMakeCurrent(NULL, NULL);

    EndPaint(hWnd,&ps);
    printf("WM_PAINT (%d,%d)\n",w,h);
    fflush(stdout);
    break;
  }
    
  case WM_ERASEBKGND:
    return 0; //DefWindowProc(hWnd, msg, wParam, lParam);

  case WM_LBUTTONDOWN: 
  case WM_MBUTTONDOWN: 
  case WM_RBUTTONDOWN: {
    printf("WM_BUTTONDOWN\n");
    fflush(stdout);
    break;
  }

  case WM_MOUSEWHEEL: {
    printf("WM_MOUSEWHEEL\n");
    fflush(stdout);
    break;
  }
    
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

HWND createNativeChildWnd(HWND hParWnd)
{
  WNDCLASSEX wc;
  wc.cbSize = sizeof wc;
  // wc.style         = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
  wc.style         = CS_HREDRAW|CS_VREDRAW;
  wc.lpfnWndProc   = sHandleWin32Event;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = NULL;
  wc.hIcon         = NULL; //::LoadIcon(::GetModuleHandle(NULL), IDI_APPLICATION);
  wc.hCursor       = NULL;
  //wc.hbrBackground = (HBRUSH)::GetStockObject(NULL_BRUSH);
  wc.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = "CueMol2NativeWidget";
  wc.hIconSm = NULL;

  if (::RegisterClassEx(&wc)==0) {
    DWORD nerr = GetLastError();
    if (nerr!=ERROR_CLASS_ALREADY_EXISTS) {
      printf("XPCNativeWidgetWin> FATAL ERROR, Cannot register window class!!\n");
      return NULL;
    }
  }

  printf("XPCNativeWidgetWin> Register window class: OK\n");

  //int width = getWidth(), height=getHeight();
  int width=100, height=100;
  if (width<0) width = 0;
  if (height<0) height = 0;

  HWND wnd = CreateWindow(wc.lpszClassName,
                          "native view",
			  //WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
			  WS_CHILD|WS_VISIBLE|WS_DISABLED,
                          0, 0, width, height,
                          hParWnd,
                          NULL,
                          NULL,
                          NULL);

  SetWindowPos(wnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW);
  SetWindowPos(wnd, HWND_TOP, 100, 100, 100, 100, 0);
  ShowWindow( wnd, SW_SHOW );
  UpdateWindow( wnd );

  return wnd;
}

NAN_METHOD(hello)
{
  printf("===============\n");
  printf(" hello called\n");

  unsigned char* buffer = (unsigned char*) node::Buffer::Data(info[0]->ToObject());
  unsigned int size = info[1]->Uint32Value();

  /*
  printf("buffer size: %d\n", size);
  for (int i=0; i<size; ++i)
    printf("data: %x\n", buffer[i]);
  */
  
  // get display scaling factors
  HDC screen = GetDC(0);
  double sclX = GetDeviceCaps(screen, LOGPIXELSX) / 96.0;
  double sclY = GetDeviceCaps(screen, LOGPIXELSY) / 96.0;
  printf("Display sclfac= (%f,%f)\n", sclX, sclY);

  HWND hWnd = *(HWND *)buffer;
  printf("wnd: %X\n", hWnd);
  printWndInfo(hWnd);
  //setClipStyles(hWnd);

  EnumChildWindows(hWnd, EnumChildProc, 0);

  //info.GetReturnValue().Set(Nan::New("hello, world").ToLocalChecked());
  info.GetReturnValue().Set(Nan::New("hello, world").ToLocalChecked());
}

NAN_METHOD(createNatWin)
{
  printf("===============\n");
  printf(" createNatWin called\n");

  unsigned char* buffer = (unsigned char*) node::Buffer::Data(info[0]->ToObject());
  unsigned int size = info[1]->Uint32Value();

  HWND hWnd = *(HWND *)buffer;
  printf("wnd: %X\n", hWnd);

  EnumChildWindows(hWnd, EnumChildProc, 0);

  HWND hChWnd1 = GetWindow(hWnd, GW_CHILD);
  printf("Ch wnd 1: %X\n", hChWnd1);
  printWndInfo(hChWnd1);
  //setClipStyles(hChWnd1);
  //setInvisible(hChWnd1);

  /*
  hChWnd1 = GetWindow(hChWnd1, GW_HWNDLAST);
  printf("Ch wnd 2: %X\n", hChWnd1);
  printWndInfo(hChWnd1);
  //setInvisible(hChWnd1);
  */

  // HWND hChWnd2 = createNativeChildWnd(hChWnd1);
  HWND hChWnd2 = createNativeChildWnd(hWnd);
  printf("child wnd: %X\n", hChWnd2);

  g_hWnd = hChWnd2;

  HDC hDC = GetDC( g_hWnd );

  PIXELFORMATDESCRIPTOR pfd;
  int ipx;
  memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
  pfd.nSize = (sizeof(PIXELFORMATDESCRIPTOR));
  pfd.nVersion = 1;

  pfd.dwFlags =
    PFD_DRAW_TO_WINDOW | // support window
      PFD_SUPPORT_OPENGL |          // support OpenGL
        PFD_DOUBLEBUFFER;           // double buffered
  pfd.iPixelType = PFD_TYPE_RGBA; // RGBA type
  pfd.cColorBits = 24; // 24-bit color depth
  pfd.cDepthBits = 32;
  pfd.iLayerType = PFD_MAIN_PLANE; // main layer
  
  pfd.cRedBits = 8;
  pfd.cGreenBits = 8;
  pfd.cBlueBits = 8;

  int pixelformat;
  pfd.cColorBits = 32;

  pixelformat = ChoosePixelFormat(hDC, &pfd);
  printf("ChoosePixelFormat: %d\n", pixelformat);
  DescribePixelFormat(hDC, pixelformat,
		      sizeof(PIXELFORMATDESCRIPTOR), &pfd);
    
  SetPixelFormat(hDC, pixelformat, &pfd);
  //setPixFmt(ipx);

  HGLRC hGL = wglCreateContext( hDC );
  wglMakeCurrent(hDC, hGL);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  glClearDepth(1.0f);

  glEnable(GL_NORMALIZE);
  glShadeModel(GL_SMOOTH);

  glEnable(GL_LINE_SMOOTH);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  g_hDC = hDC;
  g_hGLRC = hGL;
  printf("HDC=%d\n", g_hDC);
  printf("HGLRC=%d\n", g_hGLRC);

  fflush(stdout);
}

NAN_METHOD(setWinPos)
{
  printf("===============\n");
  printf(" setWinPos called\n");
  
  // expect exactly 3 arguments
  if(info.Length() != 4) {
    return Nan::ThrowError(Nan::New("Vector::New - expected arguments x, y, w, h").ToLocalChecked());
  }

  // expect arguments to be numbers
  if(!info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber() || !info[3]->IsNumber()) {
    return Nan::ThrowError(Nan::New("Vector::New - expected arguments to be numbers").ToLocalChecked());
  }

  int x = (int) info[0]->NumberValue();
  int y = (int) info[1]->NumberValue();
  int w = (int) info[2]->NumberValue();
  int h = (int) info[3]->NumberValue();

  SetWindowPos(g_hWnd, HWND_TOP, x, y, w, h, 0);

  printf("%d,%d,%d,%d\n", x,y,w,h);
  fflush(stdout);
}

NAN_MODULE_INIT(init)
{
  NAN_EXPORT(target, hello);
  NAN_EXPORT(target, createNatWin);
  NAN_EXPORT(target, setWinPos);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, init);


/*
#include <node.h>

void HelloFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    //args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "hello, world"));
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "hello, world", v8::NewStringType::kNormal).ToLocalChecked());
}

void init(v8::Local<v8::Object> exports)
{
    NODE_SET_METHOD(exports, "hello", HelloFunction);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, init);

*/
