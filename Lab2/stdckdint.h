/* Implement C23-style overflow checking by using GCC-style builtins.  */
#define ckd_add(r, a, b) ((_Bool) __builtin_add_overflow (a, b, r))
#define ckd_sub(r, a, b) ((_Bool) __builtin_sub_overflow (a, b, r))
#define ckd_mul(r, a, b) ((_Bool) __builtin_mul_overflow (a, b, r))
