; ModuleID = 'main_module'
source_filename = "main_module"

@formatStr = private constant [4 x i8] c"%g\0A\00"

declare i32 @printf(ptr, ...)

declare i32 @puts(ptr)

define i32 @main() {
entry:
  %x = alloca double, align 8
  store double 2.000000e+00, ptr %x, align 8
  %x1 = load double, ptr %x, align 8
  %addtmp = fadd double %x1, 1.000000e+00
  %0 = call i32 (ptr, ...) @printf(ptr @formatStr, double %addtmp)
  ret i32 0
}
