lip: lip.c
	clang \
		-fcolor-diagnostics \
		-fansi-escape-codes \
		-g \
		lip.c \
		-l \
		edit \
		-o \
		lip

clean:
	rm lip
