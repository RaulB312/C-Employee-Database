TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
		./$(TARGET) -f ./mynewdb.db -n
		xxd mynewdb.db
		./$(TARGET) -f ./mynewdb.db -a "Raul, Deva, 20"
		./$(TARGET) -f ./mynewdb.db -a "Francois, Liege, 40"
		./$(TARGET) -f ./mynewdb.db -a "Jose, Sao Paolo, 160"
		./$(TARGET) -f ./mynewdb.db -a "Miguel, Cordoba, 80"
		./$(TARGET) -f ./mynewdb.db -a "Andrew, London, 60"
		./$(TARGET) -f ./mynewdb.db -a "Umberto, Genova, 56"
		./$(TARGET) -f ./mynewdb.db -a "Georgios, Athena, 70"
		./$(TARGET) -f ./mynewdb.db -a "Albert, Salzburg, 80"
		./$(TARGET) -f ./mynewdb.db -l
		xxd mynewdb.db
		./$(TARGET) -f ./mynewdb.db -u "Raul" 32
		./$(TARGET) -f ./mynewdb.db -l
		./$(TARGET) -f ./mynewdb.db -r "Raul"
		xxd mynewdb.db
		./$(TARGET) -f ./mynewdb.db -l
		./$(TARGET) -f ./mynewdb.db -r "Francois"
		xxd mynewdb.db
		./$(TARGET) -f ./mynewdb.db -l
		./$(TARGET) -f ./mynewdb.db -u "Georgios" 5
		./$(TARGET) -f ./mynewdb.db -l
		xxd mynewdb.db



default: create_dirs $(TARGET)

clean:
		rm -f obj/*.o
		rm -f bin/*
		rm -f *.db

create_dirs:
		mkdir -p obj bin

$(TARGET): $(OBJ)
		gcc -g -o $@ $?

obj/%.o : src/%.c
		gcc -g -c $< -o $@ -Iinclude