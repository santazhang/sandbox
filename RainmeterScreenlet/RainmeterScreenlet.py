#!/usr/bin/env python
#
# Rainmeter on Screenlets by Santa Zhang <santa1987@gmail.com>
#
# This application is released under the GNU General Public License 
# v3 (or, at your option, any later version). You can find the full 
# text of the license under http://www.gnu.org/licenses/gpl.txt. 
# By using, editing and/or distributing this software you agree to 
# the terms and conditions of this license. 

import screenlets
from screenlets.options import StringOption

import gtk
import pango
import gettext

_ = screenlets.utils.get_translator(__file__)

def tdoc(obj):
    obj.__doc__ = _(obj.__doc__)
    return obj

@tdoc
class RainmeterScreenlet(screenlets.Screenlet):
    """Rainmeter on Screenlets"""
    
    __name__ = "RainmeterScreenlet"
    __version__ = "0.3"
    __author__ = "Santa Zhang <santa1987@gmail.com>"
    # __requires__ = []
    
    # __category__ = ""
    __desc__ = __doc__
    
    # TODO remove demo code
    test_text = 'Hi.. im a screenlet'
    demo_text = ''
    demo_number = ''
    int_example = 1
    bool_example = True
    time_example =  (7, 30, 0)
    account_example =  ('','')
    color_example =(0.0, 0.0, 0.0, 1)
    font_example = "Sans Medium 5"
    image_example = ''
    file_example = ''
    directory_example = ''
    list_example = ('','')
    
    # TODO remove demo code
    hover = False
    
    def __init__(self, **kw):
        screenlets.Screenlet.__init__(self, width=500, height=500, uses_theme=False, **kw)
        self.theme_name = "SmoothSquarePurple" # TODO choose a proper theme
    
    def on_after_set_atribute(self,name, value):
        """Called after setting screenlet atributes"""
        print name + ' is going to change from ' + str(value)
        pass

    def on_before_set_atribute(self,name, value):
        """Called before setting screenlet atributes"""
        print name + ' has changed to ' + str(value)
        pass

    def on_create_drag_icon(self):
        """Called when the screenlet's drag-icon is created. You can supply
        your own icon and mask by returning them as a 2-tuple."""
        return (None, None)

    def on_composite_changed(self):
        """Called when composite state has changed"""
        pass

    def on_drag_begin(self, drag_context):
        """Called when the Screenlet gets dragged."""
        pass
    
    def on_drag_enter(self, drag_context, x, y, timestamp):
        """Called when something gets dragged into the Screenlets area."""
        pass
    
    def on_drag_leave(self, drag_context, timestamp):
        """Called when something gets dragged out of the Screenlets area."""
        pass

    def on_drop(self, x, y, sel_data, timestamp):
        """Called when a selection is dropped on this Screenlet."""
        return False
        
    def on_focus(self, event):
        """Called when the Screenlet's window receives focus."""
        pass
    
    def on_hide(self):
        """Called when the Screenlet gets hidden."""
        pass
    
    def on_init(self):
        """Called when the Screenlet's options have been applied and the 
        screenlet finished its initialization. If you want to have your
        Screenlet do things on startup you should use this handler."""
        print 'i just got started'
        # add menu item
        self.add_menuitem("at_runtime", "A-demo")
        
        self.add_menuitem("menu_todo1", "name of this screenlet") # click to open screen let folder
        self.add_menuitem("", "-")
        self.add_menuitem("menu_todo2", "varients") # ini of same theme
        self.add_menuitem("menu_todo3", "all theme items") # clock, .., etc
        self.add_menuitem("", "-")
        setting_menuitem = self.add_menuitem("menu_todo4", "settings") #
        setting_menu = gtk.Menu()
        setting_menuitem.set_submenu(setting_menu)
        
        from screenlets.menu import add_menuitem
        add_menuitem(setting_menu, "position")
        add_menuitem(setting_menu, "-")
        add_menuitem(setting_menu, "transparency")
        add_menuitem(setting_menu, "-")
        add_menuitem(setting_menu, "hide on mouse over")
        add_menuitem(setting_menu, "-")
        add_menuitem(setting_menu, "drag")
        
            # settings
                # position
                    # disp monitor
                    # -
                    # stay top most -> ... -> desktop
                    # from right, bottom ,x as ratio, y as ratio
                # tranparency
                    # 0% -> 90 %, fade in , fade out
                # hide on mouse over
                # draggable, movable , etc

        self.add_menuitem("", "-")
        self.add_menuitem("menu_todo5", "manage skin") #
        self.add_menuitem("menu_todo6", "edit skin") #
        self.add_menuitem("menu_todo7", "refresh skin") #
        self.add_menuitem("", "-")
        self.add_menuitem("menu_todo8", "raimeter") #
            # 
        self.add_menuitem("", "-")
        self.add_menuitem("menu_todo9", "unload skin") #
        # add default menu items
        #self.add_default_menuitems()


    def on_key_down(self, keycode, keyvalue, event):
        """Called when a keypress-event occured in Screenlet's window."""
        key = gtk.gdk.keyval_name(event.keyval)
        
        if key == "Return" or key == "Tab":
            screenlets.show_message(self, 'This is the ' + self.__name__ +'\n' + 'It is installed in ' + self.__path__)
    
    def on_load_theme(self):
        """Called when the theme is reloaded (after loading, before redraw)."""
        pass
    
    def on_menuitem_select(self, id):
        """Called when a menuitem is selected."""
        if id == "at_runtime":
            screenlets.show_message(self, 'This is an example on a menu created at runtime')
        if id == "at_xml":
            screenlets.show_message(self, 'This is an example on a menu created in the menu.xml')
        pass
    
    def on_mouse_down(self, event):
        """Called when a buttonpress-event occured in Screenlet's window. 
        Returning True causes the event to be not further propagated."""
        
        return False
    
    def on_mouse_enter(self, event):
        """Called when the mouse enters the Screenlet's window."""
        print self.x + self.mousex
        print self.y+self.mousey
        #self.theme.show_tooltip("this is a tooltip , it is set to shows on mouse hover",self.x+self.mousex,self.y+self.mousey)
        self.hover = True
        print 'mouse is over me'
        
    def on_mouse_leave(self, event):
        """Called when the mouse leaves the Screenlet's window."""
        self.theme.hide_tooltip()
        self.hover = False
        print 'mouse leave'

    def on_mouse_move(self, event):
        """Called when the mouse moves in the Screenlet's window."""
        self.redraw_canvas()
        pass

    def on_mouse_up(self, event):
        """Called when a buttonrelease-event occured in Screenlet's window. 
        Returning True causes the event to be not further propagated."""
        return False
    
    def on_quit(self):
        """Callback for handling destroy-event. Perform your cleanup here!"""
        screenlets.show_question(self, 'Do you like screenlets?')
        return True
        
    def on_realize(self):
        """"Callback for handling the realize-event."""
    
    def on_scale(self):
        """Called when Screenlet.scale is changed."""
        pass
    
    def on_scroll_up(self):
        """Called when mousewheel is scrolled up (button4)."""
        pass

    def on_scroll_down(self):
        """Called when mousewheel is scrolled down (button5)."""
        pass
    
    def on_show(self):
        """Called when the Screenlet gets shown after being hidden."""
        pass
    
    def on_switch_widget_state(self, state):
        """Called when the Screenlet enters/leaves "Widget"-state."""
        pass
    
    def on_unfocus(self, event):
        """Called when the Screenlet's window loses focus."""
        pass
    
    def on_draw(self, ctx):
        """In here we load the theme"""
        # if theme is loaded
        if self.theme:
            # set scale rel. to scale-attribute
            ctx.scale(self.scale, self.scale)
            ctx.set_source_rgba(self.color_example[2], self.color_example[1], self.color_example[0],0.4)    
            #if self.hover:
            #    self.theme.draw_rounded_rectangle(ctx,0,0,20,self.width,self.height)
            self.draw_circle(ctx,0,0,self.width,self.height)
            # TEST: render example-bg into context (either PNG or SVG)
            #self.theme.render(ctx, 'example-bg')
            ctx.set_source_rgba( self.color_example[0], self.color_example[1], self.color_example[2],self.color_example[3])
            self.draw_text(ctx, self.test_text, 0, 0, self.font_example , 10,self.width,pango.ALIGN_LEFT)
            self.draw_line(ctx,0,40,self.width,0,1)
            self.draw_text(ctx, 'timer - ' + str(self.demo_number), 0, 130, self.font_example , 10, self.width,pango.ALIGN_LEFT)
            self.draw_text(ctx, self.theme_name, 0, 50, self.font_example , 10, self.width,pango.ALIGN_LEFT)
            self.draw_text(ctx, 'mouse x ' + str(self.mousex ) + ' \n mouse y ' + str(self.mousey ) , 0, 170, self.font_example , 10,self.width,pango.ALIGN_LEFT)

            # render svg-file
            #self.theme['example-bg.svg'].render_cairo(ctx)
            # render png-file
            #ctx.set_source_surface(self.theme['example-test.png'], 0, 0)
            #ctx.paint()
    
    def on_draw_shape(self, ctx):
        self.on_draw(ctx)


if __name__ == "__main__":
    import screenlets.session
    screenlets.session.create_session(RainmeterScreenlet)

