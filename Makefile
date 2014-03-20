CC = gcc
SRCCLIENT = .
SRCSERVER = .
BINCLIENT = bin/Client
BINSERVER = bin/Server
OBJCLIENT = obj
OBJSERVER = obj
OBJECTSCLIENT = ${OBJCLIENT}/commons.o ${OBJCLIENT}/client.o
OBJECTSSERVER = ${OBJSERVER}/commons.o ${OBJSERVER}/server.o
EXECUTABLECLIENT = ${BINCLIENT}/client.out
EXECUTABLESERVER = ${BINSERVER}/server.out

all:	clean client server

client:	${OBJECTSCLIENT}
	${CC} $^ -o ${EXECUTABLECLIENT}

server:	${OBJECTSSERVER}
	${CC} $^ -o ${EXECUTABLESERVER} -pthread

${OBJCLIENT}/%.o:	${SRCCLIENT}/%.c
	${CC} -c $< -o $@

${OBJSERVER}/%.o:	${SRCSERVER}/%.c
	${CC} -c $< -o $@

clean:
	rm -f ${OBJCLIENT}/*.o
	rm -f ${OBJSERVER}/*.o
	rm -f ${BINCLIENT}/*.out
	rm -f ${BINSERVER}/*.out