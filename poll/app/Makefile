TARGET		:= poll_app

SRCS		:= $(TARGET).c
OBJS		:= $(SRCS:.c=.o)

CC		:= gcc
LD		:= gcc
RM		:= rm

CFLAGS		:= -Wall
LDFLAGS		:=

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

clean:
	$(RM) -f $(OBJS)
	$(RM) -f $(TARGET)
