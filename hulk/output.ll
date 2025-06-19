; ModuleID = 'main_module'
source_filename = "main_module"

%Engine = type { double }
%Car = type { %Engine }
%Driver = type { %Car }

@formatStr = private constant [4 x i8] c"%g\0A\00"

declare i32 @printf(ptr, ...)

declare i32 @puts(ptr)

define i32 @main() {
entry:
  %d = alloca ptr, align 8
  %calltmp = call ptr @new_Driver()
  store ptr %calltmp, ptr %d, align 8
  %d1 = load ptr, ptr %d, align 8
  %calltmp2 = call double @maxSpeed_Driver(ptr %d1)
  %0 = call i32 (ptr, ...) @printf(ptr @formatStr, double %calltmp2)
  ret i32 0
}

define ptr @new_Engine() {
entry:
  %0 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%Engine, ptr null, i32 1) to i64))
  %power = getelementptr inbounds %Engine, ptr %0, i32 0, i32 0
  store double 1.500000e+02, ptr %power, align 8
  ret ptr %0
}

declare ptr @malloc(i64)

define double @getPower_Engine(ptr %self) {
entry:
  %self1 = alloca ptr, align 8
  store ptr %self, ptr %self1, align 8
  %self2 = load ptr, ptr %self1, align 8
  %power_ptr = getelementptr inbounds %Engine, ptr %self2, i32 0, i32 0
  %power = load double, ptr %power_ptr, align 8
  ret double %power
}

define ptr @new_Car() {
entry:
  %0 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%Car, ptr null, i32 1) to i64))
  %calltmp = call ptr @new_Engine()
  %engine = getelementptr inbounds %Car, ptr %0, i32 0, i32 0
  store ptr %calltmp, ptr %engine, align 8
  ret ptr %0
}

define double @enginePower_Car(ptr %self) {
entry:
  %self1 = alloca ptr, align 8
  store ptr %self, ptr %self1, align 8
  %self2 = load ptr, ptr %self1, align 8
  %engine_ptr = getelementptr inbounds %Car, ptr %self2, i32 0, i32 0
  %engine = load ptr, ptr %engine_ptr, align 8
  %calltmp = call double @getPower_Engine(ptr %engine)
  ret double %calltmp
}

define ptr @new_Driver() {
entry:
  %0 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%Driver, ptr null, i32 1) to i64))
  %calltmp = call ptr @new_Car()
  %car = getelementptr inbounds %Driver, ptr %0, i32 0, i32 0
  store ptr %calltmp, ptr %car, align 8
  ret ptr %0
}

define double @maxSpeed_Driver(ptr %self) {
entry:
  %self1 = alloca ptr, align 8
  store ptr %self, ptr %self1, align 8
  %self2 = load ptr, ptr %self1, align 8
  %car_ptr = getelementptr inbounds %Driver, ptr %self2, i32 0, i32 0
  %car = load ptr, ptr %car_ptr, align 8
  %calltmp = call double @enginePower_Car(ptr %car)
  ret double %calltmp
}
