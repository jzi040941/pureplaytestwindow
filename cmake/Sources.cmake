#########################################
#                                       #
#      Source files                     #
#                                       #
#########################################

list(APPEND SRC

    
	pureplaytestwindow.cpp
    #src/Module.cpp

    #Util
    src/audio/RtAudio.cpp
    #src/util/WAV.cpp   
    #src/util/config.cpp   
    #src/util/threads.cpp
    #src/util/memory.cpp
    #src/util/ColorMap.cpp
    src/util/RT_Base.cpp
    src/util/RT_Output.cpp  
    src/util/CircularBuffer.cpp

    )



# 헤더를 명시하지 않으면 
# VS에서 사용시 헤더가 전부다 외부 종속성으로
# 넘어가서 관리하기가 귀찮아진다. 

list(APPEND HEAD
    # intermediates
    #include/Sub.h 
    #include/top.h
    #include/bottom.h
    #include/Module.h
    #include/UI_Module.h

    # util
    include/util/RT_Base.h
    include/util/RT_Input.h
    include/util/RT_Output.h
    #include/util/WAV.h
    #include/util/TestModule.h
    #include/util/ColorMap.h
    include/util/CircularBuffer.h
    )


