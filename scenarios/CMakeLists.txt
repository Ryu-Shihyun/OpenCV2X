add_subdirectory(artery)
add_subdirectory(gemv2)
add_subdirectory(highway-police)
if(WITH_ENVMOD)
    add_subdirectory(rsu_grid)
    add_subdirectory(m50)
    add_subdirectory(test-services)
    add_subdirectory(test-services-cv2x)
endif()
if(WITH_STORYBOARD)
    add_subdirectory(storyboard)
endif()
if(WITH_TESTBED)
    add_subdirectory(testbed)
endif()
if(WITH_TRANSFUSION)
    add_subdirectory(transfusion)
endif()
if(WITH_OTS)
    add_opp_run(ots_demo WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ots-demo)
endif()


if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/mt-its2017/omnetpp.ini AND WITH_STORYBOARD)
    add_opp_run(mtits2017 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/mt-its2017)
    add_opp_test(mtits2017 SUFFIX test SIMTIME_LIMIT 30s)
endif()

if(TARGET lte)
    # keep artery-lte target for backward-compatibility
    add_library(artery-lte ALIAS artery)
    add_opp_run(simulte WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/simulte)
    add_opp_test(simulte SUFFIX cellular CONFIG VoIP-UpLink SIMTIME_LIMIT 10s)
    add_opp_test(simulte SUFFIX d2d CONFIG AlertMulticast SIMTIME_LIMIT 25s)
    add_subdirectory(m50-cv2x)
    add_opp_run(simulte-cars
        DEPENDENCY artery-lte
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cars)
    add_opp_run(simulte-Mode4
            DEPENDENCY artery-lte
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Mode4)
    add_opp_run(simulte-Example
            DEPENDENCY artery-lte
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/example)
    add_opp_run(simulte-validation
            DEPENDENCY artery-lte
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Mode4-Validation)
    add_opp_run(personal-experiments
            DEPENDENCY artery-lte
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/personal-experiments)
    add_opp_run(personal-andres-experiments
            DEPENDENCY artery-lte
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/personal-andres-experiments)
    add_opp_run(personal-Validation-simulteApp
            DEPENDENCY artery-lte
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/personal-Validation-simulteApp)
    add_opp_run(personal-DummyRun
            DEPENDENCY artery-lte
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/personal-DummyRun)
    add_opp_run(personal-Validation
            DEPENDENCY artery-lte
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/personal-Validation)
    add_opp_run(personal-non-periodic
            DEPENDENCY artery-lte
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/personal-non-periodic)
    add_opp_test(simulte-cars SUFFIX d2d CONFIG D2D_MULTICAST SIMTIME_LIMIT 70s)
endif()

if(TARGET lte AND WITH_STORYBOARD)
    add_subdirectory(lte-blackice)
endif()

add_opp_run(car2car-grid WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/car2car-grid)
add_opp_test(car2car-grid SUFFIX cam_bsp CONFIG cam_bsp RUN 0 SIMTIME_LIMIT 30s)
