#include <X11/Xlib.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


//compiler avec le flag -lX11
//g++ mouse.cpp -o mprog -lX11



void click (Display *display, int button)
{
  // Create and setting up the event
  XEvent event;
  memset (&event, 0, sizeof (event));
  event.xbutton.button = button;
  event.xbutton.same_screen = True;
  event.xbutton.subwindow = DefaultRootWindow (display);
  while (event.xbutton.subwindow)
    {
      std::cout << "hello" << std::endl;
      event.xbutton.window = event.xbutton.subwindow;
      XQueryPointer (display, event.xbutton.window,
		     &event.xbutton.root, &event.xbutton.subwindow,
		     &event.xbutton.x_root, &event.xbutton.y_root,
		     &event.xbutton.x, &event.xbutton.y,
		     &event.xbutton.state);
    }
  // Press
  event.type = ButtonPress;
  if (XSendEvent (display, PointerWindow, True, ButtonPressMask, &event) == 0)
    fprintf (stderr, "Error to send the event!\n");
  XFlush (display);
  usleep (1);
  // Release
  event.type = ButtonRelease;
  if (XSendEvent (display, PointerWindow, True, ButtonReleaseMask, &event) == 0)
    fprintf (stderr, "Error to send the event!\n");
  XFlush (display);
  usleep (1);
}

int main(void) {

    //choisir l'ecran
    Display* dpy = XOpenDisplay(0);
    int scr = XDefaultScreen(dpy);
    Window root_window = XRootWindow(dpy, scr);

    //calculer la taille et de l'ecran
    int height = DisplayHeight(dpy, scr);
    int width  = DisplayWidth(dpy, scr);
    std::cout << "Screen size : " << width << "x" << height << std::endl;

    //initialiser la position du curseur au milieu de l'écran
    int c,i,j;
    i=width/2; j=height/2;
    int posx=i;
    c = 0;

    
    while(true){
        //demander à l'utilisateur le mouvement du curseur sur l'axe de X
        std::cout << "Ajout X ";
        std::cin >> c;
        i+=c; 
	//mettre à jour le curseur
        XWarpPointer(dpy, None, root_window, 0, 0, 0, 0, i, j);
        if(c==0){
          //simuler le click si utilisateur entre 0
	  click (dpy, Button1);
	}
        XFlush(dpy);
        usleep(50);
    }

   return 0;
}
