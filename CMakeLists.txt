cmake_minimum_required(VERSION 2.8)
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

set(SHORT_NAME shiromino)
project(${SHORT_NAME})
include("${VITASDK}/share/vita.cmake" REQUIRED)
# Uncomment for extra memory
# set(VITA_MKSFOEX_FLAGS "-d ATTRIBUTE2=12")
set(VITA_APP_NAME "Shiromino")
set(VITA_TITLEID  "TEAR00001")
set(VITA_VERSION  "02.00")

if(CMAKE_BUILD_TYPE MATCHES Debug)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSQLITE_OS_OTHER=1 -DSQLITE_TEMP_STORE=3 -DSQLITE_THREADSAFE=0 -Wall -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g")
else()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSQLITE_OS_OTHER=1 -DSQLITE_TEMP_STORE=3 -DSQLITE_THREADSAFE=0 -Wall -O3 -mtune=cortex-a9")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3 -mtune=cortex-a9 -fno-exceptions")
endif()

add_executable(${SHORT_NAME}
  src/main.cpp
  src/audio.cpp
  src/bstrlib.cpp
  src/core.cpp
  src/file_io.cpp
  src/game_menu.cpp
  src/game_qs.cpp
  src/gfx.cpp
  src/gfx_menu.cpp
  src/gfx_qs.cpp
  src/grid.cpp
  src/piecedef.cpp
  src/qrs.cpp
  src/random.cpp
  src/replay.cpp
  src/rotation_tables.cpp
  src/scores.cpp
  src/timer.cpp
  src/debug.cpp
  src/SGUIL/SGUIL.cpp
  src/SGUIL/SGUIL_GuiButton.cpp
  src/SGUIL/SGUIL_GuiDropDownList.cpp
  src/SGUIL/SGUIL_GuiOptionButton.cpp
  src/SGUIL/SGUIL_GuiTextField.cpp
  src/SGUIL/SGUIL_GuiWindow.cpp
)

target_link_libraries(${SHORT_NAME}
  SDL2_image
  SDL2_mixer
  SDL2
  vita2d
  mikmod
  mpg123
  FLAC
  vorbisfile
  vorbis
  ogg
  jpeg
  png
  z
  debugnet
  c
  sqlite
  SceSqlite_stub
  SceAudio_stub
  SceGxm_stub
  SceDisplay_stub
  SceCommonDialog_stub
  SceTouch_stub
  SceCtrl_stub
  SceHid_stub
  SceNetCtl_stub
  SceSysmodule_stub
)

vita_create_self(${SHORT_NAME}.self ${SHORT_NAME})
vita_create_vpk(${SHORT_NAME}.vpk ${VITA_TITLEID} ${SHORT_NAME}.self
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  FILE sce_sys/icon0.png sce_sys/icon0.png
  FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
  FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
  FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
  
  FILE audio/danger.wav audio/danger.wav
  FILE audio/dropfield.wav audio/dropfield.wav
  FILE audio/gameover.wav audio/gameover.wav
  FILE audio/gradeup.wav audio/gradeup.wav
  FILE audio/land.wav audio/land.wav
  FILE audio/levelstop.wav audio/levelstop.wav
  FILE audio/lineclear.wav audio/lineclear.wav
  FILE audio/lock.wav audio/lock.wav
  FILE audio/medal.wav audio/medal.wav
  FILE audio/newsection.wav audio/newsection.wav
  FILE audio/piece0.wav audio/piece0.wav
  FILE audio/piece1.wav audio/piece1.wav
  FILE audio/piece2.wav audio/piece2.wav
  FILE audio/piece3.wav audio/piece3.wav
  FILE audio/piece4.wav audio/piece4.wav
  FILE audio/piece5.wav audio/piece5.wav
  FILE audio/piece6.wav audio/piece6.wav
  FILE audio/piece7.wav audio/piece7.wav
  FILE audio/prerotate.wav audio/prerotate.wav
  FILE audio/track0.ogg audio/track0.ogg
  FILE audio/track1.ogg audio/track1.ogg
  FILE audio/track2.ogg audio/track2.ogg
  FILE audio/track3.ogg audio/track3.ogg
  FILE audio/g3/track5.ogg audio/g3/track5.ogg
  
  FILE gfx/bg-temp.png gfx/bg-temp.png
  FILE gfx/bg0.png gfx/bg0.png
  FILE gfx/bg1.png gfx/bg1.png
  FILE gfx/bg2.png gfx/bg2.png
  FILE gfx/bg3.png gfx/bg3.png
  FILE gfx/bg4.png gfx/bg4.png
  FILE gfx/bg5.png gfx/bg5.png
  FILE gfx/bg6.png gfx/bg6.png
  FILE gfx/bg7.png gfx/bg7.png
  FILE gfx/bg8.png gfx/bg8.png
  FILE gfx/bg9.png gfx/bg9.png
  FILE gfx/bg10.png gfx/bg10.png
  FILE gfx/bg11.png gfx/bg11.png
  FILE gfx/bg12.png gfx/bg12.png
  FILE gfx/bg_darken.png gfx/bg_darken.png
  FILE gfx/font.png gfx/font.png
  FILE gfx/font_fixedsys_excelsior.png gfx/font_fixedsys_excelsior.png
  FILE gfx/font_no_outline.png gfx/font_no_outline.png
  FILE gfx/font_outline_only.png gfx/font_outline_only.png
  FILE gfx/font_small.png gfx/font_small.png
  FILE gfx/font_square_no_outline.png gfx/font_square_no_outline.png
  FILE gfx/font_square_no_outline_16x16.png gfx/font_square_no_outline_16x16.png
  FILE gfx/font_square_outline_only.png gfx/font_square_outline_only.png
  FILE gfx/font_square_outline_only_16x16.png gfx/font_square_outline_only_16x16.png
  FILE gfx/font_thin.png gfx/font_thin.png
  FILE gfx/font_thin_no_outline.png gfx/font_thin_no_outline.png
  FILE gfx/font_thin_outline_only.png gfx/font_thin_outline_only.png
  FILE gfx/font_tiny.png gfx/font_tiny.png
  FILE gfx/medals.png gfx/medals.png
  FILE gfx/misc.png gfx/misc.png
  FILE gfx/playfield_grid.png gfx/playfield_grid.png
  FILE gfx/playfield_grid_alt.png gfx/playfield_grid_alt.png
  FILE gfx/tetrion_qs_white.png gfx/tetrion_qs_white.png
  FILE gfx/tets_bright_qs.png gfx/tets_bright_qs.png
  FILE gfx/tets_bright_qs_small.png gfx/tets_bright_qs_small.png
  FILE gfx/tets_dark_qs.png gfx/tets_dark_qs.png
  FILE gfx/tets_dark_qs_hollow.png gfx/tets_dark_qs_hollow.png
  FILE gfx/tets_shelled.png gfx/tets_shelled.png
  FILE gfx/animation/lineclear0.png gfx/animation/lineclear0.png
  FILE gfx/animation/lineclear1.png gfx/animation/lineclear1.png
  FILE gfx/animation/lineclear2.png gfx/animation/lineclear2.png
  FILE gfx/animation/lineclear3.png gfx/animation/lineclear3.png
  FILE gfx/animation/lineclear4.png gfx/animation/lineclear4.png
  FILE gfx/g1/tetrion_g1.png gfx/g1/tetrion_g1.png
  FILE gfx/g2/tetrion_g2_death.png gfx/g2/tetrion_g2_death.png
  FILE gfx/g2/tetrion_g2_master.png gfx/g2/tetrion_g2_master.png
  FILE gfx/g3/tetrion_g3_terror.png gfx/g3/tetrion_g3_terror.png
  
  FILE game.cfg game.cfg
  FILE audio/volume.cfg audio/volume.cfg
)
