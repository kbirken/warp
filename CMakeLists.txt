cmake_minimum_required(VERSION 3.7)
project(warp)

set(CMAKE_CXX_STANDARD 11)

find_package(JNI)

if (JNI_FOUND)
    message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
    message (STATUS "JAVA_INCLUDE_PATH=${JAVA_INCLUDE_PATH}")
    message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")
    include_directories(${JNI_INCLUDE_DIRS})
endif()

set(WARP_CPP_ROOT src/main/cpp/org.nanosite.warp)
set(SRC ${WARP_CPP_ROOT}/src)
set(SRC_GEN ${WARP_CPP_ROOT}/src-gen)

include_directories(${SRC_GEN})
include_directories(${SRC})

set(COMMON_SRC
        ${SRC}/model/private/CBehavior.cpp
        ${SRC}/model/private/CFunctionBlock.cpp
        ${SRC}/model/private/CSimModel.cpp
        ${SRC}/model/private/CStep.cpp
        ${SRC}/model/private/Pool.cpp
        ${SRC}/model/private/Resource.cpp
        ${SRC}/model/CBehavior.h
        ${SRC}/model/CFunctionBlock.h
        ${SRC}/model/CSimModel.h
        ${SRC}/model/CStep.h
        ${SRC}/model/Pool.h
        ${SRC}/model/Resource.h
        ${SRC}/result/private/CIterationResult.cpp
        ${SRC}/result/private/CSimulationResult.cpp
        ${SRC}/result/CIterationResult.h
        ${SRC}/result/CSimulationResult.h
        ${SRC}/sim/private/PoolSim.cpp
        ${SRC}/sim/private/PoolSimVector.cpp
        ${SRC}/sim/PoolSim.h
        ${SRC}/sim/PoolSimVector.h
        ${SRC}/simulation/private/CAPSScheduler.cpp
        ${SRC}/simulation/private/CMessage.cpp
        ${SRC}/simulation/private/CMessageQueue.cpp
        ${SRC}/simulation/private/CResourceSlotVector.cpp
        ${SRC}/simulation/private/CSimCore.cpp
        ${SRC}/simulation/private/CTimeMonitor.cpp
        ${SRC}/simulation/private/CToken.cpp
        ${SRC}/simulation/private/CTokenFactory.cpp
        ${SRC}/simulation/CAPSScheduler.h
        ${SRC}/simulation/CIntAccuracy.h
        ${SRC}/simulation/CMessage.h
        ${SRC}/simulation/CMessageQueue.h
        ${SRC}/simulation/CResourceSlotVector.h
        ${SRC}/simulation/CSimCore.h
        ${SRC}/simulation/CTimeMonitor.h
        ${SRC}/simulation/CToken.h
        ${SRC}/simulation/CTokenFactory.h
        ${SRC}/simulation/ILogger.h
        ${SRC}/simulation/ISimEventAcceptor.h
        ${SRC}/simulation/IStepSuccessor.h
)

set(JNI_SRC
        ${SRC_GEN}/org_nanosite_warp_jni_Warp.h
        ${SRC_GEN}/org_nanosite_warp_jni_WarpBehavior.h
        ${SRC_GEN}/org_nanosite_warp_jni_WarpFunctionBlock.h
        ${SRC_GEN}/org_nanosite_warp_jni_WarpStep.h
        ${SRC}/warp_jni.cpp)

set(STANDALONE_SRC
        ${SRC}/warp.cpp)



add_executable(warp
        ${COMMON_SRC}
        ${STANDALONE_SRC}
)

add_library(warp_lib SHARED
        ${COMMON_SRC}
        ${JNI_SRC})
