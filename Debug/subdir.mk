################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rw_serial.c \
../rw_test.c \
../seek.c \
../serial.c 

OBJS += \
./rw_serial.o \
./rw_test.o \
./seek.o \
./serial.o 

C_DEPS += \
./rw_serial.d \
./rw_test.d \
./seek.d \
./serial.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


