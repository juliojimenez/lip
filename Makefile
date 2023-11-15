lip: lip.c
	clang \
		-fcolor-diagnostics \
		-fansi-escape-codes \
		-g \
		lip.c \
		mpc.c \
		-l \
		edit \
		-l \
		m \
		-o \
		lip

clean:
	rm lip
