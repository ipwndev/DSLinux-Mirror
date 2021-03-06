//
// "$Id$"
//
// Color chooser header file for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-1999 by Bill Spitzak and others.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "fltk-bugs@easysw.com".
//

// The color chooser object and the color chooser popup.  The popup
// is just a window containing a single color chooser and some boxes
// to indicate the current and cancelled color.

#ifndef Fl_Color_Chooser_H
#define Fl_Color_Chooser_H

#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Value_Input.H>

class Flcc_HueBox : public Fl_Widget {
  FL_EXPORT int handle(int);
  FL_EXPORT void draw();
  int px, py;
public:
  Flcc_HueBox(int X, int Y, int W, int H) : Fl_Widget(X,Y,W,H) {
  px = py = 0;}
};

class Flcc_ValueBox : public Fl_Widget {
  FL_EXPORT int handle(int);
  FL_EXPORT void draw();
  int py;
public:
  Flcc_ValueBox(int X, int Y, int W, int H) : Fl_Widget(X,Y,W,H) {
  py = 0;}
};

class Flcc_Value_Input : public Fl_Value_Input {
public:
  FL_EXPORT int format(char*);
  Flcc_Value_Input(int X, int Y, int W, int H) : Fl_Value_Input(X,Y,W,H) {}
};

class Fl_Color_Chooser : public Fl_Group {
  Flcc_HueBox huebox;
  Flcc_ValueBox valuebox;
  Fl_Choice choice;
  Flcc_Value_Input rvalue;
  Flcc_Value_Input gvalue;
  Flcc_Value_Input bvalue;
  Fl_Box resize_box;
  float hue_, saturation_, value_;
  float r_, g_, b_;
  FL_EXPORT void set_valuators();
  static FL_EXPORT void rgb_cb(Fl_Widget*, void*);
  static FL_EXPORT void mode_cb(Fl_Widget*, void*);
public:
  int mode() {return choice.value();}
  float hue() const {return hue_;}
  float saturation() const {return saturation_;}
  float value() const {return value_;}
  float r() const {return r_;}
  float g() const {return g_;}
  float b() const {return b_;}
  FL_EXPORT int hsv(float,float,float);
  FL_EXPORT int rgb(float,float,float);
  static FL_EXPORT void hsv2rgb(float, float, float,float&,float&,float&);
  static FL_EXPORT void rgb2hsv(float, float, float,float&,float&,float&);
  FL_EXPORT Fl_Color_Chooser(int,int,int,int,const char* = 0);
};

FL_EXPORT int fl_color_chooser(const char* name, float& r, float& g, float& b);
FL_EXPORT int fl_color_chooser(const char* name, uchar& r, uchar& g, uchar& b);

#endif

//
// End of "$Id$".
//
