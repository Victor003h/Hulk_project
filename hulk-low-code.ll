; ModuleID = 'main_module'
source_filename = "main_module"

%0 = type { double, double }

@formatStr = private constant [4 x i8] c"%g\0A\00"

declare i32 @printf(ptr, ...)

declare i32 @puts(ptr)

define i32 @main() {
entry:
}

define ptr @_ctor(double %arg0, double %arg1) {
entry:
  %self = alloca %0, align 8
  %ptr = getelementptr inbounds %0, ptr %self, i32 0, i32 0
  store double %arg0, ptr %ptr, align 8
  %ptr1 = getelementptr inbounds %0, ptr %self, i32 0, i32 1
  store double %arg1, ptr %ptr1, align 8
  ret ptr %self
  %0 = call i32 (ptr, ...) @printf(ptr @formatStr, double 2.000000e+00)
  ret i32 0
}

define double @getX() {
entry:
  ret double 3.000000e+00
}

define double @getY() {
entry:
  ret double 2.000000e+00
}
