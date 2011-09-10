#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>

#include "music.h"
#include "util.h"
#include "compile.h"

void statusout(int cur, int max) {
	fprintf(stderr, "%u/%u\r", cur, max);
}

int main(int argc, char **argv) {
	int datalen;
	int speaker;
	song *song;

	int sndcap;

	if(argc < 3) {
		fprintf(stderr, "USAGE: %s <path to event device> <music>\n", argv[0]);
		exit(-1);
	}

	speaker = open(argv[1], O_RDWR);
	if(speaker == -1) {
		perror("open");
		exit(-2);
	}

	if(ioctl(speaker, EVIOCGBIT(0, EV_MAX), &sndcap) < 0) {
		perror("ioctl");
		exit(-3);
	}

	if(sndcap & EV_SND) {
		fprintf(stderr, "Device %s does not support sound.\n", argv[1]);
		exit(-4);
	}

	song = malloc(sizeof(song));
	if(song == NULL) {
		return -1;
	}

	if(compilesong_MZX(song, argv[2], strlen(argv[2])) != 0) {
		fprintf(stderr, "Song compile failed.\n");
	}
	playsong(speaker, song, statusout);
	fprintf(stderr, "\n");

	exit(0);
}