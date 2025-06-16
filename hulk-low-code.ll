; ModuleID = 'main_module'
source_filename = "main_module"

@0 = private unnamed_addr constant [4 x i8] c"%g\0A\00", align 1

declare i32 @printf(ptr, ...)

declare i32 @puts(ptr)

define i32 @main() {
entry:
}

define double @suma(double %x, double %y) {
entry:
  %y2 = alloca double, align 8
  %x1 = alloca double, align 8
  store double %x, ptr %x1, align 8
  store double %y, ptr %y2, align 8
  %x3 = load double, ptr %x1, align 8
  %y4 = load double, ptr %y2, align 8
  %addtmp = fadd double %x3, %y4
  ret double %addtmp
  %0 = call i32 (ptr, ...) @printf(ptr @0, double %addtmp)
  ret i32 0
}
