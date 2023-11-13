lip: lip.c
	clang \
		-fcolor-diagnostics \
		-fansi-escape-codes \
		-g \
		lip.c \
		-o \
		lip

clean:
	rm lip