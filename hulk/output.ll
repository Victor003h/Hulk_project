; ModuleID = 'main_module'
source_filename = "main_module"

%Animal = type { double }
%Dog = type { %Animal, double }

@formatStr = private constant [4 x i8] c"%g\0A\00"

declare i32 @printf(ptr, ...)

declare i32 @puts(ptr)

define i32 @main() {
entry:
  %dog = alloca ptr, align 8
  %calltmp = call ptr @new_Dog(double 8.000000e+00)
  store ptr %calltmp, ptr %dog, align 8
  %dog1 = load ptr, ptr %dog, align 8
  %calltmp2 = call double @speak1_Dog(ptr %dog1)
  %0 = call i32 (ptr, ...) @printf(ptr @formatStr, double %calltmp2)
  ret i32 0
}

define ptr @new_Animal(double %age) {
entry:
  %age1 = alloca double, align 8
  %0 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%Animal, ptr null, i32 1) to i64))
  store double %age, ptr %age1, align 8
  %age2 = load double, ptr %age1, align 8
  %age3 = getelementptr inbounds %Animal, ptr %0, i32 0, i32 0
  store double %age2, ptr %age3, align 8
  ret ptr %0
}

declare ptr @malloc(i64)

define double @speak_Animal(ptr %self) {
entry:
  %self1 = alloca ptr, align 8
  store ptr %self, ptr %self1, align 8
  %self2 = load ptr, ptr %self1, align 8
  %age_ptr = getelementptr inbounds %Animal, ptr %self2, i32 0, i32 0
  %age = load double, ptr %age_ptr, align 8
  ret double %age
}

define ptr @new_Dog(double %0) {
entry:
  %1 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%Dog, ptr null, i32 1) to i64))
  %Animal = call ptr @new_Animal(double %0)
  %base = getelementptr inbounds %Dog, ptr %1, i32 0, i32 0
  call void @llvm.memcpy.p0.p0.i64(ptr align 8 %base, ptr align 8 %Animal, i64 ptrtoint (ptr getelementptr (%Animal, ptr null, i32 1) to i64), i1 false)
  %breed = getelementptr inbounds %Dog, ptr %1, i32 0, i32 1
  store double 1.000000e+00, ptr %breed, align 8
  ret ptr %1
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #0

define double @speak1_Dog(ptr %self) {
entry:
  %self1 = alloca ptr, align 8
  store ptr %self, ptr %self1, align 8
  %self2 = load ptr, ptr %self1, align 8
  %parent_ptr = getelementptr inbounds %Dog, ptr %self2, i32 0, i32 0
  %calltmp = call double @speak_Animal(ptr %self2)
  %self3 = load ptr, ptr %self1, align 8
  %breed_ptr = getelementptr inbounds %Dog, ptr %self3, i32 0, i32 1
  %breed = load double, ptr %breed_ptr, align 8
  %addtmp = fadd double %calltmp, %breed
  ret double %addtmp
}

attributes #0 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
