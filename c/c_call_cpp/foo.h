#ifndef FOO_H_
#define FOO_H_

#ifdef __cplusplus

class foo {
  public:
    void output();
};

extern "C" {
#endif  // __cplusplus

void foo_for_c();

#ifdef __cplusplus
}
#endif


#endif // FOO_H_

