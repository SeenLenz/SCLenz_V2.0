#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
 
#define SHIFT 0b0001
#define CAPS 0b0010
#define CTRL 0b0100
#define ALT 0b1000


int main()
{

// int thing = 2;
// int *pthing = &thing;

//   printf("%d", *pthing);
    Display *display;
    Window window;
    XEvent event;
    int s;

    unsigned modifiers = 0b0000;
    //LSB to MSB
    //shift -> caps lock -> ctrl -> alt 


    /* open connection with the server */
    display = XOpenDisplay(NULL);
    if (display == NULL)
    {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }
 
    s = DefaultScreen(display);
 
    /* create window */
    window = XCreateSimpleWindow(display, RootWindow(display, s), 10, 10, 200, 200, 1,
                           BlackPixel(display, s), WhitePixel(display, s));
 
    /* select kind of events we are interested in */
    XSelectInput(display, window, KeyPressMask | KeyReleaseMask );
 
    /* map (show) the window */
    XMapWindow(display, window);
 
    /* event loop */
    while (1)
    {
        XNextEvent(display, &event);
 
        /* keyboard events */
        if (event.type == KeyPress)
        {

if ( modifiers & SHIFT || modifiers & CAPS){
  printf( "KeyPress: %c\n",  XLookupKeysym(&event.xkey, 2));
}

            printf( "KeyPress: %c\n",  XLookupKeysym(&event.xkey, 1));

           
            /* exit on ESC key press */
            if ( event.xkey.keycode == 0x09 )
                break;
        }
        else if (event.type == KeyRelease)
        {
            printf( "KeyRelease: %c\n", XLookupKeysym(&event.xkey, 1) );
  if ( modifiers & SHIFT || modifiers & CAPS){
printf( "KeyRelease: %c\n", XLookupKeysym(&event.xkey, 2) );
}
        }
    }

    /* close connection to server */
    XCloseDisplay(display);
 
    return 0;
}