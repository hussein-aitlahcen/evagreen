rm *.bin
rm id.h
echo "#define AGENT_ID $1" > id.h
gcc agent.c
./a.out