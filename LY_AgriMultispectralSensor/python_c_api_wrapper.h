#ifndef PYTHON_C_API_WRAPPER_H
#define PYTHON_C_API_WRAPPER_H

/* qt调用python.h，比如matplotlib-cpp等需要python引用的C++库
   #include <Python.h>报错
   \python\python37-32\include\object.h(448): error C2059: 语法错误:“;”
   \python\python37-32\include\object.h(448): error C2238: 意外的标记位于“;”之前
  原因: Python 和 Qt 的关键词 slots 冲突
*/

#define PY_SSIZE_T_CLEAN
#pragma push_macro("slots")
#undef slots
#include <Python.h>
#pragma pop_macro("slots")

#endif // PYTHON_C_API_WRAPPER_H