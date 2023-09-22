
#pragma once

 int feclearexcept(int excepts);
 int fegetexceptflag(fexcept_t *flagp, int excepts);
 int feraiseexcept(int excepts);
 int fesetexcept(int excepts);
 int fesetexceptflag(const fexcept_t *flagp, int excepts);
 int fetestexceptflag(const fexcept_t * flagp, int excepts);
 int fetestexcept(int excepts);
 int fegetmode(femode_t *modep);
 int fegetround(void);
 int fesetmode(const femode_t *modep);
 int fesetround(int rnd);
 int fegetenv(fenv_t *envp);
 int feholdexcept(fenv_t *envp);
 int fesetenv(const fenv_t *envp);
 int feupdateenv(const fenv_t *envp);
