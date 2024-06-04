cmake_minimum_required(VERSION 3.27)
project(2024_I2P2_TowerDefense_with_answer)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")

include_directories(.)
include_directories(Resource)

file(COPY Resource/audios DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY Resource/fonts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY Resource/images DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY Resource DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY allegro_monolith-5.2.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(2024_I2P2_TowerDefense_with_answer
        Engine/Allegro5Exception.cpp
        Engine/AudioHelper.cpp
        Bullet/Bullet.cpp
        Engine/Collider.cpp
        UI/Animation/DirtyEffect.cpp
        Enemy/Enemy.cpp
        UI/Animation/ExplosionEffect.cpp
        Bullet/FireBullet.cpp
        Engine/GameEngine.cpp
        Engine/Group.cpp
        Engine/IControl.cpp
        UI/Component/Image.cpp
        UI/Component/ImageButton.cpp
        UI/Component/HoverImageButton.cpp
        Engine/IObject.cpp
        Engine/IScene.cpp
        UI/Component/Label.cpp
        Bullet/LaserBullet.cpp
        Turret/LaserTurret.cpp
        Engine/LOG.cpp
        Scene/LoseScene.cpp
        Turret/MachineGunTurret.cpp
        main.cpp
        Bullet/MissileBullet.cpp
        Bullet/AdvancedMissileBullet.cpp
        Turret/MissileTurret.cpp
        Turret/AdvancedMissileTurret.cpp
        UI/Animation/Plane.cpp
        Enemy/PlaneEnemy.cpp
        Scene/PlayScene.cpp
        Engine/Point.cpp
        Engine/Resources.cpp
        UI/Component/Slider.cpp
        Enemy/SoldierEnemy.cpp
        Enemy/AdvancedTankEnemy.cpp
        Engine/Sprite.cpp
        Scene/StageSelectScene.cpp
        Scene/DifficultySelectScene.cpp
        Enemy/TankEnemy.cpp
        Turret/Turret.cpp
        Turret/TurretButton.cpp
        Turret/HoverTurretButton.cpp
        Turret/Shovel.cpp
        Scene/WinScene.cpp
        Scene/MapCreateScene.cpp
        Scene/ModeSelectScene.cpp
        Scene/ReversePlayScene.cpp

        Scene/StartScene.cpp
        Scene/SettingsScene.cpp
        Scene/ScoreBoardScene.cpp
        UI/Component/TextInput.cpp

)


target_include_directories(2024_I2P2_TowerDefense_with_answer PRIVATE C:/vcpkg/packages/mlpack_x64-windows/include)
target_include_directories(2024_I2P2_TowerDefense_with_answer PRIVATE C:/vcpkg/packages/armadillo_x64-windows/include)
target_include_directories(2024_I2P2_TowerDefense_with_answer PRIVATE C:/vcpkg/packages/cereal_x64-windows/include)
target_include_directories(2024_I2P2_TowerDefense_with_answer PRIVATE C:/vcpkg/packages/ensmallen_x64-windows/include)

IF (WIN32)
    target_include_directories(2024_I2P2_TowerDefense_with_answer PUBLIC C:/allegro/include)
    target_link_directories(2024_I2P2_TowerDefense_with_answer PUBLIC C:/allegro/lib)
    target_compile_options(2024_I2P2_TowerDefense_with_answer PUBLIC ${ALLEGRO5_CFLAGS_OTHER})
    target_link_libraries(2024_I2P2_TowerDefense_with_answer liballegro_monolith)
ELSE()
    find_package(PkgConfig)
    pkg_check_modules(ALLEGRO5 REQUIRED allegro-5 allegro_main-5 allegro_font-5 allegro_dialog-5 allegro_primitives-5 allegro_image-5 allegro_ttf-5 allegro_audio-5 allegro_acodec-5)
    target_include_directories(2024_I2P2_TowerDefense_with_answer PUBLIC lib)
    target_include_directories(2024_I2P2_TowerDefense_with_answer PUBLIC ${ALLEGRO5_INCLUDE_DIRS})
    target_compile_options(2024_I2P2_TowerDefense_with_answer PUBLIC ${ALLEGRO5_CFLAGS_OTHER})
    target_link_libraries(2024_I2P2_TowerDefense_with_answer ${ALLEGRO5_LIBRARIES})
    target_link_directories(2024_I2P2_TowerDefense_with_answer PUBLIC ${ALLEGRO5_LIBRARY_DIRS})
ENDIF()