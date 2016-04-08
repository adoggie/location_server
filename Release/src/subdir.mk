################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/addin.cpp \
../src/config.cpp \
../src/geo.cpp \
../src/http.cpp \
../src/location.cpp \
../src/map.cpp \
../src/message.cpp \
../src/mq.cpp \
../src/pool.cpp \
../src/query.cpp \
../src/server.cpp \
../src/store.cpp \
../src/sync.cpp 

C_SRCS += \
../src/mongoose.c 

OBJS += \
./src/addin.o \
./src/config.o \
./src/geo.o \
./src/http.o \
./src/location.o \
./src/map.o \
./src/message.o \
./src/mongoose.o \
./src/mq.o \
./src/pool.o \
./src/query.o \
./src/server.o \
./src/store.o \
./src/sync.o 

C_DEPS += \
./src/mongoose.d 

CPP_DEPS += \
./src/addin.d \
./src/config.d \
./src/geo.d \
./src/http.d \
./src/location.d \
./src/map.d \
./src/message.d \
./src/mq.d \
./src/pool.d \
./src/query.d \
./src/server.d \
./src/store.d \
./src/sync.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I../../../include -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


