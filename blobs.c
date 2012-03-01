#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <assert.h>

typedef enum {
	BLOB_R,
	BLOB_RO,
	BLOB_L,
	BLOB_LO
} blob_t;

typedef struct my_node {
	char fname[255];
	struct my_node* next;
} ll_node;

void screen_clr(FILE* out) {
	fprintf(out, "\x1b[2J");
}

void line_clr(FILE* out) {
	fprintf(out, "\x1b[1A\x1b[2K");
}

void blob_pr(blob_t blob, FILE* out) {
	switch (blob) {
		case BLOB_R:
			fwprintf(out, L"\x1b[0;35m \x00002308\x1b[0;36m(^.^)\x1b[0;35m\x0000230B \n");
			break;
		case BLOB_RO:
			fwprintf(out, L"\x1b[0;35m\x00002308\x0000203E\x1b[0;36m(^.^)\x1b[0;35m_\x0000230B\n");
			break;
		case BLOB_L:
			fwprintf(out, L"\x1b[0;35m \x0000230A\x1b[0;36m(^.^)\x1b[0;35m\x00002309 \n");
			break;
		case BLOB_LO:
			fwprintf(out, L"\x1b[0;35m\x0000230A_\x1b[0;36m(^.^)\x1b[0;35m\x0000203E\x00002309\n");
			break;
		default:
			break;
	}
}

int main (int argc, char** argv) {
	
	pid_t whoami;
	blob_t blob = BLOB_R;
	int count = 0, ii, jj, cmode=0, ttycnt=0;
	FILE** files;
	DIR* dev = opendir("/dev");
	char* loc;
	struct dirent* ent;
	ll_node* head, *cur;
	
	head = malloc(sizeof(ll_node));
	cur = head;
	
	assert(dev);
	
	while ((ent = readdir(dev)) != NULL) {
		if (!strncmp(ent->d_name, "ttys0", 5)) {
			if (ent->d_name[5] >= (char)0x30 && ent->d_name[5] <= (char)0x39) {
				strncpy(cur->fname, ent->d_name, 254);
				printf("%s\n", cur->fname);
				cur->next = malloc(sizeof(ll_node));
				cur = cur->next;
				++ttycnt;
			}
		}
	}
	
	files = malloc(sizeof(FILE*) * ttycnt);
	
#ifdef FORKBG
	whoami = fork();
	if (whoami > 0) {
		return 0;
	}
#endif
	
	cur = head;
	loc = malloc(sizeof(char) * 128);
	strcpy(loc, "/dev/");
	
	for (jj = 0; jj < ttycnt; ++jj) {
		strcpy(&(loc[5]), cur->fname);
		fprintf(stdout, "Capturing: %s\n", loc);
		files[jj] = fopen(loc, "w");
		assert(files[jj]);
		screen_clr(files[jj]);
		cur = cur->next;
	}
	
	setlocale(LC_CTYPE, "en_us.UTF-8");
	
	while (1) {
		for (jj = 0; jj < ttycnt; ++jj) {
			for (ii = 0; ii < count; ++ii) {
				fprintf(files[jj], " ");
			}
			blob_pr(blob, files[jj]);
		}
		usleep(400000);
		if (blob % 2) {
			--blob;
		} else {
			++blob;
		}
		if (cmode) {
			--count;
		} else {
			++count;
		}
		if (count == 10 || count == 0) {
			if (blob > 1) {
				blob = 0;
			} else {
				blob = 2;
			}
			cmode = !cmode;
		}
		for (jj = 0; jj < ttycnt; ++jj) {
			line_clr(files[jj]);
		}
	}
	
	return 0;
}
