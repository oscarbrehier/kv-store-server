CC	= gcc
CFLAGS	= -Wall -Wextra -Werror
LDFLAGS = -L ../kv-store -lkv
VFLAGS 	= --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file="valgrind.dat"
SRCDIR	= srcs
INCDIR	= includes
KV_INCDIR = ../kv-store/includes
BUILDDIR = build
EXEC = server.a
DEBUG ?= 0

ifneq ($(DEBUG), 0)
	CFLAGS += -g3
endif

SRCS	:= $(shell find $(SRCDIR) -type f -name "*.c")

OBJS	:= $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRCS))

all:	${EXEC}

${EXEC}: ${OBJS}
	${CC} ${OBJS} -o ${EXEC} ${LDFLAGS} -lssl -lcrypto

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	${CC} ${CFLAGS} -I ${INCDIR} -I ${KV_INCDIR} -c $< -o $@

clean:
	rm -rf ${BUILDDIR}

fclean:	clean
	rm -f ${EXEC}

debug: fclean ${EXEC} run

run:
	clear
	valgrind ${VFLAGS} ./${EXEC}

re:	fclean all

.PHONY:	all clean fclean re debug run