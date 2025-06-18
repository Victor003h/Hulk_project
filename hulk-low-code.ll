; ModuleID = 'main_module'
source_filename = "main_module"

%PolarPoint = type { double }

declare i32 @printf(ptr, ...)

declare i32 @puts(ptr)

define i32 @main() {
entry:
}

define double @PolarPoint_get(double %self) {
entry:
  %self1 = alloca double, align 8
  store double %self, ptr %self1, align 8
  ret double 2.000000e+00
}

define ptr @new_PolarPoint() {
entry:
  %0 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%PolarPoint, ptr null, i32 1) to i64))
  %1 = getelementptr inbounds %PolarPoint, ptr %0, i32 0, i32 0
  store double 2.000000e+00, ptr %1, align 8
  ret ptr %0
}

declare ptr @malloc(i64)
