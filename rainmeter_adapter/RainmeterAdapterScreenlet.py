#!/usr/bin/env python

# This application is released under the GNU General Public License 
# v3 (or, at your option, any later version). You can find the full 
# text of the license under http://www.gnu.org/licenses/gpl.txt. 
# By using, editing and/or distributing this software you agree to 
# the terms and conditions of this license. 
# Thank you for using free software!

# Rainmeter Adapter by Santa Zhang <santa.zh@gmail.com>
#
# INFO:
# - Ports Rainmeter skins to Screenlets
#
# CODING:
# - Helper function/variables starts with "_"

import screenlets
from screenlets import Screenlet
from screenlets.options import IntOption, TimeOption
from screenlets import sensors

import pygtk
pygtk.require('2.0')

import gtk
import math
import cairo
import pango
import datetime
import gobject
import os
import time
from ConfigParser import ConfigParser
import _ast # for CalcMeasure

from RandomWallpaper import *

from screenlets.options import StringOption, IntOption


class Measure:
  
  _name = None

  _if_above = None # "IfAboveValue"
  _if_above_action = None

  _if_below = None
  _if_below_action = None

  _if_equal = None
  _if_equal_action = None

  _stored_value = None

  _disabled = False

  def __init__ (self):
    pass
  
  def get_value (self):
    """Gets the value of this measure. Could be any type"""
    return None

  # triggers like "IfValueAbove"
  def test_trigger (self, new_value):
    if self._disabled:
      return

    if self._if_above_action != None and self._if_above != None and self._stored_value != None:
      if (self._stored_value > self._if_above) == False and (new_value > self._if_above) == True:
        self._skin._bang.execute(self._if_above_action)

# TODO test the following 2 triggers
    if self._if_below_action != None and self._if_below != None and self._stored_value != None:
      if (self._stroed_value < self._if_below) == False and (new_value < self._if_below) == True:
        self._skin._bang.execute(self._if_below_action)

    if self._if_equal_action != None and self._if_equal != None and self._stored_value != None:
      if self._stored_value != self._if_equal and new_value == self._if_equal:
        self._skin._bang.execute(self._if_equal_action)

# this is a basic measure
class TimeMeasure (Measure):
  """Class that gives time measure"""
  
  def __init__ (self):
    Measure.__init__(self)
    self._name = "Time"

  def get_value (self):
    #tm = time.localtime()
    #return tm.tm_hour * 3600 + tm.tm_min * 60 + tm.tm_sec # returns a double-precision time
    result = time.time() - time.timezone
    self.test_trigger(result)
    self._stored_value = result
    return result


class UptimeMeasure (Measure):

  def __init__ (self):
    Measure.__init__(self)
    self._name = "Uptime"
  
  def get_value (self):
    #TODO get sys uptime
    result = sensors.sys_get_uptime_long()
#    self.test_trigger(result) this measure don't need to trigger any event
    self._stored_value = result
    return result


class PhysicalMemoryMeasure (Measure):
  
  def __init__ (self):
    Measure.__init__(self)
    self._name = "PhysicalMemory"

  def get_value (self):
    result = sensors.mem_get_usedmem()
    self._stored_value = result
    return result

class SwapMemoryMeasure (Measure):
  
  def __init__ (self):
    Measure.__init__(self)
    self._name = "SwapMemory"

  def get_value (self):
    result = sensors.mem_get_usedswap()
    self._stored_value = result
    return result

class CounterMeasure (Measure):
  """This measure always gives the refresh counter's value of the skin"""

  _skin = None
  
  def __init__ (self, skin):
    Measure.__init__(self)
    self._name = "Counter"
    self._skin = skin

  def get_value (self):
    result = self._skin._update_counter
    self.test_trigger(result)
    self._update_counter = result
    return result


class CustomMeasure (Measure):
  """Measures added in script"""


  def __init__ (self, skin, ini_section):
    Measure.__init__(self)

    if ini_section.has_key("ifabovevalue"):
      self._if_above = int(ini_section["ifabovevalue"])
      if ini_section.has_key("ifaboveaction"):
        self._if_above_action = ini_section["ifaboveaction"]

    # TODO below, equal action

    if ini_section.has_key("disabled"):
      if ini_section["disabled"] == "1":
        self._disabled = True


class CalcMeasure (CustomMeasure):

  _skin = None
  _formula = "None"
  # _stored_value is used for buffer
  _ast = None

  
  def __init__ (self, skin, ini_section):
    CustomMeasure.__init__(self, skin, ini_section)
    self._name = ini_section["__name__"]
    self._skin = skin
    self._formula = ini_section["formula"]
    self._ast = compile(self._formula, "Formula of " + self._name, "eval", _ast.PyCF_ONLY_AST)

  def get_last_calc_value (self):
    return self._stored_value

  def calc_value (self, ast_node):
    if type(ast_node) == _ast.Expression:
      return self.calc_value(ast_node.body)

    elif type(ast_node) == _ast.BinOp:
      if type(ast_node.op) == _ast.Add:
        return self.calc_value(ast_node.left) + self.calc_value(ast_node.right)
      elif type(ast_node.op) == _ast.Sub:
        return self.calc_value(ast_node.left) - self.calc_value(ast_node.right)
      elif type(ast_node.op) == _ast.Mult:
        return self.calc_value(ast_node.left) * self.calc_value(ast_node.right)
      elif type(ast_node.op) == _ast.Div:
        return self.calc_value(ast_node.left) / self.calc_value(ast_node.right)
      elif type(ast_node.op) == _ast.Mod:
        return self.calc_value(ast_node.left) % self.calc_value(ast_node.right)
      #TODO other operators
      else:
        print "*** warning: failed to process bin op", str(ast_node.op)
    
    elif type(ast_node) == _ast.Num:
      return ast_node.n

    elif type(ast_node) == _ast.Name:
      measure = self._skin._measure[ast_node.id]
      if type(measure) == CalcMeasure:
        # for Calc meters, don't re-calc data, but use stored ones
        return measure.get_last_calc_value()
      else:
        return measure.get_value()

    elif type(ast_node) == _ast.Call:
      func_name = ast_node.func.id
      arg_list = map(lambda x : self.calc_value(x), ast_node.args)
      
      if func_name == "MAX" :
        return max(arg_list)
      elif func_name == "MIN":
        return min(arg_list)
      elif func_name == "SIN":
        return math.sin(arg_list[0])
      # TODO other functions
      else:
        print "*** warning: no such function:", func_name
        return None

    else:
      print "*** warning: failed to process ast:\n" + str(ast_node)
      return None

  def get_value (self):
    if self._disabled:
      return 0

    result = self.calc_value(self._ast)
    self.test_trigger(result)
    self._stored_value = result
    return result


class NamedMeasure (CustomMeasure):
  """Custom defined measure"""
  
  _measure = None
  
  _update_divider = 1
  _skin = None

  _format = None  # result will be a string
  _substitute = None # tuple (a, b), where a will be replaced to b. only for string
  
  def __init__ (self, skin, ini_section):
    CustomMeasure.__init__(self, skin, ini_section)
    self._skin = skin
    self._name = ini_section["__name__"]

    if ini_section.has_key("format"):
      self._format = ini_section["format"]
      if self._format.startswith('"') and self._format.endswith('"'):
        self._format = self._format.strip('"')

    if ini_section.has_key("substitute"):
      self._substitute = ini_section["substitute"].split(":")
      self._substitute = map(lambda x : x.strip('"'), self._substitute)
    
    if ini_section.has_key("measure"):
      if skin._measure.has_key(ini_section["measure"]):
        self._measure = skin._measure[ini_section["measure"]]
      else:
        print "*** warning: Measure %s not found!" % ini_section["measure"]
    else: # measure not understood
      print "*** warning: Measure %s not understood!" % self._name
    
    if ini_section.has_key("updatedivider"):
      self._update_divider = int(ini_section["updatedivider"])


  def get_value(self):
    return_value = self._stored_value
    if self._skin._update_counter % self._update_divider == 0 or return_value == None:
      return_value = self._measure.get_value()

      # formatting on return value
      if self._format != None:
        formatted_value = self._format

        if type(return_value) == float: # float value might be a time value
          # NOTE: For time measure, we have already handled timezone in TimeMeasure, so here we use
          # gmtime instead of localtime
          time_value = time.gmtime(return_value)

        if self._format.find("%#d") != -1:
          # for time type, value is long-time, %#d means "date in a month"
          formatted_value = formatted_value.replace("%#d", str(time_value.tm_mday))

        if self._format.find("%d") != -1:
          formatted_value = formatted_value.replace("%d", str(time_value.tm_mday))
        
        if self._format.find("%m") != -1:
          formatted_value = formatted_value.replace("%m", ["Jan", "Feb", "Mar", "Apr", "May", "June", "July", "Aug", "Sep", "Oct", "Nov", "Dec"][time_value.tm_mon])

        if self._format.find("%a") != -1:
          formatted_value = formatted_value.replace("%a", ["Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"][time_value.tm_wday])
          # date in a week
        
        if self._format.find("%A") != -1:
          formatted_value = formatted_value.replace("%A", ["Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"][time_value.tm_wday])

        if self._format.find("%M") != -1: # minutes
          formatted_value = formatted_value.replace("%M", "%02d" % time_value.tm_min)

        if self._format.find("%I") != -1: # hour
          formatted_value = formatted_value.replace("%I", "%02d" % time_value.tm_hour)

        if self._format.find("%S") != -1: # seconds
          formatted_value = formatted_value.replace("%S", "%02d" % time_value.tm_sec)

        if self._format.find("%p") != -1: # am, pm
          if time_value.tm_hour > 12:
            formatted_value = formatted_value.replace("%p", "PM")
          else:
            formatted_value = formatted_value.replace("%p", "AM")
      
        return_value = formatted_value
      
      if self._substitute != None:
        return_value = return_value.replace(self._substitute[0], self._substitute[1])

      self.test_trigger(return_value)
      self._stored_value = return_value
    return return_value
    
    

def measure_factory (skin, ini_section):
  if ini_section.has_key("measure") and ini_section["measure"] == "Calc":
    skin.add_measure(CalcMeasure(skin, ini_section))
  else:
    skin.add_measure(NamedMeasure(skin, ini_section))
  
  

class Meter:

  _skin = None
  _name = None
  
  _x = 0
  _y = 0
  _w = -1 # means "unknown"
  _h = -1 # means "unknown"
  _hidden = False

  _left_mouse_down_action = None

  _measure = None
  
  def __init__ (self, skin, ini_section, ref_x, ref_y):
    self._skin = skin
    self._name = ini_section["__name__"]
    
    if ini_section.has_key("hidden"):
      if ini_section["hidden"] == "1":
        self._hidden = True

    if ini_section.has_key("measurename"):
      self._measure = skin._measure[ini_section["measurename"]]
#    else: # no measure found, warning
#      print "*** warning: no measure found for meter", self._name

    if ini_section.has_key("leftmousedownaction"):
      self._left_mouse_down_action = ini_section["leftmousedownaction"]

    if ini_section.has_key("x"):
      x_str = ini_section["x"]
      #print "x_str", x_str
      
      if x_str[-1] == "r":
        # NOTE: handle relative location
        self._x = float(x_str[:-1]) + ref_x
        #print "Got ref", ref_x
      else:
        # not relative location
        self._x = float(x_str)
    
    if ini_section.has_key("y"):
      y_str = ini_section["y"]
      #print "y_str", y_str
      
      if y_str[-1] == "r":
        self._y = float(y_str[:-1]) + ref_y
        #print "Got ref", ref_y
      else:
        self._y = float(y_str)
        
    if ini_section.has_key("w"):
      self._w = int(ini_section["w"])
    
    if ini_section.has_key("h"):
      self._h = int(ini_section["h"])

  def click (self, x, y):
    if self._hidden: # hidden meters are not to be tested
      return False

    if self._x <= x and self._y <= y and x <= self._x + self._w and y <= self._y + self._h:
      if self._left_mouse_down_action != None:
        self._skin._bang.execute(self._left_mouse_down_action)
      return True
    else:
      return False

  def paint (self, ctx, screenlet):
    """Draws the meter with a cairo context"""
    pass # to be overriden

  def get_bounding (self):
    """Return the runtime bounding. (x, y, w, h)"""
    return self._x, self._y, self._w, self._h

  def get_x (self):
    return self._x
    
  def get_y (self):
    return self._y

  def get_w (self):
    return self._w
    
  def get_h (self):
    return self._h




class ImageMeter (Meter):

  _png_name = None
  _auto_image_size = False # whether the size of image was automatically detected (W, H not given in .ini file)

  def __init__ (self, skin, ini_section, ref_x, ref_y):
    Meter.__init__ (self, skin, ini_section, ref_x, ref_y)
    #print ini_section

    
    if ini_section.has_key("imagename"):
      img_name = ini_section["imagename"]
      #print img_name
      self._png_name = img_name[:img_name.rfind('.')]
      
      # the width/height might already be determined explicitly
      if self._w < 0:
        self._w = skin._theme.pngs[self._png_name].get_width()
        self._auto_image_size = True
      if self._h < 0:
        self._h = skin._theme.pngs[self._png_name].get_height()
        self._auto_image_size = True
        
    #print "x, y, w, h = ", self._x, self._y, self._w, self._h

  def paint (self, ctx, screenlet):
    #print "Image %s on paint!" % self._name
    if self._png_name != None:
      #print "Draw at", self._x, self._y
      ctx.translate(self._x, self._y)
      screenlet.theme.render(ctx, self._png_name)
      #print "render image %s" % self._png_name
    else:
      # dummy image, transparent, but could handle mouse events
      pass
    



class RotatorMeter (ImageMeter):

  _offset_x = 0
  _offset_y = 0
  _start_angle = 0
  _rotation_angle = math.pi * 2
  _value_reminder = 60
  _clockwise = True

  def __init__ (self, skin, ini_section, ref_x, ref_y):
    ImageMeter.__init__(self, skin, ini_section, ref_x, ref_y)
    
    if ini_section.has_key("startangle"):
      self._start_angle = float(ini_section["startangle"])

    if ini_section.has_key("valuereminder"):
      self._value_reminder = float(ini_section["valuereminder"])

    if ini_section.has_key("rotationangle"):
      self._rotation_angle = float(ini_section["rotationangle"])

    if ini_section.has_key("offsetx"):
      self._offset_x = float(ini_section["offsetx"])

    if ini_section.has_key("offsety"):
      self._offset_y = float(ini_section["offsety"])
      
    if ini_section.has_key("clockwise"):
      if ini_section["clockwise"] != "1":
        self._clockwise = False

    #print "Rotator:", self._start_angle, self._value_reminder, self._rotation_angle, self._offset_x, self._offset_y
  
  def get_bounding (self):
  
    # XXX This is really puzzling
    if self._auto_image_size == True:
      center_x = self._x
      center_y = self._y
      
    else:
      center_x = self._x + self._w / 2
      center_y = self._y + self._h / 2
      
    radius = math.sqrt(self._w * self._w + self._h * self._h) / 2
    return center_x - radius, center_y - radius, 2 * radius, 2 * radius

  def paint (self, ctx, screenlet):
    
    if self._measure != None:
      # rotator always uses float calculation
      rotate_value = self._measure.get_value()
    else: # measure not found!
      print "*** Error! Measure not found for meter", self._name
      
    rotate_value %= 1.0 * self._value_reminder
    #print "Rot", rotate_value
    rotate_angle = (1.0 * rotate_value) / self._value_reminder * self._rotation_angle + self._start_angle
    rotate_angle %= math.pi * 2
    #print "RotAngle", rotate_angle
    #print self._png_name, "x, y, w, h, ox, oy,", self._x, self._y, self._w, self._h, self._offset_x, self._offset_y
    
    
    # XXX This is really puzzling
    if self._auto_image_size == True: # for those auto-detected image, center is (self._x, self._y)
      ctx.translate(self._x, self._y)
      if self._clockwise:
        ctx.rotate(rotate_angle)
      else: # counter clockwise
        ctx.rotate(-rotate_angle)
      ctx.translate(-self._offset_x, -self._offset_y)
      
    else:
      ctx.translate(self._x + self._w / 2, self._y + self._h / 2)
      if self._clockwise:
        ctx.rotate(rotate_angle)
      else: # counter clockwise
        ctx.rotate(-rotate_angle)
      ctx.translate(-self._offset_x, -self._offset_y)
    
    screenlet.theme.render(ctx, self._png_name)
    #print ctx.get_matrix()


class RoundLineMeter (Meter):

  _offset_x = 0
  _offset_y = 0
  _start_angle = 0
  _rotation_angle = math.pi * 2
  _value_reminder = 60
  _clockwise = True
  _line_length = 10
  _line_width = 2
  _line_color = (0.6, 0.6, 0.6, 1.0)

  def __init__ (self, skin, ini_section, ref_x, ref_y):
    Meter.__init__(self, skin, ini_section, ref_x, ref_y)

    # print "RoundLine, ref_x=%d, ref_y=%d" % (ref_x, ref_y)
    
    if ini_section.has_key("startangle"):
      self._start_angle = float(ini_section["startangle"])
      #print "SET START ANGLE", self._start_angle

    if ini_section.has_key("valuereminder"):
      self._value_reminder = float(ini_section["valuereminder"])

    if ini_section.has_key("rotationangle"):
      self._rotation_angle = float(ini_section["rotationangle"])

    if ini_section.has_key("offsetx"):
      self._offset_x = float(ini_section["offsetx"])
      # print "has_key: offsetx", self._offset_x
    else:
      self._offset_x = self._w / 2
      # print "offset_x = w/2", self._offset_x

    if ini_section.has_key("offsety"):
      self._offset_y = float(ini_section["offsety"])
      #print "has_key: offsety", self._offset_y
    else:
      self._offset_y = self._h / 2
      #print "offset_y = h/2", self._offset_y
      
    if ini_section.has_key("clockwise"):
      if ini_section["clockwise"] != "1":
        self._clockwise = False
        
    if ini_section.has_key("linewidth"):
      self._line_width = float(ini_section["linewidth"])
    
    if ini_section.has_key("linelength"):
      self._line_length = float(ini_section["linelength"])
    
    if ini_section.has_key("linecolor"):
      color_tuple = ini_section["linecolor"].strip("'\"").split(", ")
      self._line_color = map(lambda x : float(x) / 255.0, color_tuple)
    
  
  def paint (self, ctx, screenlet):
  
    if self._measure != None:
      # rotator always uses float calculation
      rotate_value = self._measure.get_value()
    else: # measure not found!
      print "*** Error! Measure not found for meter", self._name
      
    # make sure the value is a float
    rotate_value = float(rotate_value)

    rotate_value %= self._value_reminder

    rotate_angle = (1.0 * rotate_value / self._value_reminder) * self._rotation_angle + self._start_angle
    rotate_angle %= math.pi * 2
    #print "Tick", (rotate_angle + math.pi / 2) % (2 * math.pi) / 2 / math.pi * 60
    
    ctx.translate(self._offset_x + self._x, self._offset_y + self._y)

    if self._clockwise:
      ctx.rotate(rotate_angle)
    else: # counter clockwise
      ctx.rotate(-rotate_angle)
    #ctx.translate(-self._offset_x, -self._offset_y)
    
    ctx.set_source_rgba(self._line_color[0], self._line_color[1], self._line_color[2], self._line_color[3])
    #ctx.set_operator(cairo.OPERATOR_OVER)
    ctx.set_line_width(self._line_width)
    ctx.move_to(0, 0)
    #ctx.move_to(self._offset_x, self._offset_y)
    ctx.line_to(self._line_length, 0)
    ctx.stroke()



# There's 2 kind of skin meters, constant, or based on measure
#
# for constant string meters, no "measure" or "measure name" is provided
# and a "text" value is providided in settings file
#
# for measured string meters, the string is obtained from measuring
#
class StringMeter (Meter):
  #TODO

  _text = "#TEXT#"
  _font_face = "Sans"
  _font_color = (0.6, 0.6, 0.6, 1)
  _font_size = 30
  _string_style = "BOLD"
  _string_align = "LEFT"
  _prefix = ""

  def __init__ (self, skin, ini_section, ref_x, ref_y):
    Meter.__init__(self, skin, ini_section, ref_x, ref_y)

    if ini_section.has_key("text"):
      self._text = ini_section["text"]
      if self._text.startswith('"') and self._text.endswith('"'):
        self._text = self._text.strip('"')

    if ini_section.has_key("fontface"):
      self._font_face = ini_section["fontface"]

    if ini_section.has_key("fontcolor"):
      self._font_color = map(lambda x : float(x) / 255.0, ini_section["fontcolor"].strip("'\"").split(", "))

    if ini_section.has_key("fontsize"):
      self._font_size = int(ini_section["fontsize"])

    if ini_section.has_key("prefix"):
      self._prefix = ini_section["prefix"].strip('"')

    if ini_section.has_key("stringalign"):
      self._string_align = ini_section["stringalign"]

  
  def _is_const (self):
    return self._measure == None

  def paint(self, ctx, screenlet):
    text_to_paint = ""

    if self._is_const():
      if self._prefix != "":
        text_to_paint = self._prefix
      else:
        text_to_paint = self._text
    else:
      # stupid format hint, but useful
      value = self._measure.get_value()
      if type(value) != str:
        if self._measure._name.lower().find("time") != -1:
          tm = time.gmtime(value)  # time zone have already been handled in TimeMeasure
          value = "%02d:%02d:%02d" % (tm.tm_hour, tm.tm_min, tm.tm_sec)
      text_to_paint = value

    #print type(text_to_paint), text_to_paint
    
    if type(text_to_paint) != str:
      # print "*** warning: string meter: value not text! convert forced"
      text_to_paint = str(text_to_paint)

    text_to_paint = text_to_paint

    #print text_to_paint
    txt_xbearing, txt_ybearing, txt_width, txt_height, txt_xadvance, txt_yadvance = ctx.text_extents(text_to_paint)
    
    x_pos = self._x

    align = pango.ALIGN_LEFT
    if self._string_align == "LEFT":
      align = pango.ALIGN_LEFT
    elif self._string_align == "RIGHT":
      align = pango.ALIGN_RIGHT
      x_pos -= min(screenlet.width, txt_width)
    elif self._string_align == "CENTER":
      align = pango.ALIGN_CENTER
    else:
      print "*** warning: align in String"

    # TODO determine width height
    #self._w = 200
    #self._h = 200
    # TODO string style: BOLD ...
    ctx.set_source_rgba(self._font_color[0], self._font_color[1], self._font_color[2], self._font_color[1])

    # "screenlet.width" is used for layout multiple lines

    screenlet.draw_text(ctx, text_to_paint, int(x_pos), int(self._y), self._font_face, self._font_size, min(screenlet.width, txt_width), align)


def meter_factory (skin, ini_section, ref_x, ref_y):
  """Create a meter and add to skin"""

  meter = None
  
  if ini_section["meter"].lower() == "image":
    meter = ImageMeter(skin, ini_section, ref_x, ref_y)
  elif ini_section["meter"].lower() == "roundline":
    meter = RoundLineMeter(skin, ini_section, ref_x, ref_y)
  elif ini_section["meter"].lower() == "rotator":
    meter = RotatorMeter(skin, ini_section, ref_x, ref_y)
  elif ini_section["meter"].lower() == "string":
    meter = StringMeter(skin, ini_section, ref_x, ref_y)
    
  if meter == None:
    print "*** warning: do not understand meter definition: ", ini_section
  else:
    skin.add_meter(meter)
  
  return meter.get_x(), meter.get_y()



class Bang:
  
  _skin = None
  
  def __init__ (self, skin):
    self._skin = skin
  
  def execute (self, action, *params):
    if action.find(" ") != -1:
      idx = action.find(" ")
      bang_name = action[:idx].strip()
      bang_arg = action[idx:].strip()
    else:
      bang_name = action

    #print "exec", action

    bang_name = bang_name.lower()
    if bang_name == "!execute":
      idx2 = 0
      while True:
        idx1 = action.find("[", idx2)
        if idx1 < 0:
          break;
        idx2 = action.find("]", idx1)
        self.execute(action[(idx1 + 1) : idx2])
    
    elif bang_name == "!rainmetershowmeter":
      if self._skin._meter.has_key(bang_arg):
        self._skin._meter[bang_arg]._hidden = False
        #print "Show", bang_arg

    elif bang_name == "!rainmeterhidemeter":
      if self._skin._meter.has_key(bang_arg):
        self._skin._meter[bang_arg]._hidden = True
        #print "Hide", bang_arg

    elif bang_name == "!rainmetertogglemeter":
      if self._skin._meter.has_key(bang_arg):
        self._skin._meter[bang_arg]._hidden = not self._skin._meter[bang_arg]._hidden
        #print "Toggle", bang_arg

    elif bang_name == "!rainmeterredraw":
      # do nothing here. calling skin.update() will cause stack overflow
      pass

    elif bang_name == "!rainmeterenablemeasure":
      if self._skin._measure.has_key(bang_arg):
        self._skin._measure[bang_arg]._disabled = False
        #print "Enable measure", bang_arg

    elif bang_name == "!rainmeterdisablemeasure":
      if self._skin._measure.has_key(bang_arg):
        self._skin._measure[bang_arg]._disabled = True
        #print "Disable measure", bang_arg

    elif bang_name == "!rainmetertogglemeasure":
      if self._skin._measure.has_key(bang_arg):
        self._skin._measure[bang_arg]._disabled = not self._skin._measure[bang_arg]._disabled
        #print "Toggle measure", bang_arg

    # TODO implement more bangs

    else:
      print "*** warning: Unknown BANG:", bang_name, "\nFull cmd:", action


class Skin:
  
  # BUG FIXED: config parser should be re-created every time the skin changes
  #_config = ConfigParser()
  
  _update_counter = 0
  _update_interval = 1000
  
  _width = 200
  _height = 200
  
  _meter = {}
  _measure = {}
  _bang = None

  _theme = None
  
  _ordered_section_list = []
  
  def _try_get_config(self, section, item):
    try:
      return self._config.get(section, item)
    except:
      pass

      
  def _get_ordered_section_list(self, ini_path):
    list = []
    f = open(ini_path)
    
    try:
      for line in f.readlines():
        line = line.strip(" \t\r\n")
        if line.startswith("[") and line.endswith("]"):
          section_name = line.strip("[]")
          if section_name != "Variables":
            list += line.strip("[]"),
          
    finally:
      f.close()
      
    return list

  def add_bang (self, bang):
    self._bang = bang


  def add_meter (self, meter):
    #print meter._name
    self._meter[meter._name] = meter
    
    
  def add_measure (self, measure):
    self._measure[measure._name] = measure

  
  def __init__ (self, ini_path, theme):

    # BUG FIXED: config parser should always be re-created when the skin changed
    self._config = ConfigParser()
    self._config.read(ini_path)
    
    # handle "Variable" section as in SS_purple.ini    
    if self._config._sections.has_key("Variables"):
      for section_name in self._config._sections.keys():
        for setting_name in self._config._sections[section_name].keys():
          if self._config._sections[section_name][setting_name].startswith("#"):
            variable_name = self._config._sections[section_name][setting_name].strip("#").lower()
            self._config._sections[section_name][setting_name] = self._config._sections["Variables"][variable_name].strip("\"'")

      # remove variable section
      del self._config._sections["Variables"]
    

    self._theme = theme
    
    # this list contains the section names, in order
    self._ordered_section_list = self._get_ordered_section_list(ini_path)
    
    if self._try_get_config("Rainmeter", "Update"):
      self._update_interval = int(self._config.get("Rainmeter", "Update"))
    else:
      self._update_interval = 1000
    
    ref_x, ref_y = 0, 0
    
    # add a single bang
    self.add_bang(Bang(self))

    # add default measures
    self.add_measure(TimeMeasure())
    self.add_measure(UptimeMeasure())
    self.add_measure(CounterMeasure(self))
    self.add_measure(PhysicalMemoryMeasure())
    self.add_measure(SwapMemoryMeasure())
    
    for section in self._ordered_section_list:
      if section == "Rainmeter":
        continue
        
      #print section
      
      if self._try_get_config(section, "Measure"):
        # it is a measure
        measure_factory(self, self._config._sections[section])
        
        
      elif self._try_get_config(section, "Meter"):
        # it is a meter
        #print "Meter:", section
        ref_x, ref_y = meter_factory(self, self._config._sections[section], ref_x, ref_y)
      
      else:
        # section not understood
        print "*** warning: section %s not understood!" % section
  
  
  def update (self):
    """Called on each update of the Screenlet"""
    for section in self._ordered_section_list:
      if self._measure.has_key(section):
        measure = self._measure[section]
        measure.get_value() # we call "get_value" function to inform "update" of the measures
    self._update_counter += 1


  def get_update_interval (self):
    return self._update_interval


  def get_size (self):
    return self._width, self._height

    
  def get_update_counter (self):
    return self._update_counter


  def left_mouse_click (self, x, y):
    # check in reverse order, the picture drawn later is on higher level
    for idx in range(len(self._ordered_section_list) - 1, -1, -1):
      section = self._ordered_section_list[idx]
      if self._meter.has_key(section) and self._meter[section]._hidden == False:
        if self._meter[section].click(x, y):
          return True

    return False

    
  def paint (self, ctx, screenlet):
    # update size
    w_bound = 0
    h_bound = 0
    for section in self._ordered_section_list:
      if self._meter.has_key(section) and self._meter[section]._hidden == False:
        x, y, w, h = self._meter[section].get_bounding()
        w_bound = max(x + w, w_bound)
        h_bound = max(y + h, h_bound)
    
    self._width = w_bound
    self._height = h_bound
    
    # do painting, in order as given in .ini file
    for section in self._ordered_section_list:
      if self._meter.has_key(section):
        if self._meter[section]._hidden == False:
          ctx.save()
          self._meter[section].paint(ctx, screenlet)
          ctx.restore()
#        else: # hidden image
#          print "Image hidden:", section
      
      
  

class RainmeterAdapterScreenlet (Screenlet):
  """The Screenlet as an adapter to Rainmeter."""

  # default meta-info for Screenlets
  __name__ = "RainmeterAdapterScreenlet"
  __version__ = "0.2"
  __author__ = "Santa Zhang (santa1987@gmail.com)"
  __desc__ = __doc__

  config_filename = os.getenv("HOME") + "/.screenlets/RainmeterAdapter/rainmeter_adapter_screenlet.conf"

  # those parameters are intended for 'Random Wallpaper' Effect
  wallpaper_dir = "/home/santa/Pictures/Best Collection"
  wallpaper_interval = 60
  wallpaper_last_change_time = None

  # set default theme for this Screenlet
  # it will be overridden by the stored settings or by on_init function
  theme_name = "TH2_Konomi"

  # internal variable
  _update_interval = 1000
  
#  _new_update_interval = None # used to change the default _update_interval
  _skin = None # the rainmeter skin to be rendered


  def __init__ (self, **keyword_args):
    """Create a new RainmeterAdapterScreenlet instance."""
    
    Screenlet.__init__(self, uses_theme = True, **keyword_args)
    
    #self.theme_name = "TH2_Konomi"
    
    # the default time out, 1sec
    self.timer = gobject.timeout_add(self._update_interval, self.update)

    if os.path.exists(self.config_filename):
      self.read_config_file() # read user's config data
    else:
      self.write_config_file() # write default config data

  def read_config_file(self):
    print "Reading config file :)"
    config_file = open(self.config_filename, "r")
    config_lines = config_file.readlines()
    for config_line in config_lines:
      config_line = config_line.strip()
      if config_line.startswith("wallpaper_dir="):
        self.wallpaper_dir = config_line[14:]
        print "Got config: wallpaper_dir=%s" % self.wallpaper_dir
      elif config_line.startswith("wallpaper_interval="):
        self.wallpaper_interval = int(config_line[19:])
        print "Got config: wallpaper_interval=%d" % self.wallpaper_interval
      elif config_line.startswith("theme_name="):
        self.theme_name = config_line[11:]
        print "Got config: theme_name=%s" % self.theme_name
    config_file.close()

  def write_config_file(self):
    config_file = open(self.config_filename, "w")
    config_file.write("wallpaper_dir=%s\n" % self.wallpaper_dir)
    config_file.write("wallpaper_interval=%d\n" % self.wallpaper_interval)
    config_file.write("theme_name=%s\n" % self.theme_name)
    config_file.close()

  def on_after_set_atribute(self, name, value):
#    print "after set attribute: " + str(name) + " = " + str(value)
    if (name.startswith("wallpaper_")):
      self.write_config_file()
  
  def on_init (self):
    """Called when the Screenlet's options have been applied and the 
    screenlet finished its initialization. If you want to have your
    Screenlet do things on startup you should use this handler."""
    
    # add menu item
    # TODO menus
    self.add_menuitem("about", "About...")
    self.add_menuitem("help", "Help")
    self.add_menuitem("refresh", "Refresh")
    self.add_menuitem("next_wallpaper", "Next Wallpaper")
    # add default menu items
    self.add_default_menuitems()
#    self._refresh() # useless, duplicate

    self.add_options_group("Wallpaper", "Automatically change wallpaper")
    self.add_option(StringOption('Wallpaper', 'wallpaper_dir', self.wallpaper_dir, 'Wallpaper folder', 'Folder containing wallpapers'))
    self.add_option(IntOption('Wallpaper', 'wallpaper_interval', self.wallpaper_interval, 'Update interval (minutes)', 'Update interval', min=1, max=1440 * 5))
  
  
  def next_wallpaper(self):
      print "changing wallpaper from folder: " + self.wallpaper_dir
      self.wallpaper_last_change_time = time.time()
      if os.path.exists(self.wallpaper_dir):
          random_wallpaper(all_images_in_folder(self.wallpaper_dir))

  def update (self):
#    print str(self.__options__)
    if self.wallpaper_last_change_time == None:
      self.wallpaper_last_change_time = time.time()

    if time.time() - self.wallpaper_last_change_time > 60 * self.wallpaper_interval:
      self.next_wallpaper()
    
    if (self._skin):
      self._skin.update()
      # always update size
      self.width, self.height = self._skin.get_size()
  
    self.redraw_canvas()
    # print "Update finished"
    
    if (self._skin):
      # works on update interval
      if self._update_interval != self._skin.get_update_interval():
      
        self._update_interval = self._skin.get_update_interval()
        self.timer = gobject.timeout_add(self._update_interval, self.update)
        
        print "Update timer set to " + str(self._update_interval)
        
        # start new timer event
        return False
      
    # no need to update, keep running this timer event
    return True
    
  
  def on_mouse_down (self, event):
    """Called when a buttonpress-event occured in Screenlet's window. 
    Returning True causes the event to be not further propagated."""
    if event.button == 1: # left button down (right button is 3)
      self._skin.left_mouse_click(event.x, event.y)
    return False
    
  
#  def on_mouse_enter (self, event):
#    """Called when the mouse enters the Screenlet's window."""
#    self.hover = True
    
    
#  def on_mouse_leave (self, event):
#    """Called when the mouse leaves the Screenlet's window."""
#    self.hover = False
  
  
#  def _change_update_interval(self, new_update_interval):
#    """Change the default update interval"""
#    self._new_update_interval = new_update_interval
    
  
  def _refresh (self):
    """Called to reset a screentlet"""
    
    #TODO also need to update skin list
    
    dir_content = os.listdir(self.theme.path)
    ini_file_path = ""
    
    for f in dir_content:
      if f.lower().endswith(".ini"):
        ini_file_path = self.theme.path + os.path.sep + f
        break

    if (ini_file_path != ""):
      self._skin = Skin(ini_file_path, self.theme)
      self.width, self.height = self._skin.get_size()
    else:
      screenlets.show_error(self, "Oops, no .ini settings file found in '" + self.theme.path + "'!")


  def on_load_theme (self):
    """Called when the theme is reloaded(after loading, before redraw)"""
    self.write_config_file()
    self._refresh()
    
  def on_menuitem_select (self, id):
    if id == "refresh":
      self._refresh()
    if id == "next_wallpaper":
      self.next_wallpaper()
  
  
#  def on_quit (self):
#    """Callback to handle destroy event"""
#    return True
    

  def on_draw (self, ctx):
    # if theme is loaded
    
    if self.theme and self._skin:
      
      # set scale rel. to scale-attribute
      ctx.scale(self.scale, self.scale)
      #ctx.set_source_rgba(100, 100, 100, 0.4)
      #self.draw_circle(ctx, 0, 0, self.width, self.height)
      
      self._skin.paint(ctx, self)

      #ctx.set_source_rgba(100, 100, 100, 0.4)
      #self.draw_text(ctx, "Text 1", 0, 0, "Sans", 10, self.width, pango.ALIGN_LEFT)

      
      #self.draw_text(ctx, 'timer - ' + str(23), 0, 130, "Sans", 10, self.width, pango.ALIGN_LEFT)
      #self.draw_text(ctx, self.theme_name, 0, 50, "Sans", 10, self.width, pango.ALIGN_LEFT)

      # render svg-file
      # self.theme['example-bg.svg'].render_cairo(ctx)
      # render png-file
      # ctx.set_source_surface(self.theme['example-test.png'], 0, 0)
      # ctx.paint()
  
  
  def on_draw_shape (self, ctx):
    self.on_draw(ctx)
    

if __name__ == "__main__":
    import screenlets.session
    screenlets.session.create_session(RainmeterAdapterScreenlet)

