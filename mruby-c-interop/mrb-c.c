// http://matt.aimonetti.net/posts/2012/04/25/getting-started-with-mruby/
// http://rmosolgo.github.io/blog/2014/11/21/defining-mruby-methods-with-c/

#include <stdio.h>
#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/string.h>

void greeting_c(const char* msg) {
    printf("  *** greetings from c: '%s' ***\n", msg);
}

mrb_value greeting_c_wrapper(mrb_state *mrb, mrb_value self) {
    mrb_value s = mrb_str_new_cstr(mrb, "ALOHA~");
    mrb_get_args(mrb, "|S", &s);
    greeting_c(mrb_string_value_cstr(mrb, &s));
    return mrb_nil_value();
}

int main() {
    printf("This shall be done!\n");
    mrb_state *mrb = mrb_open();

    struct RClass *mrb_demo;
    mrb_demo = mrb_define_module(mrb, "Demo");

    mrb_value s = mrb_str_new_cstr(mrb, "ALOHA");
    mrb_define_const(mrb, mrb_demo, "Greetings", s);

    mrb_define_module_function(mrb, mrb_demo, "c_greetings", greeting_c_wrapper, MRB_ARGS_OPT(1));

    mrb_load_string(mrb, "p \"greetings from mruby: #{Demo::Greetings}\";");  // c call mruby
    mrb_load_string(mrb, "Demo.c_greetings 'aloha'");  // mruby call c
    mrb_load_string(mrb, "p 1234");

    mrb_funcall(mrb, mrb_obj_value(mrb_demo), "c_greetings", 1, s);  // c call mruby

    mrb_close(mrb);
    return 0;
}
