#include <linux/input.h>

#include "output.h"

#include "freqs.h"

int playnote(int fd, command *c) {
	struct input_event ev;
	ev.type = EV_SND;
	ev.code = SND_TONE;

	if(c == NULL) {
		ev.value = 0;
		if(write(fd, &ev, sizeof(struct input_event)) < 0)
			return(-1);
		return(-1);
	} else {
		switch(c->note) {
			case C:
			case CSHARP:
			case D:
			case DSHARP:
			case E:
			case F:
			case FSHARP:
			case G:
			case GSHARP:
			case A:
			case ASHARP:
			case B:
				if(c->octave > 6)
					return(-2);
				ev.value = (int)(notefreqs[c->octave][c->note]);
				if(write(fd, &ev, sizeof(struct input_event)) < 0)
					return(-1);
				break;
			default:
				ev.value = 0;
				if(write(fd, &ev, sizeof(struct input_event)) < 0)
					return(-1);
		}
	}
	return(0);
}

int playsong(int fd, song *s, void (*status)(int cur, int max)) {
	int i;
	struct timespec req, rem;
	unsigned long int notelen;

	i = 1;
	do {
		status(i, s->count);

		if(s->current->note <= REST) {
			if(playnote(fd, s->current) < 0)
				return(-1);

			notelen = 60000000000ul / s->bpm / s->current->divisor;
			if(s->current->duration == DOT) {
				notelen = notelen * 3 / 2;
			} else if(s->current->duration == TRIPLET) {
				notelen = notelen / 3;
			}
			req.tv_sec = notelen / 1000000000ul;
			req.tv_nsec = notelen % 1000000000ul;

			while(nanosleep(&req, &rem) == -1) {
				req.tv_sec = rem.tv_sec;
				req.tv_nsec = rem.tv_nsec;
			}
		} else {
			switch(s->current->note) {
				case BPM:
					s->bpm = s->current->octave;
					break;
				default:
					break;
			}
		}

		i++;
	} while(nextcommand(s) == 0);

	if(playnote(fd, NULL) < 0)
		return(-1);
	return(0);
}