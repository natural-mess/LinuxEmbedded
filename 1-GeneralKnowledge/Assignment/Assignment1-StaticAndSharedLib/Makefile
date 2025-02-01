CUR_DIR := .
SRC_DIR := $(CUR_DIR)/src
INC_DIR := $(CUR_DIR)/inc
BIN_SHARED_DIR := $(CUR_DIR)/bin/shared
BIN_STATIC_DIR := $(CUR_DIR)/bin/static
OBJ_DIR := $(CUR_DIR)/obj
OBJ_SHARED_DIR := $(CUR_DIR)/obj/shared
OBJ_STATIC_DIR := $(CUR_DIR)/obj/static
LIB_DIR := $(CUR_DIR)/lib
LIB_STATIC_DIR := $(LIB_DIR)/static
LIB_SHARED_DIR := $(LIB_DIR)/shared

C_FLAGS := -I $(INC_DIR)

create_dirs:
	@mkdir -p $(SRC_DIR) $(INC_DIR)
	@mkdir -p $(BIN_SHARED_DIR) $(BIN_STATIC_DIR)
	@mkdir -p $(OBJ_DIR) $(OBJ_SHARED_DIR) $(OBJ_STATIC_DIR)
	@mkdir -p $(LIB_DIR) $(LIB_STATIC_DIR) $(LIB_SHARED_DIR)

create_objs:
	# create object files for shared lib
	gcc -c -fPIC $(SRC_DIR)/add.c    -o $(OBJ_SHARED_DIR)/add.o    $(C_FLAGS)
	gcc -c -fPIC $(SRC_DIR)/answer.c -o $(OBJ_SHARED_DIR)/answer.o $(C_FLAGS)
	# create object files for static lib
	gcc -c $(SRC_DIR)/add.c    -o $(OBJ_STATIC_DIR)/add.o    $(C_FLAGS)
	gcc -c $(SRC_DIR)/answer.c -o $(OBJ_STATIC_DIR)/answer.o $(C_FLAGS)
	# create object file for main
	gcc -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o $(C_FLAGS)

create_shared_lib:
	# create shared lib
	gcc -shared $(OBJ_SHARED_DIR)/add.o $(OBJ_SHARED_DIR)/answer.o -o $(LIB_SHARED_DIR)/libAddAnswerShared.so

create_static_lib:
	# create static lib
	ar rcs $(LIB_STATIC_DIR)/libAddAnswerStatic.a $(OBJ_STATIC_DIR)/add.o $(OBJ_STATIC_DIR)/answer.o

all: create_objs create_shared_lib create_static_lib
	# link share lib
	gcc $(OBJ_DIR)/main.o -L$(LIB_SHARED_DIR) -lAddAnswerShared -o $(BIN_SHARED_DIR)/use-shared-library
	# link static lib
	gcc $(OBJ_DIR)/main.o -L$(LIB_STATIC_DIR) -lAddAnswerStatic -o $(BIN_STATIC_DIR)/statically-linked

clean:
	rm -rf $(OBJ_DIR)/*.o
	rm -rf $(OBJ_SHARED_DIR)/*.o
	rm -rf $(OBJ_STATIC_DIR)/*.o
	rm -rf $(LIB_SHARED_DIR)/*.so
	rm -rf $(LIB_STATIC_DIR)/*.a
	rm -rf $(BIN_SHARED_DIR)/use-shared-library
	rm -rf $(BIN_STATIC_DIR)/statically-linked
