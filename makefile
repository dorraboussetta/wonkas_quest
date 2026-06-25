wonka: common.o save_menu.o load_menu.o mainmenu.o player_mode.o player_selection.o submenu.o enemy.o player.o collision_bb.o collision_perfect.o minimap.o background.o outfit_menu.o main.o
	gcc common.o save_menu.o load_menu.o mainmenu.o player_mode.o player_selection.o submenu.o enemy.o player.o minimap.o collision_perfect.o collision_bb.o background.o outfit_menu.o main.o -o wonka -lSDL2_mixer -lSDL2_image -lSDL2_ttf -lSDL2 -g
common.o: common.c
	gcc -c common.c -g
save_menu.o: save_menu.c
	gcc -c save_menu.c -g
load_menu.o:load_menu.c
	gcc -c load_menu.c -g
mainmenu.o:mainmenu.c
	gcc -c mainmenu.c -g
player_mode.o:player_mode.c
	gcc -c player_mode.c -g
player_selection.o:player_selection.c
	gcc -c player_selection.c -g
submenu.o:submenu.c
	gcc -c submenu.c -g
main.o:main.c
	gcc -c main.c -g
enemy.o:enemy.c
	gcc -c enemy.c -g
player.o: player.c
	gcc -c player.c -g
background.o: background.c
	gcc -c background.c -g
minimap.o: minimap.c
	gcc -c minimap.c -g
collision_perfect.o: collision_perfect.c
	gcc -c collision_perfect.c -g
collision_bb.o: collision_bb.c
	gcc -c collision_bb.c -g



outfit_menu.o:outfit_menu.c
	gcc -c outfit_menu.c -g
