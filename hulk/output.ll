; ModuleID = 'main_module'
source_filename = "main_module"

@0 = private unnamed_addr constant [6 x i8] c"%.6g\0A\00", align 1

declare i32 @printf(ptr, ...)

declare i32 @puts(ptr)

define i32 @main() {
entry:
  %b = alloca double, align 8
  store double 3.000000e+00, ptr %b, align 8
  %b1 = load double, ptr %b, align 8
  %addtmp = fadd double %b1, 2.000000e+00
  %0 = call i32 (ptr, ...) @printf(ptr @0, double %addtmp)
  ret i32 0
}
