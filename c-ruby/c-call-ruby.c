#include <stdio.h>
#include <ruby.h>

int ruby_add(int a, int b) {
	int r = 0;
	VALUE rb_a = INT2FIX(a);
	VALUE rb_b = INT2FIX(b);
	ID meth_id = rb_intern("+");
	VALUE rb_r = rb_funcall(rb_a, meth_id, 1, rb_b);
	r = FIX2INT(rb_r);
	return r;
}

void ruby_puts(const char* str) {
	VALUE rb_s = rb_str_new2(str);
	ID method_id = rb_intern("puts");
	VALUE rb_r = rb_funcall(rb_stdout, method_id, 1, rb_s);	
}

// call singleton
void ruby_puts2(const char* s) {
	VALUE rb_s = rb_str_new2(s);
	VALUE kernel = rb_const_get(rb_cObject, rb_intern("Kernel"));
	//kernel = rb_path2class("Kernel");
	rb_funcall(kernel, rb_intern("puts"), 1, rb_s);
	rb_funcall(kernel, rb_intern("puts"), 1, rb_funcall(kernel, rb_intern("class"), 0));
}

int main() {
	ruby_init();
	printf("%d\n", ruby_add(3, 234));
	ruby_puts("hello, world!\n");
	ruby_puts2("hello, world!\n");
	return 0;
}
